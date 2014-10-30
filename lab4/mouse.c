#include "mouse.h"

static unsigned char packet[MOUSE_PACKET_SIZE];
static unsigned char next_byte;

static bool mouse_check_synchronization();

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
		printf("Finished subscription.\n");
		return hook_bit;
	}
	return -1;
}

bool mouse_get_packet(unsigned char return_packet[])
{
	if (next_byte == 0 && mouse_check_synchronization())
	{
		memcpy(return_packet, packet, sizeof(packet));
		return true;
	}
	return false;
}

int mouse_write(unsigned num_tries, unsigned char command)
{
	unsigned long response;
	size_t i;
	for (i = 0; i < num_tries; ++i)
	{
		if (kbc_write_to_mouse())
		{
			return 1;
		}
		if (kbc_send_data(num_tries, command))
		{
			return 1;
		}
		if (kbc_read(num_tries, &response))
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

int mouse_send_argument(unsigned num_tries, unsigned char argument)
{
	unsigned long response;
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
	if (response == MOUSE_RESPONSE_ACK)
	{
		return 0;
	}
	return -1;
}

int mouse_int_handler(unsigned num_tries)
{
	unsigned long output;
	if (kbc_read(num_tries, &output))
	{
		printf("Error reading packet byte.\n");
		return 1;
	}
	printf("BYTE READ: 0x%X\n", output);
	packet[next_byte++] = output;
	return 0;
}

int mouse_set_stream_mode(unsigned num_tries)
{
	if(mouse_write(num_tries, MOUSE_SET_STREAM_MODE))
	{
		return 1;
	}
	printf("Stream mode successfully set.\n");
	return 0;
}

int mouse_enable_stream_mode(unsigned num_tries)
{
	if(mouse_write(num_tries, MOUSE_ENABLE_DATA_PACKETS))
	{
		return 1;
	}
	memset(packet, 0, sizeof(packet));	// Clean the array to make sure bit 3 is off in all bytes
	next_byte = 0;
	return 0;
}

int mouse_disable_stream_mode(unsigned num_tries)
{
	if(mouse_write(num_tries, MOUSE_DISABLE_STREAM_MODE))
	{
		return 1;
	}
	memset(packet, 0, sizeof(packet));	// Clean the array to make sure bit 3 is off in all bytes
	next_byte = 0;
}

int mouse_unsubscribe_int(unsigned hook_id)
{
	return kbc_unsubscribe_int(hook_id);
}

static bool mouse_check_synchronization()
{
	size_t i, j;
	if (MOUSE_IS_POSSIBLE_FIRST_BYTE(packet[0]))	// Most of the times this will be true, so it's a good idea to check it first to avoid entering the loop
	{
		return true;
	}
	for (i = 1; i < MOUSE_PACKET_SIZE; ++i)
	{
		if (MOUSE_IS_POSSIBLE_FIRST_BYTE(packet[i]))
		{
			for (j = 0; j < MOUSE_PACKET_SIZE; ++j)
			{
				printf("derp");
				packet[j] = packet[(i + j) % MOUSE_PACKET_SIZE];
				next_byte -= i;
				return true;
			}
		}
	}
	return false;	// Couldn't find a byte with bit 3 set
}
