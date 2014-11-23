#include "vbe.h"
#include <minix/drivers.h>
#include <machine/int86.h>

#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)

/* adjust physical address; boot code runs with %ds having a 64k offset */
#define VBEPHYPTR(x)    ((uint8_t *)((x) - (64 * 1024)))

int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p)
{
	mmap_t map;

	if (lm_alloc(sizeof(vbe_mode_info_t), &map) == NULL)
	{
		return 1;
	}

	struct reg86u reg86;

	reg86.u.b.intno = VBE_INTERRUPT_VECTOR; /* BIOS video services */
	reg86.u.b.ah = VBE_FUNCTION;
	reg86.u.b.al = VBE_RETURN_VBE_MODE_INFO;
	reg86.u.w.cx = mode;
	reg86.u.w.es = PB2BASE(map.phys);
	reg86.u.w.di = PB2OFF(map.phys);

	if (sys_int86(&reg86) == OK)
	{
		if (reg86.u.w.ax == VBE_FUNCTION_SUPPORTED | VBE_FUNCTION_CALL_SUCCESSFUL)
		{
			*vmi_p = *((vbe_mode_info_t *)map.virtual);
			lm_free(&map);
			return 0;
		}
	}

	lm_free(&map);
	return 1;
}

int vbe_get_info_block(vbe_info_block_t *vib_p, uint16_t **video_modes, unsigned *num_video_modes)
{
	mmap_t map;
	if (lm_alloc(sizeof(vbe_info_block_t), &map) == NULL)
	{
		return 1;
	}

	memcpy(vib_p->VbeSignature, "VBE2", sizeof("VBE2"));

	struct reg86u reg86;

	reg86.u.b.intno = VBE_INTERRUPT_VECTOR; /* BIOS video services */
	reg86.u.b.ah = VBE_FUNCTION;
	reg86.u.b.al = VBE_RETURN_VBE_CONTROLLER_INFO;
	reg86.u.w.es = PB2BASE(map.phys);
	reg86.u.w.di = PB2OFF(map.phys);

	if (sys_int86(&reg86) != OK)
	{
		return 1;
	}

	if (reg86.u.w.ax != VBE_FUNCTION_SUPPORTED | VBE_FUNCTION_CALL_SUCCESSFUL)
	{
		return 1;
	}

	*vib_p = *((vbe_info_block_t *)map.virtual);

	if (memcmp(vib_p->VbeSignature, "VESA", sizeof(vib_p->VbeSignature)) != 0)
	{
		return 1;
	}

	void *farptr = (void *)(((vib_p->VideoModePtr & 0xffff0000) >> 12) + (vib_p->VideoModePtr & 0xffff) + ((uint32_t)map.virtual & 0xF0000000));

	lm_free(&map);

	int16_t *modes = farptr;
	*num_video_modes = 0;
	size_t i;
	for (i = 0; *modes != VBE_VIDEO_MODE_PTR_TERMINATE; ++modes, *num_video_modes += 1)
	{
		// Find the total number of video modes
	}
	if ((*video_modes = malloc(*num_video_modes * sizeof(uint16_t))) == NULL)
	{
		return 1;
	}
	for (i = 0, modes = farptr; i < *num_video_modes; ++i, ++modes)
	{
		(*video_modes)[i] = *modes;
	}
	return 0;
}
