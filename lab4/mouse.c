#include "mouse.h"

#define BIT(n) (0x01<<(n))
#define MOUSE_IS_POSSIBLE_FIRST_BYTE(byte)	((byte) & BIT(MOUSE_1ST_BYTE_ALWAYS_1_BIT))
#define IS_BIT_SET(n, bit)	(((n) & BIT(bit)) ? 1 : 0)

static unsigned char packet[MOUSE_PACKET_SIZE];
static unsigned char next_byte;

static bool mouse_synchronize();
static int mouse_read();

int mouse_subscribe_int(unsigned* hook_id)
{
	unsigned char hook_bit = (unsigned char)*hook_id;
	if (hook_bit != *hook_id)
	{
		return -1;
	}
	if (sys_irqsetpolicy(I8042_MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, hook_id) == OK)
	{
		memset(packet, 0, sizeof(packet));	// Clean the array to make sure bit 3 is off in all bytes
		next_byte = 0;
		return hook_bit;
	}
	return -1;
}

bool mouse_get_packet(mouse_data_packet_t *mouse_data_packet)
{
	if (mouse_synchronize() && next_byte == 0)
	{
		mouse_data_packet->bytes[0] = packet[0];
		mouse_data_packet->bytes[1] = packet[1];
		mouse_data_packet->bytes[2] = packet[2];
		mouse_data_packet->x_overflow = IS_BIT_SET(packet[0], MOUSE_1ST_BYTE_X_OF_BIT);
		mouse_data_packet->y_overflow = IS_BIT_SET(packet[0], MOUSE_1ST_BYTE_Y_OF_BIT);
		mouse_data_packet->left_button = IS_BIT_SET(packet[0], MOUSE_1ST_BYTE_LEFT_BTN_BIT);
		mouse_data_packet->middle_button = IS_BIT_SET(packet[0], MOUSE_1ST_BYTE_MIDDLE_BTN_BIT);
		mouse_data_packet->right_button = IS_BIT_SET(packet[0], MOUSE_1ST_BYTE_RIGHT_BTN_BIT);
		mouse_data_packet->x_delta = MOUSE_DATA_PACKET_COUNTER((short)packet[1], IS_BIT_SET(packet[0], MOUSE_1ST_BYTE_X_SIGN_BIT));
		mouse_data_packet->y_delta = MOUSE_DATA_PACKET_COUNTER((short)packet[2], IS_BIT_SET(packet[0], MOUSE_1ST_BYTE_Y_SIGN_BIT));
		if (mouse_data_packet->x_overflow)
		{
			if (packet[0] & BIT(MOUSE_1ST_BYTE_X_SIGN_BIT))
			{
				mouse_data_packet->x_delta = -1 << 8;
			}
			else
			{
				mouse_data_packet->x_delta = 1 << 8;
			}
		}
		if (mouse_data_packet->y_overflow)
		{
			if (packet[0] & BIT(MOUSE_1ST_BYTE_Y_SIGN_BIT))
			{
				mouse_data_packet->y_delta = -1 << 8;
			}
			else
			{
				mouse_data_packet->y_delta = 1 << 8;
			}
		}
		return true;
	}
	return false;
}

int mouse_get_status(mouse_status_packet_t *mouse_status_packet)
{
	if (mouse_write(MOUSE_DISABLE_STREAM_MODE))
	{
		return 1;
	}
	if (mouse_write(MOUSE_STATUS_REQUEST))
	{
		return 1;
	}
	size_t i;
	for (i = 0; i < MOUSE_STATUS_SIZE; ++i)
	{
		if (mouse_read(&mouse_status_packet->bytes[i]))
		{
			return 1;
		}
	}
	mouse_status_packet->remote_mode = mouse_status_packet->bytes[0] & BIT(MOUSE_STATUS_REMOTE_BIT);
	mouse_status_packet->enabled = mouse_status_packet->bytes[0] & BIT(MOUSE_STATUS_ENABLED_BIT);
	mouse_status_packet->scaling_2_1 = mouse_status_packet->bytes[0] & BIT(MOUSE_STATUS_SCALING_2_1_BIT);
	mouse_status_packet->left_button = mouse_status_packet->bytes[0] & BIT(MOUSE_STATUS_LEFT_BTN_BIT);
	mouse_status_packet->middle_button = mouse_status_packet->bytes[0] & BIT(MOUSE_STATUS_MIDDLE_BTN_BIT);
	mouse_status_packet->right_button = mouse_status_packet->bytes[0] & BIT(MOUSE_STATUS_RIGHT_BTN_BIT);
	mouse_status_packet->resolution_byte = mouse_status_packet->bytes[1];
	mouse_status_packet->sample_rate = mouse_status_packet->bytes[2];
	return 0;
}

