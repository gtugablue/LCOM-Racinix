#include "kbc.h"

int kbc_subscribe_keyboard_int(unsigned char* hook_id)
{
	unsigned char hook_bit = *hook_id;
	if (sys_irqsetpolicy(I8042_KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, hook_i) == OK)
	{
		return hook_bit;
	}
	return -1;
}

int kbc_subscribe_mouse_int(unsigned char* hook_id);

int kbc_send_command(unsigned char command);

int kbc_read_response(unsigned char* response);

int kbc_unsubscribe_keyboard_int(unsigned char hook_id)
{
	if (sys_irqrmpolicy(hook_id) == OK)
	{
		return 0;
	}
	return 1;
}

int kbc_unsubscribe_mouse_int(unsigned char hook_id);
