#include "timer.h"

static unsigned long time_counter = 0;
static int irq_hook_id;
static int doing_task = false;

static void print_binary(unsigned char number)
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

static unsigned long binary_to_BCD(unsigned long binary)
{
	unsigned long invertedBCD = 0;
	unsigned long BCD = 0;
	while (binary != 0)
	{
		invertedBCD <<= 4;
		invertedBCD |= binary % 10;
		binary /= 10;
	}
	while(invertedBCD != 0)
	{
		BCD <<= 4;
		BCD |= invertedBCD % (1 << 4);
		invertedBCD /= (1 << 4);
	}
	return BCD;
}

static int get_timer_BCD(unsigned long timer)
{
	if (sys_outb(TIMER_CTRL, TIMER_RB_SEL(timer) | TIMER_RB_CMD | TIMER_RB_COUNT_))
	{
		return -1;
	}
	unsigned long st;
	switch(timer)
	{
	case 0:
		if (sys_inb(TIMER_0, &st)) return -1;
		break;
	case 1:
		if (sys_inb(TIMER_1, &st)) return -1;
		break;
	case 2:
		if (sys_inb(TIMER_2, &st)) return -1;
		break;
	default:
		return -1;
	}
	return (st & TIMER_BCD) == TIMER_BCD;
}

int timer_set_square(unsigned long timer, unsigned long freq) {
	if (freq == 0)
	{
		return 1; // The frequency can't be set to 0.
	}
	if (freq > TIMER_FREQ)
	{
		return 1; // The number to put in the counter can't be lower than 1. TIMER_FREQ + 1 doesn't cause problems because the result is truncated, but if the user/programmer uses that value he's probably doing something wrong.
	}
	freq = TIMER_FREQ / freq;
	int BCD;
	if ((BCD = get_timer_BCD(timer)) == -1)
	{
		return 1; // Error obtaining the least significant bit from the status byte.
	}
	if (((BCD == TIMER_BCD) && freq > MAX_BCD_IN_WORD) || freq > MAX_BINARY_IN_WORD)
	{
		return 1; // The counter only accepts numbers with a maximum of 16 bits.
	}
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
	switch (get_timer_BCD(timer))
	{
	case TIMER_BIN:
		if (sys_outb(TIMER_CTRL, timer_bit | TIMER_SQR_WAVE | TIMER_LSB_MSB | TIMER_BIN) == OK)
		{
			if (sys_outb(timer_port, WORD_LSB((unsigned)freq)) == OK)
			{
				return sys_outb(timer_port, WORD_MSB((unsigned)(freq)));
			}
		}
		return 1;
	case TIMER_BCD:
		if (sys_outb(TIMER_CTRL, timer_bit | TIMER_SQR_WAVE | TIMER_LSB_MSB | TIMER_BCD) == OK)
		{
			if (sys_outb(timer_port, WORD_LSB(binary_to_BCD((unsigned)freq))) == OK)
			{
				return sys_outb(timer_port, WORD_MSB(binary_to_BCD((unsigned)(freq))));
			}
		}
		return 1;
	default:	// -1
		return 1;
	}
}

int timer_subscribe_int(void ) {
	irq_hook_id = TIMER0_HOOK_BIT;
	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &irq_hook_id) == OK)
	{
		if (sys_irqenable(&irq_hook_id) == OK)
		{
			return TIMER0_HOOK_BIT;
		}
	}
	return 1;
}

int timer_unsubscribe_int() {
	if (sys_irqrmpolicy(&irq_hook_id) == OK)
	{
		if (sys_irqdisable(&irq_hook_id) == OK)
			return 0;
	}
	return 1;
}

void timer_int_handler() {
	time_counter++; // If the counter overflows, it will start from 0 again.
	return;
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
	unsigned long speaker;
	if (sys_inb(SPEAKER_CTRL, &speaker) == OK)
	{
		if (sys_outb(SPEAKER_CTRL, speaker | (long unsigned int)3) == OK)
		{
			return 0;
		}
	}
	return 1;
}

int timer_disable_speaker()
{
	unsigned long speaker;
	if (sys_inb(SPEAKER_CTRL, &speaker) == OK)
	{
		if (sys_outb(SPEAKER_CTRL, speaker & (char)(11111 << 3)) == OK) ////
		{
			return 0;
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
	if ((conf & TIMER_BCD) == TIMER_BCD)
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
	return 0;
}

int timer_test_int(unsigned long time) {
	if (timer_subscribe_int())
	{
		return 1;
	}
	int r, ipc_status;
	message msg;
	time_counter = 0;
	while (time_counter <= time * TIMER_DEFAULT_FREQ)
	{
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & BIT(TIMER0_HOOK_BIT)) { /////// /* subscribed interrupt */
					timer_int_handler();
					if(time_counter % TIMER_DEFAULT_FREQ == 0)
					{
						printf("Segundos decorridos: %d\n", time_counter / TIMER_DEFAULT_FREQ);
					}
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}
	return timer_unsubscribe_int();
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
		return timer_display_conf((unsigned char)*st);
	}
	free(st);
	return 1;
}

int set_repetitive_task(unsigned long freq, void(*func)())
{
	if (doing_task) // Current task must be stopped first
	{
		return 1;
	}
	if (timer_subscribe_int())
	{
		return 1;
	}
	if (timer_set_square(0, freq))
	{
		return 1;
	}
	int r, ipc_status;
	message msg;
	time_counter = 0;
	doing_task = true;
	while (doing_task)
	{
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & BIT(TIMER0_HOOK_BIT)) { /* subscribed interrupt */
					timer_int_handler();
					func();
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}
	return 0;
}

int stop_repetitive_task()
{
	if (doing_task)
	{
		doing_task = false;
		return timer_unsubscribe_int();
	}
	return 1; // Can't stop a non-started task
}