int mouse_write(unsigned char command)
{
	unsigned long response;
	size_t i;
	for (i = 0; i < MOUSE_NUM_TRIES; ++i)
	{
		if (kbc_write_to_mouse())
		{
			return 1;
		}
		if (kbc_send_data(command))
		{
			return 1;
		}
		if (kbc_read(&response))
		{
			return 1;
		}
		if (response == MOUSE_RESPONSE_ACK)
		{
			return 0;
		}
	}
	return 1;
}

int mouse_send_argument(unsigned char argument)
{
	unsigned long response;
	if (kbc_write_to_mouse())
	{
		return 1;
	}
	if (kbc_send_data(argument))
	{
		return 1;
	}
	if (kbc_read(&response))
	{
		return 1;
	}
	if (response == MOUSE_RESPONSE_ACK)
	{
		return 0;
	}
	return -1;	// NACK
}

int mouse_write_and_argument(unsigned char command, unsigned char argument)
{
	// TODO
}

int mouse_int_handler()
{
	unsigned long output;
	if (kbc_read(&output))
	{
		return 1;
	}
	packet[next_byte] = output;
	next_byte = (next_byte + 1) % MOUSE_PACKET_SIZE;
	return 0;
}

int mouse_set_stream_mode()
{
	if(mouse_write(MOUSE_SET_STREAM_MODE))
	{
		return 1;
	}
	return 0;
}

int mouse_enable_stream_mode()
{
	if(mouse_write(MOUSE_ENABLE_DATA_PACKETS))
	{
		return 1;
	}
	memset(packet, 0, sizeof(packet));	// Clean the array to make sure bit 3 is off in all bytes
	next_byte = 0;
	return 0;
}

int mouse_disable_stream_mode()
{
	if(mouse_write(MOUSE_DISABLE_STREAM_MODE))
	{
		return 1;
	}
	memset(packet, 0, sizeof(packet));	// Clean the array to make sure bit 3 is off in all bytes
	next_byte = 0;
}

int mouse_reset()
{
	return mouse_write(MOUSE_RESET);
}

int mouse_unsubscribe_int(unsigned hook_id)
{
	return kbc_unsubscribe_int(hook_id);
}

static bool mouse_synchronize()
{
	if (MOUSE_IS_POSSIBLE_FIRST_BYTE(packet[0]))	// Most of the times this will be true, so it's a good idea to check it first to avoid entering the loop
	{
		return true;
	}
	size_t i, j;
	for (i = 1; i < MOUSE_PACKET_SIZE; ++i)
	{
		if (MOUSE_IS_POSSIBLE_FIRST_BYTE(packet[i]))
		{
			for (j = 0; j < MOUSE_PACKET_SIZE; ++j)
			{
				packet[j] = packet[(i + j) % MOUSE_PACKET_SIZE];
				next_byte -= i;
				return true;
			}
		}
	}
	return false;	// Couldn't find a byte with bit 3 set
}

static int mouse_read(unsigned char *byte)
{
	// TODO
	unsigned long status;
	do
	{
		if (kbc_read_status(&status))
		{
			return 1;
		}
		if (kbc_read(byte))
		{
			return 1;
		}
	} while (!(status & BIT(I8042_STATUS_AUX_BIT)));
	return 0;
}
