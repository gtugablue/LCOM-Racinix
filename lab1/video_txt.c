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

  /* To complete ... */

}


int vt_draw_frame(int width, int height, char attr, int r, int c) {

  /* To complete ... */

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
