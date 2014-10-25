#include "kbd.h"

#define KBC_TRIES			10

static int kbc_wait_for_input_buffer(unsigned long *status);

static int irq_hook_id;
static unsigned char led_status = 0;

int kbd_is_response(unsigned long interruption)
{
	if(interruption == I8042_ACK || interruption == I8042_RESEND || interruption == I8042_ERROR)
	{
		return 1;
	}
	return 0;
}

static int kbd_wait_for_response(unsigned current_try)
{
	unsigned long status;
	while (current_try < KBC_TRIES)
	{
		tickdelay(micros_to_ticks(I8042_KBD_TIMEOUT));
		if (kbd_read_status(&status))
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

int kbd_subscribe_int()
{
	irq_hook_id = KBD_HOOK_BIT;
	if (sys_irqsetpolicy(I8042_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &irq_hook_id) == OK)
	{
		return KBD_HOOK_BIT;
	}
	return -1;
}

int kbd_toggle_leds(unsigned short leds)
{
	// Wait for ACK
	unsigned current_try = 0;
	while (current_try < KBC_TRIES)
	{
		if (sys_outb(I8042_IN_KBD_CMD_BUF, I8042_SWITCH_KBD_LEDS) != OK)
		{
			return 1;
		}
		if (kbd_wait_for_response(current_try))
		{
			return 1;
		}

		unsigned long output;
		if (sys_inb(I8042_OUT_BUF, &output) != OK)
		{
			return 1;
		}
		if (!(output == I8042_ACK))
		{
			printf("%d\n", output);
			++current_try;
			continue;
		}

		// Command sent successfully, send argument now
		if (sys_outb(I8042_IN_ARG_BUF, leds))
		{
			return 1;
		}
		if (kbd_wait_for_response(current_try))
		{
			return 1;
		}
		if (sys_inb(I8042_OUT_BUF, &output) != OK)
		{
			return 1;
		}
		if (output == I8042_ACK)
		{
			led_status = leds;
			return 0;
		}
		else if (output == I8042_RESEND)
		{
			if (sys_outb(I8042_IN_ARG_BUF, leds))
			{
				return 1;
			}
			if (kbd_wait_for_response(current_try))
			{
				return 1;
			}
			if (sys_inb(I8042_OUT_BUF, &output) != OK)
			{
				return 1;
			}
			if (output == I8042_ACK)
			{
				led_status = leds;
				return 0;
			}
		}
		++current_try;
	}
	return 1;
}

int kbd_read_status(unsigned long* status)
{
	return sys_inb(I8042_STAT_REG, status);
}

int kbd_read_output(unsigned short ass, unsigned long* output)
{
	unsigned long* output;
	if ((output == malloc(sizeof(unsigned long))) == NULL)
	{
		return 1;
	}
	if (sys_inb(I8042_OUT_REG, output) == OK)
	{
		return 0;
	}
	return 1;
}

int kbd_unsubscribe_int()
{
	if (sys_irqrmpolicy(&irq_hook_id) == OK)
	{
		return 0;
	}
	return 1;
}

unsigned char kbd_get_led_status()
{
	return led_status;
}
