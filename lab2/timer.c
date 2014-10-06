#include "timer.h"

int timer_set_square(unsigned long timer, unsigned long freq) {

	return 1;
}

int timer_subscribe_int(void ) {

	return 1;
}

int timer_unsubscribe_int() {

	return 1;
}

void timer_int_handler() {

}

int timer_get_conf(unsigned long timer, unsigned long *st) {
	if (sys_outb(TIMER_CTRL, TIMER_RB_SEL(timer) | TIMER_RB_CMD))
	{
		return 1;
	}
	switch(timer)
	{
	case 0:
		return sys_inb(TIMER_0, st);
	case 1:
		return sys_inb(TIMER_1, st);
	case 2:
		return sys_inb(TIMER_2, st);
	default:
		return 1;
	}
}

int timer_get_counter(unsigned long timer, unsigned char conf, unsigned char *counter)
{
	unsigned char timer_addr;
	switch (timer)
	{
	case 0:
		timer_addr = TIMER_SEL0;
		break;
	case 1:
		timer_addr = TIMER_SEL1;
		break;
	case 2:
		timer_addr = TIMER_SEL2;
		break;
	}
	if (sys_outb(TIMER_CTRL, (conf & (TIMER_OP_MODE | TIMER_BCD)) | TIMER_LSB | TIMER_RB_SEL(timer) | timer_addr))
	{
		return 1;
	}
	switch (timer)
	{
	case 0:
		timer = (unsigned long)TIMER_0;
		break;
	case 1:
		timer = (unsigned long)TIMER_1;
		break;
	case 2:
		timer = (unsigned long)TIMER_2;
		break;
	default:
		return 1;
	}
	return *counter = sys_outb((unsigned char)timer, counter);
}

int timer_display_conf(unsigned char conf, unsigned char counter) {
	printf("Counter: %lu\n", counter);
	printf("Status byte: %d\n", (int)conf);
	printf("Output: %d\n", (conf & TIMER_STATUS_OUTPUT) >> TIMER_STATUS_OUTPUT_BIT);
	printf("Null count: %d\n", (conf & TIMER_STATUS_NULL) >> TIMER_STATUS_NULL_BIT);
	printf("Type of access: ");
	switch (conf & TIMER_LSB_MSB)
	{
	case TIMER_LSB:
		printf("LSB\n");
		break;
	case TIMER_MSB:
		printf("MSB\n");
		break;
	case TIMER_LSB_MSB:
		printf("LSB followed by MSB\n");
		break;
	default:
		printf("Unknown\n");
	}
	printf("Operating mode: ");
	switch (conf & (TIMER_RATE_GEN | TIMER_SQR_WAVE))
	{
	case TIMER_RATE_GEN:
		printf("rate generator\n");
		break;
	case TIMER_SQR_WAVE:
		printf("square wave generator\n");
		break;
	default:
		printf("Unknown\n");
	}
	printf("Counting mode: ");
	if ((conf & (TIMER_BCD | TIMER_BIN)) == TIMER_BCD)
	{
		printf("BCD\n");
	}
	else
	{
		printf("binary\n");
	}

	return 0;
}

int timer_test_square(unsigned long freq) {
	
	return 1;
}

int timer_test_int(unsigned long time) {

	return 1;
}

int timer_test_config(unsigned long timer) {
	unsigned long *st;
	unsigned char *counter;
	if ((st = malloc(sizeof(unsigned char))) && (counter = malloc(sizeof(unsigned long))))
	{
		if (timer_get_conf(timer, st) || timer_get_counter(timer, (unsigned char)*st, counter))
		{
			return 1;
		}
		return timer_display_conf((unsigned char)*st, *counter);
	}
	return 1;
}
