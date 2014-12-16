#include "serial.h"
#include "minix/driver.h"

int main(int argc, char **argv) {

	sef_startup();
	unsigned hook_id = 5;
	unsigned serial_hook_bit = serial_subscribe_int(&hook_id, 1, 2);

	//unsigned char *string;

	/*while (1)
	{
		if (serial_receive_string(1, &string) == 0)
		{
			printf("String recebida: %s\n", string);
			free(string);
			break;
		}
	}*/

	/*serial_interrupt_receive_string(1, &string);
	printf("Queue size: %d, String recebida: %s\n", serial_get_num_queued_strings(1), string);
	free(string);
	serial_set(1, 8, 1, 0, 9600);

	while (serial_get_num_queued_strings(1) > 0)
	{
		if (serial_interrupt_receive_string(1, &string) == 0)
		{
			printf("Queue size: %d, String recebida: %s\n", serial_get_num_queued_strings(1), string);
			free(string);
		}
	}*/

	/*char string[] = "teste";
	serial_interrupt_transmit_string(1, string);*/

	int r, ipc_status;
	message msg;
	unsigned int_counter = 0;
	while(int_counter < 5)
	{
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			// Driver receive fail
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			if (_ENDPOINT_P(msg.m_source) == HARDWARE) /* hardware interrupt notification */
			{
				if (msg.NOTIFY_ARG & (1 << serial_hook_bit))
				{
					++int_counter;
					if (serial_int_handler(1))
					{
						printf("Error.\n");
					}
				}
			}
		}
	}

	printf("Unsubscribing...\n");
	serial_unsubscribe_int(hook_id, 1);

	return 0;
}
