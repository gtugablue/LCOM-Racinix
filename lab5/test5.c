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
	if (lm_init())
	{
		return NULL;
	}
	if (vg_exit()) // To fix the bug that makes colors darker the first time Minix enters graphics mode
	{
		return NULL;
	}
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
	if (lm_init())
	{
		return 1;
	}
	char *video_mem;
	if (vg_exit()) // To fix the bug that makes colors darker the first time Minix enters graphics mode
	{
		return 1;
	}
	if ((video_mem = vg_init(TEST_MODE)) == NULL)
	{
		return 1;
	}
	vg_draw_rectangle(x, y, size, size, color);
	vg_swap_buffer();
	vg_swap_mouse_buffer();
	if (keyboard_subscribe_int() == -1)
	{
		return 1;
	}
	int r, ipc_status;
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
	return keyboard_unsubscribe_int() | vg_exit();
}

int test_line(unsigned short xi, unsigned short yi, unsigned short xf, unsigned short yf, unsigned long color)
{
	if (lm_init())
	{
		return 1;
	}
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
	vg_swap_buffer();
	vg_swap_mouse_buffer();
	if (keyboard_subscribe_int() == -1)
	{
		return 1;
	}
	int r, ipc_status;
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
	return keyboard_unsubscribe_int() | vg_exit();
}

int test_xpm(unsigned short xi, unsigned short yi, char *xpm[])
{
	if (lm_init())
	{
		return 1;
	}
	if (vg_exit()) // To fix the bug that makes colors darker the first time Minix enters graphics mode
	{
		return 1;
	}
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
	vg_swap_buffer();
	vg_swap_mouse_buffer();
	free(pixmap);
	if (keyboard_subscribe_int() == -1)
	{
		return 1;
	}
	int r, ipc_status;
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
	return keyboard_unsubscribe_int() | vg_exit();
}	

int test_move(unsigned short xi, unsigned short yi, char *xpm[], unsigned short hor, short delta, unsigned short time)
{
	if (lm_init())
	{
		return 1;
	}
	if (vg_exit()) // To fix the bug that makes colors darker the first time Minix enters graphics mode
	{
		return 1;
	}
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
	unsigned char timer_hook_bit;
	if ((timer_hook_bit = timer_subscribe_int()) < 0)
	{
		return 1;
	}
	if (keyboard_subscribe_int() == -1)
	{
		return 1;
	}
	int r, ipc_status;
	message msg;
	bool pressed = false;
	unsigned counter = 0;
	if (keyboard_subscribe_int() == -1)
	{
		return 1;
	}
	vector2D_t velocity = vectorCreate(0, 0);
	if (hor == 0)
	{
		velocity.x = (double)delta / time;
	}
	else
	{
		velocity.y = (double)delta / time;
	}
	Sprite *sprite = create_sprite(xpm, xi, yi, velocity.x, velocity.y, vbe_mode_info.XResolution, vbe_mode_info.YResolution);
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
				if (msg.NOTIFY_ARG & BIT(timer_hook_bit) && counter <= TIMER_DEFAULT_FREQ * time)
				{
					// New frame
					clear_sprite_area(sprite);
					animate_sprite(sprite, 1.0 / TIMER_DEFAULT_FREQ);
					++counter;

					// Swap buffers
					vg_swap_buffer();
					vg_swap_mouse_buffer();
				}
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
	destroy_sprite(sprite);
	return timer_unsubscribe_int() | keyboard_unsubscribe_int() | vg_exit();
}					

int test_controller()
{
	if (lm_init())
	{
		return 1;
	}
	static uint16_t *video_modes;
	vbe_info_block_t vbe_info_block;
	unsigned num_video_modes;
	if (vbe_get_info_block(&vbe_info_block, &video_modes, &num_video_modes))
	{
		return 1;
	}
	printf("Controller capabilities: 0x%X\n", vbe_info_block.Capabilities);
	if (vbe_info_block.Capabilities & BIT(VBE_CONTROLLER_CAPABILITIES_DAC_SWITCHABLE_WIDTH_BIT))
	{
		printf("\tDAC width is switchable to 8 bits per primary color\n");
	}
	else
	{
		printf("\tDAC is fixed width, with 6 bits per primary color\n");
	}
	if (vbe_info_block.Capabilities & BIT(VBE_CONTROLLER_CAPABILITIES_NOT_VGA_BIT))
	{
		printf("\tController is not VGA compatible\n");
	}
	else
	{
		printf("\tController is VGA compatible\n");
	}
	if (vbe_info_block.Capabilities & BIT(VBE_CONTROLLER_CAPABILITIES_RAMDAC_USE_BLANK_BIT))
	{
		printf("\tWhen programming large blocks of information to the RAMDAC, the blank bit must be used in function 0x09\n");
	}
	else
	{
		printf("\tNormal RAMDAC operation\n");
	}
	printf("\nVideo modes: ");
	size_t i;
	if (num_video_modes > 0)
	{
		printf("0x%X", video_modes[0]);
	}
	for (i = 1; i < num_video_modes; ++i)
	{
		printf(", 0x%X", video_modes[i]);
	}
	free(video_modes);
	printf("\n\nSize of VRAM memory: %lu KB\n", vbe_info_block.TotalMemory * 64);
	return 0;
}
