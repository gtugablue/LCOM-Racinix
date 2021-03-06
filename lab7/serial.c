#include "serial.h"
#include <stdlib.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <stdbool.h>

#define DELAY_US	100
#define SERIAL_NUM_PORTS	2

#define BIT(n) (0x01<<(n))
#define WORD_MSB(x)			((x) >> 8)
#define WORD_LSB(x)			((x) & 0xFF)

static queue_t *serial_transmit_queue[SERIAL_NUM_PORTS] = { NULL };
static queue_t *serial_receive_queue[SERIAL_NUM_PORTS] = { NULL };

static unsigned num_queued_strings[SERIAL_NUM_PORTS] = { 0 };

static int serial_port_number_to_address(unsigned char port_number);
static int serial_port_number_to_irq_line(unsigned char port_number);
static int serial_polled_transmit_char(int base_address, unsigned char character);
static int serial_polled_receive_char(int base_address, unsigned long *character);
static int serial_clear_UART_receive_queue(unsigned char port_number);
static int serial_clear_transmit_queue(unsigned char port_number);

int serial_subscribe_int(unsigned *hook_id, unsigned char port_number, unsigned char trigger_level)
{
	unsigned char hook_bit = (unsigned char)*hook_id;
	if (hook_bit != *hook_id)
	{
		return -1;
	}
	int irq_line;
	if ((irq_line = serial_port_number_to_irq_line(port_number)) == -1)
	{
		return -1;
	}

	// Set FIFO
	if (trigger_level > 2) return -1;
	if (sys_outb(serial_port_number_to_address(port_number) + UART_REGISTER_FCR,
			BIT(UART_REGISTER_FCR_ENABLE_FIFO) |
			BIT(UART_REGISTER_FCR_CLEAR_RECEIVE_FIFO) |
			BIT(UART_REGISTER_FCR_CLEAR_TRANSMIT_FIFO) |
			(trigger_level << UART_REGISTER_FCR_FIFO_INT_TRIGGER_LVL)
	)) return -1;

	// Set interrupts
	if (sys_outb(serial_port_number_to_address(port_number) + UART_REGISTER_IER,
			BIT(UART_REGISTER_IER_RECEIVED_DATA_INT) |
			BIT(UART_REGISTER_IER_TRANSMITTER_EMPTY_INT) |
			BIT(UART_REGISTER_IER_RECEIVER_LSR_INT)
	)) return -1;

	// Create queues
	--port_number;
	if ((serial_transmit_queue[port_number] = queue_create()) == NULL)
	{
		return -1;
	}
	if ((serial_receive_queue[port_number] = queue_create()) == NULL)
	{
		return -1;
	}

	// Tell Minix we want to subscribe the interrupts
	if (sys_irqsetpolicy(irq_line, IRQ_REENABLE | IRQ_EXCLUSIVE, hook_id) == OK)
	{
		return hook_bit;
	}
	return -1;
}

int serial_set(unsigned char port_number, unsigned long bits, unsigned long stop, long parity, unsigned long rate)
{
	int base_address;
	if ((base_address = serial_port_number_to_address(port_number)) == -1)
	{
		return 1;
	}

	unsigned long lcr;
	if (sys_inb(base_address + UART_REGISTER_LCR, &lcr)) return 1;

	lcr &= BIT(UART_REGISTER_LCR_BREAK_CONTROL_BIT);
	lcr |= BIT(UART_REGISTER_LCR_DLAB_BIT); // Set to 1 because we are going to change the rate

	// Set the bits per char in an efficient way
	bits -= 5;
	if (bits > 3) return 1;
	lcr |= bits << UART_REGISTER_LCR_BITS_PER_CHAR_BIT;

	if (stop == 2)
	{
		lcr |= BIT(UART_REGISTER_LCR_NUM_STOP_BITS_BIT);
	}

	switch (parity)
	{
	case -1: break;
	case 0:
		lcr |= (3 << UART_REGISTER_LCR_PARITY_CONTROL_BIT);
		break;
		//11
	case 1:
		lcr |= (1 << UART_REGISTER_LCR_PARITY_CONTROL_BIT);
		break;
		//1
	default: return 1;
	}

	if (sys_outb(base_address + UART_REGISTER_LCR, lcr))
	{
		return 1;
	}

	rate = UART_MAX_BITRATE / rate;
	if (sys_outb(base_address + UART_REGISTER_DLL, WORD_LSB(rate)))
	{
		return 1;
	}

	if (sys_outb(base_address + UART_REGISTER_DLM, WORD_MSB(rate)))
	{
		return 1;
	}

	if (sys_outb(base_address + UART_REGISTER_LCR, lcr & ~BIT(UART_REGISTER_LCR_DLAB_BIT)))
	{
		return 1;
	}

	return 0;
}

