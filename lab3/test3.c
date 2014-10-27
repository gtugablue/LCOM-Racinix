#include "test3.h"

static int kbd_scan_int_handler();
static void kbd_timer_int_handler();

bool two_byte_scancode = 0;
static unsigned long counter = 0;

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
	if (scancode == TWO_BYTE_SCANCODE)
	{
		two_byte_scancode = true;
		return 0;
	}
	else if (two_byte_scancode)
	{
		scancode |= (TWO_BYTE_SCANCODE << 8);
		two_byte_scancode = false;
	}
	if (IS_BREAK_CODE(scancode))
	{
		printf("breakcode: 0x%X\n", scancode);
		if (scancode == (KEY_ESC | BIT(I8042_BREAK_CODE_BIT)))
		{
			return -1;
		}
	}
	else
	{
		printf("makecode: 0x%X\n", scancode);
	}
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
	int r, ipc_status, scan_result;
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
					if (ass)
					{
						scan_result = kbd_scan_int_handler_asm();
					}
					else
					{
						scan_result = kbd_scan_int_handler();
					}
					if (scan_result == -1)
					{
						break;
					}
					else if (scan_result != OK)
					{
						return 1;
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
	int r, ipc_status, scan_result;
	message msg;
	while (counter < n * TIMER_DEFAULT_FREQ)
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
					counter = 0;
					scan_result = kbd_scan_int_handler();
					if (scan_result == -1)
					{
						break;
					}
					else if (scan_result != OK)
					{
						return 1;
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
