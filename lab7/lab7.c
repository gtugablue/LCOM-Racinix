#include "serial.h"
#include "minix/driver.h"
#include "queue.h"

int main(int argc, char **argv) {

	sef_startup();
	unsigned hook_id = 5;
	serial_subscribe_int(&hook_id, 1);

	unsigned char *string;
	/*while (1)
	{
		if (serial_receive_string(1, &string) == 0)
		{
			printf("String recebida: %s\n", string);
			free(string);
			break;
		}
	}*/

	serial_set(1, 8, 1, 0, 9600);

	/*if (serial_fifo_receive_string(1, &string) == 0)
	{
		printf("Retornou 0.\n");
		printf("String recebida: %s\n", string);
		free(string);
	}*/

	printf("Numero de strings recebidas: %d\n", serial_get_num_queued_strings(1));

	printf("Unsubscribing...\n");
	serial_unsubscribe_int(hook_id, 1);

	return 0;
}
