#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>

#include "vbe.h"
#include "lmlib.h"

#define LINEAR_MODEL_BIT 14

#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)

int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p) {
	if (lm_init())
	{
		return 1;
	}
	mmap_t map;
	if (lm_alloc(sizeof(vbe_mode_info_t), &map) == NULL)
	{
		return 1;
	}

	struct reg86u reg86;

	reg86.u.b.intno = 0x10; /* BIOS video services */
	reg86.u.w.ax = 0x4F01;
	reg86.u.w.cx = mode;
	reg86.u.w.es = PB2BASE(map.phys);
	reg86.u.w.di = PB2OFF(map.phys);

	if (sys_int86(&reg86) == OK)
	{
		*vmi_p = *((vbe_mode_info_t *)map.virtual);
		lm_free(&map);
		return 0;
	}

	lm_free(&map);
	return 1;
}


