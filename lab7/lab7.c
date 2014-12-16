#include "serial.h"
#include "minix/driver.h"

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

	serial_fifo_receive_string(1, &string);
	printf("String recebida: %s\n", string);
	free(string);

	serial_unsubscribe_int(hook_id, 1);

	return 0;
}
