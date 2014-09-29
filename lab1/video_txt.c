#include <minix/drivers.h>
#include <sys/video.h>
#include <sys/mman.h>

#include <assert.h>

#include "vt_info.h"

#include "video_txt.h"

/* Private global variables */

static char *video_mem;		/* Address to which VRAM is mapped */

static unsigned scr_width;	/* Width of screen in columns */
static unsigned scr_lines;	/* Height of screen in lines */

void vt_fill(char ch, char attr) {
  
  char *p;
  for (p = video_mem; p < video_mem + 2 * (scr_width * scr_lines); ++p)
  {
	  *p = ch;
	  *(++p) = attr;
  }

  return;
}

void vt_blank() {
	vt_fill(0x0, 0x0);

	return;

}

int vt_print_char(char ch, char attr, int r, int c) {

	if (r >= scr_width || c >= scr_width)
	{
		return 1;
	}
	char *p = video_mem + 2 * r * scr_width + 2 * c;
	*p = ch;
	*(++p) = attr;

	return 0;

}

int vt_print_string(char *str, char attr, int r, int c) {
	char *strp = str;
	unsigned length = 0;
	while (*strp != 0)
	{
		length++;
		++strp;
	}
	if (r >= scr_width || c + length > scr_width)
	{
		return 1;
	}

	char *vptr = video_mem + 2 * r * scr_width + 2 * c;
	for (strp = str; strp < str + length; ++strp, ++vptr)
	{
		*vptr = *strp;
		*(++vptr) = attr;
	}

	return 0;
}

int vt_print_int(int num, char attr, int r, int c) {
	if (num > 0)
	{
		int digitCounter = 1;
		int tempnum = num;
		while (tempnum > 9)
		{
			tempnum /= 10;
			digitCounter++;
		}
		char texto[digitCounter + 1];
		texto[digitCounter--] = '\0';
		while (digitCounter >= 0)
		{
			texto[digitCounter--] = num % 10 + '0';
			num /= 10;
		}
		vt_print_string(texto, attr, r, c);

	}
	else if (num < 0)
	{
		return 1;
	}
	else
	{
		return 1;
	}
	return 0;
}

int vt_draw_frame(int width, int height, char attr, int r, int c) {
	if (width >= 2 && height >= 2 && width + c <= scr_width && height + r <= scr_width)
	{
		vt_print_char(UL_CORNER, attr, r, c);
		size_t i;
		for (i = c + 1; i < c + width; i++)
		{
			vt_print_char(HOR_BAR, attr, r, i);
			vt_print_char(HOR_BAR, attr, r + height, i);
		}
		vt_print_char(UR_CORNER, attr, r, c + width);
		for (i = r + 1; i < r + height; i++)
		{
			vt_print_char(VERT_BAR, attr, i, c);
			vt_print_char(VERT_BAR, attr, i, c + width);
		}
		vt_print_char(LL_CORNER, attr, r + height, c);
		vt_print_char(LR_CORNER, attr, r + height, c + width);
		return 0;
	}
	else
	{
		return 1;
	}
}

/*
 * THIS FUNCTION IS FINALIZED, do NOT touch it
 */

char *vt_init(vt_info_t *vi_p) {

  int r;
  struct mem_range mr;

  /* Allow memory mapping */

  mr.mr_base = (phys_bytes)(vi_p->vram_base);
  mr.mr_limit = mr.mr_base + vi_p->vram_size;

  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
	  panic("video_txt: sys_privctl (ADD_MEM) failed: %d\n", r);

  /* Map memory */

  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vi_p->vram_size);

  if(video_mem == MAP_FAILED)
	  panic("video_txt couldn't map video memory");

  /* Save text mode resolution */

  scr_lines = vi_p->scr_lines;
  scr_width = vi_p->scr_width;

  return video_mem;
}
