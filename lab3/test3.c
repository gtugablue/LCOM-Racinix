#include "test3.h"

#define KEY_ESC		0x01

static int kbd_int_handler();
static int kbd_scancode_handler(unsigned long scancode);
static int kbd_response_handler(unsigned long response);
static int kbd_test_timer_handler();

static int kbd_int_handler()
{
	unsigned long interruption;
	if (sys_inb(I8042_OUT_BUF, &interruption))
	{
		return 1;
	}
	if (kbd_is_response(interruption))
	{
		return kbd_response_handler(interruption);
	}
	else
	{
		return kbd_scancode_handler(interruption);
	}
}

static int kbd_scancode_handler(unsigned long scancode)
{
	if (IS_BREAK_CODE(scancode))
	{
		printf("breakcode: 0x%X\n", scancode);
		if (scancode == (KEY_ESC | BIT(I8042_BREAK_CODE_BIT)))
		{
			return 1;
		}
	}
	else
	{
		printf("makecode: 0x%X\n", scancode);
	}
	return 0;
}

static int kbd_response_handler(unsigned long response)
{
	return 0;
}

static int kbd_test_timer_handler()
{
	return 0;
}

int kbd_test_scan(unsigned short ass)
{
	if (kbd_subscribe_int() == -1)
	{
		return 1;
	}
	int r, ipc_status;
	message msg;
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
				if (msg.NOTIFY_ARG & BIT(I8042_HOOK_BIT)) {
					if (kbd_int_handler())
					{
						break;
					}
				}
			}
		}
	}
	return 0;
}

int kbd_test_leds(unsigned short n, unsigned short *leds) {
	/* To be completed */
}
int kbd_test_timed_scan(unsigned short n) {
	unsigned char timer_hook_bit;
	if (timer_hook_bit = timer_subscribe_int())
	{
		return 1;
	}
	if (kbd_subscribe_int() == -1)
	{
		return 1;
	}
	int r, ipc_status;
	message msg;
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
				if (msg.NOTIFY_ARG & BIT(I8042_HOOK_BIT)) {
					if (kbd_int_handler())
					{
						break;
					}
				}
				if (msg.NOTIFY_ARG & timer_hook_bit) {
					if (kbd_test_timer_handler())
					{
						break;
					}
				}
			}
		}
	}
	return 0;
}
