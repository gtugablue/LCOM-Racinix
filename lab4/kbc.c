#include "kbc.h"

#define BIT(n) (0x01<<(n))

static int kbd_wait_for_in_buf(unsigned num_tries);
static int kbd_wait_for_out_buf(unsigned num_tries);

int kbc_read_status(unsigned long* status)
{
	return sys_inb(I8042_STAT_REG, status);
}

int kbc_subscribe_keyboard_int(unsigned* hook_id)
{
	unsigned char hook_bit = (unsigned char)*hook_id;
	if (hook_bit != *hook_id)
	{
		 return -1;
	}
	if (sys_irqsetpolicy(I8042_KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, hook_id) == OK)
	{
		return -1;
	}
	return hook_bit;
}

static int kbd_wait_for_in_buf(unsigned num_tries)
{
	unsigned long status;
	size_t i;
	for (i = 0; i < num_tries; ++i)
	{
		if (kbc_read_status(&status))
		{
			return 1;
		}
		if (!(status & BIT(I8042_STATUS_IBF_BIT)))
		{
			return 0;
		}
		tickdelay(micros_to_ticks(I8042_KBD_TIMEOUT));
	}
	return 1;
}

static int kbd_wait_for_out_buf(unsigned num_tries)
{
	unsigned long status;
	size_t i;
	for (i = 0; i < num_tries; ++i)
	{
		if (kbc_read_status(&status))
		{
			return -1; // Error
		}
		if (status & (BIT(I8042_STATUS_TIMEOUT_BIT) | BIT(I8042_STATUS_PARITY_BIT)))
		{
			return 1; // Try again
		}
		if (status & BIT(I8042_STATUS_OBF_BIT))
		{
			return 0;
		}
		tickdelay(micros_to_ticks(I8042_KBD_TIMEOUT));
	}
	return -1; // Error
}

int kbc_write(unsigned num_tries, unsigned char command)
{
	if (sys_outb(I8042_CTRL_REG, I8042_WRITE_COMMAND_BYTE) != OK)
	{
		return 1;
	}
	if (kbd_wait_for_in_buf(num_tries) != 0)
	{
		return 1;
	}
	if (sys_outb(I8042_CTRL_REG, command) != OK)
	{
		return 1;
	}
	return 0;
}

int kbc_send_data(unsigned num_tries, unsigned char argument)
{
	if (kbd_wait_for_in_buf(num_tries))
	{
		return 1;
	}
	if (sys_outb(I8042_IN_BUF, argument) != OK)
	{
		return 1;
	}
	return 0;
}

int kbc_write_to_mouse(unsigned num_tries)
{
	if (kbd_wait_for_in_buf(num_tries))
	{
		return 1;
	}
	if(sys_outb(I8042_CTRL_REG, I8042_WRITE_BYTE_TO_MOUSE) == OK)
	{
		return 0;
	}
	return 1;
}

int kbc_read(unsigned num_tries, unsigned long* output)
{
	size_t i;
	for (i = 0; i < num_tries; ++i)
	{
		int result = kbd_wait_for_out_buf(num_tries);
		if (result == -1)
		{
			return 1;
		}
		else if (result == 0)
		{
			break;
		}
	}
	if (sys_inb(I8042_OUT_BUF, output) != OK)
	{
		return 1;
	}

	return 0;
}

int kbc_clean_output_buffer(unsigned num_tries)
{
	unsigned long status;
	size_t i;
	for (i = 0; i < num_tries; ++i)
	{
		if (kbc_read_status(&status))
		{
			return 1;
		}
		if (status & I8042_STATUS_OBF_BIT)
		{
			if (sys_inb(I8042_OUT_BUF, &status))
			{
				return 1;
			}
			continue;
		}
		return 0;
	}
}

int kbc_unsubscribe_int(unsigned hook_id)
{
	if (sys_irqrmpolicy(&hook_id) == OK)
	{
		return 0;
	}
	return 1;
}
