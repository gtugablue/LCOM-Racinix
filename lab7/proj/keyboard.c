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
		kbd_keys[KEY_NONE].makecode = 0x0000;
		kbd_keys[KEY_ESC].makecode = 0x0001;
		kbd_keys[KEY_1].makecode = 0x0002;
		kbd_keys[KEY_2].makecode = 0x0003;
		kbd_keys[KEY_3].makecode = 0x0004;
		kbd_keys[KEY_4].makecode = 0x0005;
		kbd_keys[KEY_5].makecode = 0x0006;
		kbd_keys[KEY_6].makecode = 0x0007;
		kbd_keys[KEY_7].makecode = 0x0008;
		kbd_keys[KEY_8].makecode = 0x0009;
		kbd_keys[KEY_9].makecode= 0x000A;
		kbd_keys[KEY_0].makecode = 0x000B;
		kbd_keys[KEY_APOSTROPHE].makecode = 0x000C;
		kbd_keys[KEY_ANGLE_QUOTES].makecode = 0x000D;
		kbd_keys[KEY_BKSP].makecode = 0x000E;
		kbd_keys[KEY_TAB].makecode = 0x000F;
		kbd_keys[KEY_Q].makecode = 0x0010;
		kbd_keys[KEY_W].makecode = 0x0011;
		kbd_keys[KEY_E].makecode = 0x0012;
		kbd_keys[KEY_R].makecode = 0x0013;
		kbd_keys[KEY_T].makecode = 0x0014;
		kbd_keys[KEY_Y].makecode = 0x0015;
		kbd_keys[KEY_U].makecode = 0x0016;
		kbd_keys[KEY_I].makecode = 0x0017;
		kbd_keys[KEY_O].makecode = 0x0018;
		kbd_keys[KEY_P].makecode = 0x0019;
		kbd_keys[KEY_PLUS].makecode = 0x001A;
		kbd_keys[KEY_ACCENT].makecode = 0x001B;
		kbd_keys[KEY_ENTER].makecode = 0x001C;
		kbd_keys[KEY_L_CTRL].makecode = 0x001D;
		kbd_keys[KEY_A].makecode = 0x001E;
		kbd_keys[KEY_S].makecode = 0x001F;
		kbd_keys[KEY_D].makecode = 0x0020;
		kbd_keys[KEY_F].makecode = 0x0021;
		kbd_keys[KEY_G].makecode = 0x0022;
		kbd_keys[KEY_H].makecode = 0x0023;
		kbd_keys[KEY_J].makecode = 0x0024;
		kbd_keys[KEY_K].makecode = 0x0025;
		kbd_keys[KEY_L].makecode = 0x0026;
		kbd_keys[KEY_C_CEDILLA].makecode = 0x0027;
		kbd_keys[KEY_ORDINAL].makecode = 0x0028;
		kbd_keys[KEY_BACKSLASH].makecode = 0x0029;
		kbd_keys[KEY_L_SHIFT].makecode = 0x002A;
		kbd_keys[KEY_TILDE].makecode = 0x002B;
		kbd_keys[KEY_Z].makecode = 0x002C;
		kbd_keys[KEY_X].makecode = 0x002D;
		kbd_keys[KEY_C].makecode = 0x002E;
		kbd_keys[KEY_V].makecode = 0x002F;
		kbd_keys[KEY_B].makecode = 0x0030;
		kbd_keys[KEY_N].makecode = 0x0031;
		kbd_keys[KEY_M].makecode = 0x0032;
		kbd_keys[KEY_COMMA].makecode = 0x0033;
		kbd_keys[KEY_DOT].makecode = 0x0034;
		kbd_keys[KEY_MINUS].makecode = 0x0035;
		kbd_keys[KEY_R_SHIFT].makecode = 0x0036;
		kbd_keys[KEY_ALT].makecode = 0x0038;
		kbd_keys[KEY_SPACE].makecode = 0x0039;
		kbd_keys[KEY_CAPS].makecode = 0x003A;
		kbd_keys[KEY_F1].makecode = 0x003B;
		kbd_keys[KEY_F2].makecode = 0x003C;
		kbd_keys[KEY_F3].makecode = 0x003D;
		kbd_keys[KEY_F4].makecode = 0x003E;
		kbd_keys[KEY_F5].makecode = 0x003F;
		kbd_keys[KEY_F6].makecode = 0x0040;
		kbd_keys[KEY_F7].makecode = 0x0041;
		kbd_keys[KEY_F8].makecode = 0x0042;
		kbd_keys[KEY_F9].makecode = 0x0043;
		kbd_keys[KEY_F10].makecode= 0x0044;
		kbd_keys[KEY_NUM].makecode = 0x0045;
		kbd_keys[KEY_SCRLL].makecode = 0x0046;
		kbd_keys[KEY_NUM_7].makecode = 0x0047;
		kbd_keys[KEY_NUM_8].makecode = 0x0048;
		kbd_keys[KEY_NUM_9].makecode = 0x0049;
		kbd_keys[KEY_NUM_MINUS].makecode = 0x004A;
		kbd_keys[KEY_NUM_4].makecode = 0x004B;
		kbd_keys[KEY_NUM_5].makecode = 0x004C;
		kbd_keys[KEY_NUM_6].makecode = 0x004D;
		kbd_keys[KEY_NUM_PLUS].makecode = 0x004E;
		kbd_keys[KEY_NUM_1].makecode = 0x004F;
		kbd_keys[KEY_NUM_2].makecode = 0x0050;
		kbd_keys[KEY_NUM_3].makecode = 0x0051;
		kbd_keys[KEY_NUM_0].makecode = 0x0052;
		kbd_keys[KEY_NUM_DEL].makecode = 0x0053;
		kbd_keys[KEY_MINOR].makecode = 0x0056;
		kbd_keys[KEY_F11].makecode = 0x0057;
		kbd_keys[KEY_F12].makecode= 0x0058;
		kbd_keys[KEY_NUM_ENTER].makecode = 0xE01C;
		kbd_keys[KEY_R_CTRL].makecode = 0xE01D;
		kbd_keys[KEY_NUM_SLASH].makecode = 0xE03;
		kbd_keys[KEY_ALT_GR].makecode = 0xE038;
		kbd_keys[KEY_HOME].makecode = 0xE047;
		kbd_keys[KEY_ARR_UP].makecode = 0xE048;
		kbd_keys[KEY_PGUP].makecode = 0xE049;
		kbd_keys[KEY_ARR_LEFT].makecode = 0xE04B;
		kbd_keys[KEY_ARR_RIGHT].makecode = 0xE04D;
		kbd_keys[KEY_ARR_DOWN].makecode = 0xE050;
		kbd_keys[KEY_PGDN].makecode = 0xE051;
		kbd_keys[KEY_INS].makecode = 0xE052;
	    kbd_keys[KEY_DEL].makecode = 0xE053;
		kbd_keys[KEY_WIN].makecode = 0xE05B;
		kbd_keys[KEY_CNTX].makecode = 0xE05D;
		kbd_keys[KEY_END].makecode = 0xE04F;
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
		return -1;
	}
	if (current_scancode == TWO_BYTE_SCANCODE)
	{
		scancode = TWO_BYTE_SCANCODE << 8;
		return KEY_NONE;
	}
	current_scancode |= scancode;
	scancode = KEY_NONE;
	if (IS_BREAK_CODE(current_scancode))
	{
		return keyboard_update_key((current_scancode) & ~(1 << I8042_BREAK_CODE_BIT), false);
	}
	else
	{
		return keyboard_update_key(current_scancode, true);
	}
	return KEY_NONE;
}

static int keyboard_update_key(unsigned long makecode, bool pressed)
{
	size_t i;
	for (i = 0; i < sizeof(kbd_keys) / sizeof(kbd_key_t); ++i)
	{
		// KEY_NONE is also considered here, but that's not a problem
		if (kbd_keys[i].makecode == makecode)
		{
			kbd_keys[i].pressed = pressed;
			return i;
		}
	}
	return KEY_NONE;
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
