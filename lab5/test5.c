#include "test5.h"
#include <stdlib.h>
#include "xpm.h"
#include <sys/types.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include "vbe.h"

#define TEST_MODE		VBE_MODE_GRAPHICS_1024_786_256

#define BIT(n) 			(0x01<<(n))

void *test_init(unsigned short mode, unsigned short delay)
{
	char *video_mem;
	if ((video_mem = vg_init(mode)) == NULL)
	{
		return NULL;
	}

	vbe_mode_info_t vbe_mode_info;
	if (vbe_get_mode_info(mode, &vbe_mode_info)) // We are running this command again but this way we avoid having to pass a vbe_mode_info_t struct by reference to vg_init, which sometimes may not be needed
	{
		return NULL;
	}

	unsigned char timer_hook_bit;
	if ((timer_hook_bit = timer_subscribe_int()) < 0)
	{
		return NULL;
	}

	int r, ipc_status;
	message msg;
	unsigned timer_counter = 0;
	while(timer_counter < delay * TIMER_DEFAULT_FREQ)
	{
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			// Driver receive fail
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			if (_ENDPOINT_P(msg.m_source) == HARDWARE) /* hardware interrupt notification */
			{
				if (msg.NOTIFY_ARG & BIT(timer_hook_bit)) {
					++timer_counter;
				}
			}
		}
	}

	if (timer_unsubscribe_int())
	{
		return NULL;
	}

	if (vg_exit())
	{
		return NULL;
	}
	else
	{
		printf("\nPhysical address of the video RAM: 0x%X\n", vbe_mode_info.PhysBasePtr);
		return video_mem;
	}
}

int test_square(unsigned short x, unsigned short y, unsigned short size, unsigned long color)
{
	char *video_mem;
	if (vg_exit()) // To fix the bug that makes colors darker the first time Minix enters graphics mode
	{
		return 1;
	}
	if ((video_mem = vg_init(TEST_MODE)) == NULL)
	{
		return 1;
	}
	vg_draw_square(x, y, size, color);

	if (keyboard_subscribe_int() == -1)
	{
		return 1;
	}
	int r, ipc_status, scan_result;
	message msg;
	bool pressed = false;
	while (1)
	{
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			// Driver receive fail
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			if (_ENDPOINT_P(msg.m_source) == HARDWARE) /* hardware interrupt notification */
			{
				if (msg.NOTIFY_ARG & BIT(KEYBOARD_HOOK_BIT)) {
					if (keyboard_int_handler())
					{
						return 1;
					}
					if (kbd_keys[KEY_ESC].pressed)
					{
						pressed = true;
						continue;
					}
					if (!kbd_keys[KEY_ESC].pressed && pressed)
					{
						break;
					}
				}
			}
		}
	}
	if (keyboard_unsubscribe_int())
	{
		return 1;
	}
	return vg_exit();
}

int test_line(unsigned short xi, unsigned short yi, unsigned short xf, unsigned short yf, unsigned long color)
{
	if (vg_exit()) // To fix the bug that makes colors darker the first time Minix enters graphics mode
	{
		return 1;
	}
	char *video_mem;
	if ((video_mem = vg_init(TEST_MODE)) == NULL)
	{
		return 1;
	}
	vg_draw_line(xi, yi, xf, yf, color);

	if (keyboard_subscribe_int() == -1)
	{
		return 1;
	}
	int r, ipc_status, scan_result;
	message msg;
	bool pressed = false;
	while (1)
	{
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			// Driver receive fail
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			if (_ENDPOINT_P(msg.m_source) == HARDWARE) /* hardware interrupt notification */
			{
				if (msg.NOTIFY_ARG & BIT(KEYBOARD_HOOK_BIT)) {
					if (keyboard_int_handler())
					{
						return 1;
					}
					if (kbd_keys[KEY_ESC].pressed)
					{
						pressed = true;
						continue;
					}
					if (!kbd_keys[KEY_ESC].pressed && pressed)
					{
						break;
					}
				}
			}
		}
	}
	if (keyboard_unsubscribe_int())
	{
		return 1;
	}

	return vg_exit();
}

int test_xpm(unsigned short xi, unsigned short yi, char *xpm[])
{
	char *video_mem;
	if ((video_mem = vg_init(TEST_MODE)) == NULL)
	{
		return 1;
	}
	vbe_mode_info_t vbe_mode_info;
	if (vbe_get_mode_info(TEST_MODE, &vbe_mode_info)) // We are running this command again but this way we avoid having to pass a vbe_mode_info_t struct by reference to vg_init, which sometimes may not be needed
	{
		return 1;
	}
	int width, height;
	char* pixmap = read_xpm(xpm, &width, &height, vbe_mode_info.XResolution, vbe_mode_info.YResolution);
	vg_draw_pixmap(xi, yi, pixmap, width, height);
	size_t i;

	if (keyboard_subscribe_int() == -1)
	{
		return 1;
	}
	int r, ipc_status, scan_result;
	message msg;
	bool pressed = false;
	while (1)
	{
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			// Driver receive fail
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			if (_ENDPOINT_P(msg.m_source) == HARDWARE) /* hardware interrupt notification */
			{
				if (msg.NOTIFY_ARG & BIT(KEYBOARD_HOOK_BIT)) {
					if (keyboard_int_handler())
					{
						return 1;
					}
					if (kbd_keys[KEY_ESC].pressed)
					{
						pressed = true;
						continue;
					}
					if (!kbd_keys[KEY_ESC].pressed && pressed)
					{
						break;
					}
				}
			}
		}
	}
	if (keyboard_unsubscribe_int())
	{
		return 1;
	}

	return vg_exit();
}	

int test_move(unsigned short xi, unsigned short yi, char *xpm[], unsigned short hor, short delta, unsigned short time)
{
	/* To be completed */
	
}					

int test_controller()
{
	vbe_info_block_t vbe_info_block;
	if (vbe_get_info_block(&vbe_info_block))
	{
		return 1;
	}
	printf("Video modes:\n");

	void *farptr = vbe_farptr(vbe_info_block.VideoModePtr);
	size_t i;
	for (i = 0; *((uint16_t *)farptr + i) != VBE_VIDEO_MODE_PTR_TERMINATE; ++i)
	{
		printf("0x%X\n", *((uint16_t *)farptr + i));
	}
	return 0;
}
