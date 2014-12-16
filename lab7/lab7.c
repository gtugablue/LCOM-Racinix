#include "serial.h"
#include "minix/driver.h"
#include "queue.h"

int main(int argc, char **argv) {

	sef_startup();
	/*unsigned hook_id = 5;
	serial_subscribe_int(&hook_id, 1);

	unsigned char *string;*/
	/*while (1)
	{
		if (serial_receive_string(1, &string) == 0)
		{
			printf("String recebida: %s\n", string);
			free(string);
			break;
		}
	}*/

	/*serial_set(1, 8, 1, 0, 9600);

	if (serial_fifo_receive_string(1, &string) == 0)
	{
		printf("Retornou 0.\n");
		printf("String recebida: %s\n", string);
		free(string);
	}
	printf("Unsubscribing...\n");
	serial_unsubscribe_int(hook_id, 1);*/

	queue_t *queue = queue_create();
	size_t i;
	for (i = 0; i < 10; ++i)
	{
		unsigned char *c = malloc(1);
		*c = rand() % 255;
		printf("Pushing char #%d...\n", *c);
		queue_push(queue, c);
	}
queue_print(queue);
	for (i = 0; i < 10; ++i)
	{
		unsigned char *c = queue_pop(queue);
		printf("Popped char #%d.\n", *c);
		free(c);
	}

	return 0;
}
