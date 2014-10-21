#include "test3.h"

static int kbd_scan_int_handler();
static void kbd_timer_int_handler();

static unsigned long counter;

static void kbd_timer_int_handler()
{
	++counter;		// If the counter overflows it goes back to 0
	return;
}

static int kbd_scan_int_handler()
{
	unsigned long scancode;
	if (sys_inb(I8042_OUT_BUF, &scancode))
	{
		return 1;
	}
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
				if (msg.NOTIFY_ARG & BIT(KBD_HOOK_BIT)) {
					if (kbd_scan_int_handler())
					{
						break;
					}
				}
			}
		}
	}
	return kbd_unsubscribe_int();
}

int kbd_test_leds(unsigned short n, unsigned char *leds) {
	unsigned char timer_hook_bit;
	if ((timer_hook_bit = timer_subscribe_int()) < 0)
	{
		return 1;
	}
	int r, ipc_status;
	message msg;
	counter = 0;
	while(counter < n * TIMER_DEFAULT_FREQ)
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
					kbd_timer_int_handler();
					if ((counter % TIMER_DEFAULT_FREQ) == 0)
					{
						if (kbd_toggle_leds(TOGGLE_LED(leds[counter / TIMER_DEFAULT_FREQ], kbd_get_led_status())))
						{
							return 1;
						}
					}
				}
			}
		}
	}
	return 0;
}
int kbd_test_timed_scan(unsigned short n) {
	unsigned char timer_hook_bit;
	if ((timer_hook_bit = timer_subscribe_int()) < 0)
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
				if (msg.NOTIFY_ARG & BIT(KBD_HOOK_BIT)) {
					if (kbd_scan_int_handler())
					{
						break;
					}
				}
				if (msg.NOTIFY_ARG & BIT(timer_hook_bit)) {
					kbd_timer_int_handler();
				}
			}
		}
	}
	return 0;
}
