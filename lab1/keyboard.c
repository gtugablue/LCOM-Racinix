#include "keyboard.h"

#define BIT(n) (0x01<<(n))

#define KBC_TRIES			10

static int keyboard_wait_for_response(unsigned current_try);
static int keyboard_send_argument(unsigned long argument, unsigned current_try);
static int keyboard_update_key(unsigned long makecode, bool pressed);

static int irq_hook_id;
static unsigned char led_status = 0;

int keyboard_is_response(unsigned long interruption)
{
	if(interruption == I8042_KBD_RESPONSE_ACK || interruption == I8042_KBD_RESPONSE_RESEND || interruption == I8042_KBD_RESPONSE_ERROR)
	{
		return 1;
	}
	return 0;
}

static int keyboard_wait_for_response(unsigned current_try)
{
	unsigned long status;
	while (current_try < KBC_TRIES)
	{
		tickdelay(micros_to_ticks(I8042_KBD_TIMEOUT));
		if (keyboard_read_status(&status))
		{
			return 1;
		}
		if (!(status & BIT(I8042_STATUS_TIMEOUT_BIT)))
		{
			break;
		}
		++current_try;
	}
	if (current_try == KBC_TRIES)
	{
		return 1; // Command failed after KBC_TRIES tries
	}
	return 0;
}

int keyboard_subscribe_int()
{
	irq_hook_id = KEYBOARD_HOOK_BIT;
	if (sys_irqsetpolicy(I8042_KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &irq_hook_id) == OK)
	{
		memset(&kbd_keys, 0, sizeof(kbd_keys));
		kbd_keys[KEY_ESC].makecode = 0x0001;
		kbd_keys[KEY_ARR_UP].makecode = 0xE048;
		kbd_keys[KEY_ARR_DOWN].makecode = 0xE050;
		kbd_keys[KEY_ARR_LEFT].makecode = 0xE04B;
		kbd_keys[KEY_ARR_RIGHT].makecode = 0xE04D;
		kbd_keys[KEY_ENTER].makecode = 0x001C;
		kbd_keys[KEY_SPACE].makecode = 0x0039;
		return KEYBOARD_HOOK_BIT;
	}
	return -1;
}

int keyboard_toggle_leds(unsigned short leds)
{
	// Wait for ACK
	unsigned current_try = 0;
	int result;
	while (current_try < KBC_TRIES)
	{
		if (sys_outb(I8042_IN_BUF, I8042_SWITCH_KBD_LEDS) != OK)
		{
			return 1;
		}
		if (keyboard_wait_for_response(current_try))
		{
			return 1;
		}

		unsigned long output;
		if (sys_inb(I8042_OUT_BUF, &output) != OK)
		{
			return 1;
		}
		if (!(output == I8042_KBD_RESPONSE_ACK))
		{
			++current_try;
			continue;
		}

		result = keyboard_send_argument(leds, current_try);

		if (result == -1)
		{
			continue;
		}
		else if (result == OK)
		{
			led_status = leds;
			return 0;
		}
		else
		{
			return 1;
		}

		++current_try;
	}
	return 1;
}

static int keyboard_send_argument(unsigned long argument, unsigned current_try)
{
	unsigned long output;
	while (current_try < KBC_TRIES)
	{
		if (sys_outb(I8042_IN_BUF, argument))
		{
			return 1;
		}
		if (keyboard_wait_for_response(current_try))
		{
			return 1;
		}
		if (sys_inb(I8042_OUT_BUF, &output) != OK)
		{
			return 1;
		}
		if (output == I8042_KBD_RESPONSE_ACK)
		{
			return 0;
		}
		else if (output == I8042_KBD_RESPONSE_RESEND)
		{
			++current_try;
			continue;
		}
		return -1;	// Error, repeat sequence
	}
	return 1;
}

int keyboard_read_status(unsigned long* status)
{
	return sys_inb(I8042_STAT_REG, status);
}

int keyboard_int_handler()
{
	static unsigned long scancode = KEY_NONE;
	unsigned long current_scancode;
	if (sys_inb(I8042_OUT_BUF, &current_scancode))
	{
		return 1;
	}
	if (current_scancode == TWO_BYTE_SCANCODE)
	{
		scancode = TWO_BYTE_SCANCODE << 8;
		return 0;
	}
	if (IS_BREAK_CODE(scancode | current_scancode))
	{
		keyboard_update_key((scancode | current_scancode) & ~(1 << I8042_BREAK_CODE_BIT), false);
	}
	else
	{
		keyboard_update_key(scancode | current_scancode, true);
	}
	scancode = KEY_NONE;
	return 0;
}

static int keyboard_update_key(unsigned long makecode, bool pressed)
{
	size_t i;
	for (i = 0; i < sizeof(kbd_keys) / sizeof(kbd_key_t); ++i)
	{
		// KEY_NULL is also considered here, but that's not a problem
		if (kbd_keys[i].makecode == makecode)
		{
			kbd_keys[i].pressed = pressed;
			return 0;
		}
	}
	return 1;
}

int keyboard_unsubscribe_int()
{
	if (sys_irqrmpolicy(&irq_hook_id) == OK)
	{
		return 0;
	}
	return 1;
}

unsigned char keyboard_get_led_status()
{
	return led_status;
}
