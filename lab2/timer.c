#include "timer.h"

int timer_set_square(unsigned long timer, unsigned long freq) {
	if (freq == 0)
	{
		return 1; // The frequency can't be set to 0.
	}
	if (freq > TIMER_FREQ)
	{
		return 1; // The number to put in the counter can't be lower than 1. TIMER_FREQ + 1 doesn't cause problems because the result is truncated, but if the user/programmer uses that value he's probably doing something wrong. Also, setting the counter to '0' makes it
	}
	freq = TIMER_FREQ / freq;
	if (freq >= (1 << 16))
	{
		return 1; // The counter only accepts numbers with a maximum of 16 bits.
	}
	unsigned long* value;
	value = &freq;
	unsigned char timer_bit, timer_port;
	switch (timer)
	{
	case 0:
		timer_bit = TIMER_SEL0;
		timer_port = TIMER_0;
		break;
	case 1:
		timer_bit = TIMER_SEL1;
		timer_port = TIMER_1;
		break;
	case 2:
		timer_bit = TIMER_SEL2;
		timer_port = TIMER_2;
		break;
	default:
		return 1;
	}
	if (sys_outb(TIMER_CTRL, timer_bit | TIMER_SQR_WAVE | TIMER_LSB_MSB) == OK)
	{
		if (sys_outb(timer_port, *value & 11111111) == OK)
		{
			return sys_outb(timer_port, *value >> 8);
		}
	}
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
	if (sys_outb(TIMER_CTRL, TIMER_RB_SEL(timer) | TIMER_RB_CMD | TIMER_RB_COUNT_))
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

int timer_enable_speaker()
{
	unsigned long* speaker;
	printf("penis\n");
	if (speaker = malloc(sizeof(unsigned long)))
	{
		if (sys_inb(0x61, speaker) == OK)
		{
			if (sys_outb(0x61, *speaker | (long unsigned int)3) == OK)
			{
				printf("\n");
				printf("\n");
				print_binary((char)*speaker);
				printf("\n");
				sys_inb(0x61, speaker);
				print_binary((char)*speaker);
				printf("\n");
				printf("\n");
			}
		}
	}
	return 1;
}

int timer_display_conf(unsigned char conf) {
	printf("Status byte: ");
	print_binary(conf);
	printf("\n");
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

int timer_test_square(unsigned long timer, unsigned long freq) {
	timer_set_square(timer, freq);
	return 1;
}

int timer_test_int(unsigned long time) {

	return 1;
}

int timer_test_config(unsigned long timer) {
	unsigned long *st;
	if (st = malloc(sizeof(unsigned char)))
	{
		if (timer_get_conf(timer, st))
		{
			free(st);
			return 1;
		}
		timer_enable_speaker();
		return timer_display_conf((unsigned char)*st);
	}
	free(st);
	return 1;
}

void print_binary(unsigned char number)
{
	size_t i;
	for (i = 0; i < 8; ++i)
	{
		if (number & 10000000)
		{
			printf("1");
		}
		else
		{
			printf("0");
		}
		number = number << 1;
	}
	return;
}
