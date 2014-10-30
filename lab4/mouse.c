#include "mouse.h"
#include "queue.h"

static queue_t byte_queue;

int mouse_subscribe_int(unsigned* hook_id)
{
	unsigned char hook_bit = (unsigned char)*hook_id;
	if (hook_bit != *hook_id)
	{
		return -1;
	}
	if (sys_irqsetpolicy(I8042_MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, hook_id) == OK)
	{
		return hook_bit;
	}
	return -1;
}

int mouse_write(unsigned num_tries, unsigned char command)
{
	unsigned char response;
	size_t i;
	for (i = 0; i < num_tries; ++i)
	{
		if (kbc_write_to_mouse())
		{
			return 1;
		}
		if (kbc_send_data(num_tries, I8042_WRITE_BYTE_TO_MOUSE))
		{
			return 1;
		}
		if (kbc_read(num_tries, &response))
		{
			return 1;
		}
		if (response == I8042_MOUSE_RESPONSE_ACK)
		{
			return 0;
		}
	}
	return 1;
}

int mouse_send_argument(unsigned num_tries, unsigned char argument)
{
	unsigned char response;
	size_t i;
	for (i = 0; i < num_tries; ++i)
	{
		if (kbc_write_to_mouse())
		{
			return 1;
		}
		if (kbc_send_data(num_tries, argument))
		{
			return 1;
		}
		if (kbc_read(num_tries, &response))
		{
			return 1;
		}
		if (response == I8042_MOUSE_RESPONSE_ACK)
		{
			return 0;
		}
	}
	return 1;
}

int mouse_unsubscribe_int(unsigned hook_id)
{
	return kbc_unsubscribe_int(hook_id);
}
