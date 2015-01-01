#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "math.h"
#include "video_gr.h"

#define PI	3.141592653589793238463

#define BIT(n) (0x01<<(n))

/* Constants for VBE 0x105 mode */

/* Private global variables */

static uint16_t *video_mem;		/* Process address to which VRAM is mapped */

unsigned h_res;		/* Horizontal screen resolution in pixels, not static because _video_gr.S needs it */
unsigned v_res;		/* Vertical screen resolution in pixels, not static because _video_gr.S needs it */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

uint16_t *double_buffer = NULL; // Not static because _video_gr.S needs it
static uint16_t *mouse_buffer = NULL;

static vbe_mode_info_t vbe_mode_info;

void *vg_init(unsigned short mode)
{
	struct reg86u reg86;

	reg86.u.b.intno = VBE_INTERRUPT_VECTOR; /* BIOS video services */
	reg86.u.b.ah = VBE_FUNCTION;
	reg86.u.b.al = VBE_SET_VBE_MODE;
	reg86.u.w.bx = mode | BIT(VBE_MODE_NUMBER_LINEAR_FLAT_FRAME_BUFFER_BIT);
	if (sys_int86(&reg86) == OK)
	{
		if (reg86.u.w.ax == (VBE_FUNCTION_SUPPORTED | VBE_FUNCTION_CALL_SUCCESSFUL))
		{
			if(vbe_get_mode_info(mode, &vbe_mode_info))
			{
				return NULL;
			}
			else
			{
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
						if ((mouse_buffer = malloc(h_res * v_res * bits_per_pixel / 8)) != NULL)
						{
							return video_mem;
						}
					}
				}
			}
		}
	}
	return NULL;
}

int vg_fill(uint16_t color)
{
	/*uint16_t *pixel;
	for (pixel = double_buffer; pixel < double_buffer + h_res * v_res; ++pixel)
	{
	 *pixel = color;
	}*/
	memset16(double_buffer, color, h_res * v_res);
	return 0;
}

inline vg_set_pixel(unsigned long x, unsigned long y, uint16_t color)
{
	if (x < h_res && y < v_res) // Since the function arguments are of the "unsigned long" type, this check is enough and therefore performance is increased
	{
		*(double_buffer + (x + y * h_res)) = color;
		return 0;
	}
	return 1;
}

inline int vg_set_mouse_pixel(unsigned long x, unsigned long y, uint16_t color)
{
	if(color != VIDEO_GR_64K_TRANSPARENT && x < h_res && y < v_res)
	{
		*(mouse_buffer + (x + y * h_res)) = color;
		return 0;
	}
	return 1;
}

inline long vg_get_pixel(unsigned long x, unsigned long y) {
	if (x <= h_res && y <= v_res)
	{
		return *(double_buffer + (x + y * h_res));
	}
	return 0;
}

static void swap(long* a, long* b)
{
	int t=*a;
	*a=*b;
	*b=t;
}

int vg_draw_line(long xi, long yi, long xf, long yf, long color)
{
	//Bresenham's line algorithm
	long dx,dy;
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

int vg_draw_rectangle(unsigned long x, unsigned long y, unsigned long width, unsigned long height, uint16_t color)
{
	/*size_t i, j;
	for (i = x; i < x + width; ++i)
	{
		for (j = y; j < y + height; ++j)
		{
			vg_set_pixel(i, j, color);
		}
	}*/
	size_t i;
	for (i = y; i < y + height; ++i)
	{
		memset16(double_buffer + x + i * vbe_mode_info.XResolution, color, width);
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

int vg_draw_pixmap(unsigned long x, unsigned long y, uint16_t *pixmap, unsigned short width, unsigned short height)
{
	size_t i, j;
	for (i = 0; i < width; ++i)
	{
		for (j = 0; j < height; ++j)
		{
			vg_set_pixel(x + i, y + j, *(pixmap + (i + j * width)));
		}
	}
	return 0;
}

void vg_draw_mouse(unsigned long x, unsigned long y, bitmap_t *bitmap)
{
	size_t i, j;
	for (i = x; i < x + bitmap->bitmap_information_header.width; ++i)
	{
		for (j = y; j < y + bitmap->bitmap_information_header.height; ++j)
		{
			vg_set_mouse_pixel(i, 2 * y + bitmap->bitmap_information_header.height - j, *((uint16_t *)bitmap->pixel_array + (i - x) + (j - y) * (bitmap->bitmap_information_header.width)));
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
			max.y = polygon[i].y;
		}
	}
	vector2D_t point;
	size_t count = 0;
	for (i = min.x; i < max.x; ++i)
	{
		for (j = min.y; j < max.y; ++j)
		{
			point = vectorCreate(i, j);
			++count;
			if (isPointInPolygon(polygon, n, point))
			{
				vg_set_pixel(i, j, color);
			}
		}
	}
	return 0;
}

vbe_mode_info_t *vg_get_vbe_mode_info()
{
	return &vbe_mode_info;
}

void *vg_get_double_buffer()
{
	return double_buffer;
}

void vg_swap_buffer()
{
	memcpy(mouse_buffer, double_buffer, h_res * v_res * bits_per_pixel / 8);
}

void vg_swap_mouse_buffer()
{
	memcpy(video_mem, mouse_buffer, h_res * v_res * bits_per_pixel / 8);
}

uint16_t rgb(unsigned char r, unsigned char g, unsigned char b)
{
	uint16_t c = 0x0;
	c |= (r * (1 << vbe_mode_info.RedMaskSize) / VIDEO_GR_RGB_MAX) << vbe_mode_info.RedFieldPosition;
	c |= (g * (1 << vbe_mode_info.GreenMaskSize) / VIDEO_GR_RGB_MAX) << vbe_mode_info.GreenFieldPosition;
	c |= (b * (1 << vbe_mode_info.BlueMaskSize) / VIDEO_GR_RGB_MAX) << vbe_mode_info.BlueFieldPosition;
	return c;
}

int vg_exit() {
	free(double_buffer);
	double_buffer = NULL;
	free(mouse_buffer);
	mouse_buffer = NULL;
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
