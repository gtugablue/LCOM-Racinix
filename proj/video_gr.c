#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "math.h"
#include "vbe.h"
#include "video_gr.h"

#define PI	3.141592653589793238463

#define BIT(n) (0x01<<(n))

/* Constants for VBE 0x105 mode */

/* Private global variables */

static char *video_mem;		/* Process address to which VRAM is mapped */

static unsigned h_res;		/* Horizontal screen resolution in pixels */
static unsigned v_res;		/* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

static char *double_buffer;

void *vg_init(unsigned short mode)
{
	struct reg86u reg86;

	reg86.u.b.intno = VBE_INTERRUPT_VECTOR; /* BIOS video services */
	reg86.u.b.ah = VBE_FUNCTION;
	reg86.u.b.al = VBE_SET_VBE_MODE;
	reg86.u.w.bx = mode | BIT(VBE_MODE_NUMBER_LINEAR_FLAT_FRAME_BUFFER_BIT);

	vbe_mode_info_t vbe_mode_info;
	if (sys_int86(&reg86) == OK)
	{
		if (reg86.u.w.ax == VBE_FUNCTION_SUPPORTED | VBE_FUNCTION_CALL_SUCCESSFUL)
		{
			if(vbe_get_mode_info(mode, &vbe_mode_info))
			{
				return NULL;
			}
			else
			{
				int r;
				struct mem_range mr;
				unsigned mr_size;

				h_res = vbe_mode_info.XResolution;
				v_res = vbe_mode_info.YResolution;
				bits_per_pixel = vbe_mode_info.BitsPerPixel;

				/* Allow memory mapping */

				mr.mr_base = vbe_mode_info.PhysBasePtr;
				mr_size = h_res * v_res * bits_per_pixel;
				mr.mr_limit = mr.mr_base + mr_size;

				if(sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))
				{
					return NULL;
				}

				/* Map memory */

				video_mem = vm_map_phys(SELF, (void *)mr.mr_base, mr_size);

				if(video_mem != MAP_FAILED)
				{
					if ((double_buffer = malloc(h_res * v_res * bits_per_pixel / 8)) != NULL)
					{
						return video_mem;
					}
				}
			}
		}
	}
	return NULL;
}

int vg_fill(unsigned long color)
{
	char *pixel;
	for (pixel = double_buffer; pixel < double_buffer + h_res * v_res; ++pixel)
	{
		*pixel = color;
	}

	return 0;
}

inline int vg_set_pixel(unsigned long x, unsigned long y, unsigned long color) {
	if(x <= h_res && y <= v_res)
	{
		if (color != VIDEO_GR_TRANSPARENT)
		*(double_buffer + x + y * h_res) = (char)color;
		return 0;
	}
	return 1;
}

inline long vg_get_pixel(unsigned long x, unsigned long y) {
	if (x <= h_res && y <= v_res)
	{
		return *(double_buffer + x + y * h_res);
	}
	return 0;
}

static void swap(unsigned long* a, unsigned long* b)
{
	int t=*a;
	*a=*b;
	*b=t;
}

int vg_draw_line(long xi, long yi, long xf, long yf, long color)
{
	// Bresenham's line algorithm

	// TODO ALTERAR ESTE CÓDIGO!!! ARRANJAR UM ALGORITMO MEU!!

	unsigned long dx,dy;
	int d,incry,incre,incrne,slopegt1=0;
	dx=abs(xi-xf);dy=abs(yi-yf);
	if(dy>dx)
	{
		swap(&xi,&yi);
		swap(&xf,&yf);
		swap(&dx,&dy);
		slopegt1=1;
	}
	if(xi>xf)
	{
		swap(&xi,&xf);
		swap(&yi,&yf);
	}
	if(yi>yf)
		incry=-1;
	else
		incry=1;
	d=2*dy-dx;
	incre=2*dy;
	incrne=2*(dy-dx);
	while(xi<xf)
	{
		if(d<=0)
			d+=incre;
		else
		{
			d+=incrne;
			yi+=incry;
		}
		xi++;
		if(slopegt1)
			vg_set_pixel(yi,xi,color);
		else
			vg_set_pixel(xi,yi,color);
	}

	return 0;
}

int vg_draw_square(unsigned long x, unsigned long y, unsigned long size, unsigned long color)
{
	size_t i, j;
	for (i = x; i < x + size; ++i)
	{
		for (j = y; j < y + size; ++j)
		{
			vg_set_pixel(i, j, color);
		}
	}
	return 0;
}

int vg_draw_circle(unsigned long x0, unsigned long y0, unsigned long radius, unsigned long color)
{
	// Midpoint circle algorithm

	int x = radius;
	int y = 0;
	int error = 1 - x;

	while (x >= y)
	{
		vg_draw_line(-x + x0, y + y0, x + x0, y + y0, color);
		vg_draw_line(-y + x0, x + y0, y + x0, x + y0, color);
		vg_draw_line(-x + x0, -y + y0, x + x0, -y + y0, color);
		vg_draw_line(-y + x0, -x + y0, y + x0, -x + y0, color);
		y++;
		if (error < 0)
		{
			error += 2 * y + 1;
		}
		else
		{
			x--;
			error += 2 * (y - x + 1);
		}
	}

	return 0;
}

int vg_draw_pixmap(unsigned long x, unsigned long y, char *pixmap, unsigned short width, unsigned short height)
{
	size_t i, j;
	for (i = 0; i < width; ++i)
	{
		for (j = 0; j < height; ++j)
		{
			vg_set_pixel(x + i, y + j, *(pixmap + i + j * width));
		}
	}
}

int vg_draw_polygon(vector2D_t polygon[], unsigned n, unsigned long color)
{
	size_t i, j;
	vector2D_t min = polygon[0];
	vector2D_t max = polygon[0];
	for (i = 1; i < n; ++i)
	{
		if (polygon[i].x < min.x)
		{
			min.x = polygon[i].x;
		}
		else if (polygon[i].x > max.x)
		{
			max.x = polygon[i].x;
		}
		if (polygon[i].y < min.y)
		{
			min.y = polygon[i].y;
		}
		else if (polygon[i].y > max.y)
		{
			max.x = polygon[i].y;
		}
	}
	vector2D_t point;
	for (i = min.x; i < max.x; ++i)
	{
		for (j = min.y; j < max.y; ++j)
		{
			point = vectorCreate(i, j);
			if (isPointInPolygon(polygon, n, &point))
			{
				vg_set_pixel(i, j, color);
			}
		}
	}
}

void vg_swap_buffer()
{
	memcpy(video_mem, double_buffer, h_res * v_res * bits_per_pixel / 8);
}

int vg_exit() {
	free(double_buffer);
	struct reg86u reg86;

	reg86.u.b.intno = 0x10; /* BIOS video services */

	reg86.u.b.ah = 0x00;    /* Set Video Mode function */
	reg86.u.b.al = 0x03;    /* 80x25 text mode*/

	if( sys_int86(&reg86) != OK ) {
		printf("\tvg_exit(): sys_int86() failed \n");
		return 1;
	} else
		return 0;
}