int serial_interrupt_transmit_string(unsigned char port_number, unsigned char *string)
{
	int base_address;
	if ((base_address = serial_port_number_to_address(port_number)) == -1)
	{
		return 1;
	}
	--port_number;
	unsigned char *character;
	size_t i;
	while (strlen(string) > 0)
	{
		if ((character = malloc(sizeof(unsigned char))) == NULL)
		{
			return 1;
		}
		*character = *string;
		if (!queue_push(serial_transmit_queue[port_number], character))
		{
			return 1;
		}
		++string;
	}
	if ((character = malloc(sizeof(unsigned char))) == NULL)
	{
		return 1;
	}
	*character = SERIAL_STRING_TERMINATION_CHAR;
	if (!queue_push(serial_transmit_queue[port_number], character))
	{
		return 1;
	}

	if (serial_clear_transmit_queue(port_number + 1))
	{
		return 1;
	}

	return 0;
}

int serial_interrupt_receive_string(unsigned char port_number, unsigned char **string)
{
	int base_address;
	if ((base_address = serial_port_number_to_address(port_number)) == -1)
	{
		return 1;
	}

	--port_number;
	unsigned char *character;
	int i = -1;
	*string = NULL;
	do
	{
		++i;
		if ((*string = realloc(*string, (i + 1) * sizeof(**string))) == NULL)
		{
			return 1;
		}
		character = queue_pop(serial_receive_queue[port_number]);
		(*string)[i] = *character;
		free(character);
	} while ((*string)[i] != SERIAL_STRING_TERMINATION_CHAR);
	--num_queued_strings[port_number];

	printf("Read string %s\n", *string);

	return 0;
}

int serial_get_num_queued_strings(unsigned char port_number)
{
	--port_number;
	if (port_number > SERIAL_NUM_PORTS - 1) return -1;
	else return num_queued_strings[port_number];
}

int serial_int_handler(unsigned char port_number)
{
	int base_address;
	if ((base_address = serial_port_number_to_address(port_number)) == -1)
	{
		return 1;
	}

	unsigned long iir;
	if (sys_inb(base_address + UART_REGISTER_IIR, &iir)) return 1;
	iir >>= UART_REGISTER_IIR_INTERRUPT_ORIGIN_BIT;
	iir &= 3;
	switch (iir)
	{
	case 0: // Modem Status
		printf("---- Interrupt: Modem Status ----\n");
		break;
	case 1: // Transmitter Empty
		printf("---- Interrupt: Transmitter Empty ----\n");
		if (serial_clear_transmit_queue(port_number))
		{
			return 1;
		}
		break;
	case 2: // Received Data Available
		printf("---- Interrupt: Received Data Available ----\n");
	case 4: // Character Timeout Indication
		printf("---- Interrupt: Character Timeout Indication ----\n");
		if (serial_clear_UART_receive_queue(port_number))
		{
			return 1;
		}
		break;
	case 3: // Line Status
		printf("---- Interrupt: Line Status ----\n");
		break;
	default:
		break;
	}

	return 0;
}

int serial_polled_transmit_string(unsigned char port_number, unsigned char *string)
{
	int base_address;
	if ((base_address = serial_port_number_to_address(port_number)) == -1)
	{
		return 1;
	}
	while (strlen(string) > 0)
	{
		if (serial_polled_transmit_char(base_address, *string))
		{
			return 1;
		}
		++string;
	}
	return 0;
}

int serial_polled_receive_string(unsigned char port_number, unsigned char **string)
{
	int base_address;
	if ((base_address = serial_port_number_to_address(port_number)) == -1)
	{
		return 1;
	}
	*string = NULL;
	unsigned long character;
	size_t i = 0;
	do
	{
		if (serial_polled_receive_char(base_address, &character))
		{
			free(*string);
			return 1;
		}
		if ((*string = realloc(*string, (i + 1) * sizeof(**string))) == NULL)
		{
			free(*string);
			return 1;
		}
		(*string)[i] = character;
		++i;
	} while (character != '.');
	return 0;
}

int serial_unsubscribe_int(unsigned hook_id, unsigned char port_number)
{
	// Unsubscribe interrupts
	if (sys_outb(serial_port_number_to_address(port_number) + UART_REGISTER_IER, 0)) return 1;

	// Delete queues
	--port_number;
	queue_delete(serial_transmit_queue[port_number]);
	serial_transmit_queue[port_number] = NULL;
	queue_delete(serial_receive_queue[port_number]);
	serial_receive_queue[port_number] = NULL;

	// Tell Minix we don't want to subscribe the interrupts anymore
	if (sys_irqrmpolicy(&hook_id) == OK)
	{
		return 0;
	}
	return 1;
}

