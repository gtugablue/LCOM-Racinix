#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "vbe.h"
#include "video_gr.h"

/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000 
 * Currently on lab I304 is 0xF0000000
 * Better run my version of lab2 as follows:
 *     service run `pwd`/lab2 -args "mode 0x105"
 */
#define VRAM_PHYS_ADDR	0xF0000000
#define H_RES             1024
#define V_RES		  768
#define BITS_PER_PIXEL	  8

/* Private global variables */

static char *video_mem;		/* Process address to which VRAM is mapped */

static unsigned h_res;		/* Horizontal screen resolution in pixels */
static unsigned v_res;		/* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

void * vg_init(unsigned short mode) {
	struct reg86u reg86;

	reg86.u.b.intno = 0x10; /* BIOS video services */
	reg86.u.w.ax = 0x4F02;
	reg86.u.w.bx = mode | (1 << 14);

	if (sys_int86(&reg86) == OK)
	{
		vbe_mode_info_t *vmi_p;
		if (vmi_p = malloc(sizeof(vbe_mode_info_t)))
		{
			if(vbe_get_mode_info(mode, vmi_p))
			{
				return NULL;
			}
			else
			{
				int r;
				struct mem_range mr;
				unsigned mr_size;

				h_res = vmi_p->XResolution;
				v_res = vmi_p->YResolution;
				bits_per_pixel = vmi_p->BitsPerPixel;

				/* Allow memory mapping */

				mr.mr_base = vmi_p->PhysBasePtr;
				mr_size = h_res * v_res * bits_per_pixel;
				mr.mr_limit = mr.mr_base + mr_size;

				if(sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))
				{
					return NULL;
				}

				/* Map memory */

				video_mem = vm_map_phys(SELF, (void *)mr.mr_base, mr_size);

				if(video_mem == MAP_FAILED)
				{
					return NULL;
				}

				return video_mem;
			}
		}
	}
	return NULL;
}

int vg_fill(unsigned long color) {

	if (color >= (1 << 8))
	{
		return 1;
	}

	char *pixel;
	for (pixel = video_mem; pixel < video_mem + h_res * v_res; ++pixel)
	{
		*pixel = color;
	}

	return 0;
}

int vg_set_pixel(unsigned long x, unsigned long y, unsigned long color) {
	if(x <= h_res && y <= v_res)
	{
		*(video_mem + x + y * h_res) = (char)color;
		return 0;
	}
	return 1;
}

long vg_get_pixel(unsigned long x, unsigned long y) {
	return 0;
}

static void swap(unsigned long* a, unsigned long* b)
{
	int t=*a;
	*a=*b;
	*b=t;
}

int vg_draw_line(unsigned long xi, unsigned long yi, unsigned long xf, unsigned long yf, unsigned long color)
{
	if (xi >= h_res || xf >= h_res || yi >= v_res || yf >= v_res)
	{
		return 1;
	}

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

int vg_draw_circle(unsigned long x0, unsigned long y0, unsigned long radius, unsigned long color)
{
	// Midpoint circle algorithm

	int y = 0;
	int error = 1 - radius;

	while (radius >= y)
	{
		vg_draw_line(-radius + x0, y + y0, radius + x0, y + y0, color);
		vg_draw_line(-y + x0, radius + y0, y + x0, radius + y0, color);
		vg_draw_line(-radius + x0, -y + y0, radius + x0, -y + y0, color);
		vg_draw_line(-y + x0, -radius + y0, y + x0, -radius + y0, color);
		y++;
		if (error < 0)
		{
			error += 2 * y + 1;
		}
		else
		{
			radius--;
			error += 2 * (y - radius + 1);
		}
	}

	return 0;
}

int vg_exit() {
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

bool isPointInPolygon(vector2D_t polygon[], unsigned polygon_size, vector2D_t* point)
{
	int i, j;
	bool c = false;
	for (i = 0, j = polygon_size - 1; i < polygon_size; j = i++) {
		if (((polygon[i].y > point->y) != (polygon[j].y > point->y)) && (point->x < (polygon[j].x - polygon[i].x) * (point->y - polygon[i].y) / (polygon[j].y - polygon[i].y) + polygon[i].x))
		{
			c = !c;
		}
	}
	return c;
}