static int serial_port_number_to_address(unsigned char port_number)
{
	switch (port_number)
	{
	case 1:
		return UART_PORT_COM1_BASE_ADDRESS;
	case 2:
		return UART_PORT_COM2_BASE_ADDRESS;
	default:
		return -1;
	}
}

static int serial_polled_transmit_char(int base_address, unsigned char character)
{
	size_t num_tries;
	for (num_tries = 0; num_tries < SERIAL_NUM_TRIES; ++num_tries, tickdelay(micros_to_ticks(DELAY_US)))
	{
		unsigned long lsr;
		if (sys_inb(base_address + UART_REGISTER_LSR, &lsr))
		{
			return 1;
		}
		if (lsr & BIT(UART_REGISTER_LSR_THR_EMPTY_BIT))
		{
			if (sys_outb(base_address + UART_REGISTER_THR, character))
			{
				return 1;
			}
			return 0;
		}
	}
	return 1; // Transmitter Holding Register full
}

static int serial_polled_receive_char(int base_address, unsigned long *character)
{
	size_t num_tries;
	for (num_tries = 0; num_tries < SERIAL_NUM_TRIES; ++num_tries, tickdelay(micros_to_ticks(DELAY_US)))
	{
		unsigned long lsr;
		if (sys_inb(base_address + UART_REGISTER_LSR, &lsr))
		{
			return 1;
		}
		if (lsr & BIT(UART_REGISTER_LSR_RECEIVER_DATA_BIT))
		{
			if (lsr & (BIT(UART_REGISTER_LSR_OVERRUN_ERROR_BIT) | BIT(UART_REGISTER_LSR_PARITY_ERROR_BIT) | BIT(UART_REGISTER_LSR_FRAMING_ERROR_BIT)))
			{
				return 1;
			}
			if (sys_inb(base_address + UART_REGISTER_RBR, character))
			{
				return 1;
			}
			return 0;
		}
	}
	return 1;
}

static int serial_port_number_to_irq_line(unsigned char port_number)
{
	switch(port_number)
	{
	case 1:
		return UART_PORT_COM1_IRQ;
	case 2:
		return UART_PORT_COM2_IRQ;
	default:
		return -1;
	}
}

static int serial_clear_transmit_queue(unsigned char port_number)
{
	int base_address = serial_port_number_to_address(port_number);
	unsigned char *character;
	--port_number;
	while (!queue_empty(serial_transmit_queue[port_number]))
	{
		unsigned long lsr;
		if (sys_inb(base_address + UART_REGISTER_LSR, &lsr)) return 1;
		if (!(lsr & BIT(UART_REGISTER_LSR_THR_EMPTY_BIT)))
		{
			break;
		}
		character = queue_pop(serial_transmit_queue[port_number]);
		if (sys_outb(base_address + UART_REGISTER_THR, *character)) return 1;
		free(character);
		if (sys_inb(base_address + UART_REGISTER_LSR, &lsr)) return 1;
	}
	return 0;
}

static int serial_clear_UART_receive_queue(unsigned char port_number)
{
	int base_address = serial_port_number_to_address(port_number);
	--port_number;
	unsigned long lsr;
	if (sys_inb(base_address + UART_REGISTER_LSR, &lsr))
	{
		return 1;
	}
	void *character;
	while (lsr & BIT(UART_REGISTER_LSR_RECEIVER_DATA_BIT))
	{
		if ((character = malloc(sizeof(unsigned long))) == NULL)
		{
			return 1;
		}
		if (lsr & (BIT(UART_REGISTER_LSR_OVERRUN_ERROR_BIT) | BIT(UART_REGISTER_LSR_PARITY_ERROR_BIT) | BIT(UART_REGISTER_LSR_FRAMING_ERROR_BIT)))
		{
			return -1;
		}
		if (sys_inb(base_address + UART_REGISTER_RBR, character))
		{
			return 1;
		}
		if (*(unsigned char *)character == SERIAL_STRING_TERMINATION_CHAR)
		{
			++num_queued_strings[port_number];
		}
		if (sys_inb(base_address + UART_REGISTER_LSR, &lsr))
		{
			free(character);
			return 1;
		}
		if (!queue_push(serial_receive_queue[port_number], character))
		{
			free(character);
			return 1;
		}
	}
	return 0;
}
