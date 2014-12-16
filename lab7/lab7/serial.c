#include "serial.h"
#include <stdlib.h>
#include <minix/syslib.h>
#include <minix/drivers.h>

#define DELAY_US	100
#define BIT(n) (0x01<<(n))
#define WORD_MSB(x)			((x) >> 8)
#define WORD_LSB(x)			((x) & 0xFF)

queue_t *serial_queue = NULL;

static int serial_port_number_to_address(unsigned char port_number);
static int serial_port_number_to_irq_line(unsigned char port_number);
static int serial_transmit_char(int base_address, unsigned char character);
static int serial_receive_char(int base_address, unsigned long *character);

int serial_subscribe_int(unsigned *hook_id, unsigned char port_number)
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
	if (sys_irqsetpolicy(irq_line, IRQ_REENABLE | IRQ_EXCLUSIVE, hook_id) == OK)
	{
		if ((serial_queue = queue_create()) == NULL)
		{
			return -1;
		}
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

	if (sys_outb(base_address + UART_REGISTER_LCR, lcr | BIT(UART_REGISTER_LCR_DLAB_BIT)))
	{
		return 1;
	}

	return 0;
}

queue_t *serial_get_queue()
{
	return serial_queue;
}

int serial_int_handler()
{

}

int serial_transmit_string(unsigned char port_number, unsigned char *string)
{
	int base_address;
	if ((base_address = serial_port_number_to_address(port_number)) == -1)
	{
		return 1;
	}
	while (strlen(string) > 0)
	{
		if (serial_transmit_char(base_address, *string))
		{
			return 1;
		}
		++string;
	}
	return 0;
}

int serial_receive_string(unsigned char port_number, unsigned char **string)
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
		if (serial_receive_char(base_address, &character))
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
	free(serial_queue);
	serial_queue = NULL;
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

static int serial_transmit_char(int base_address, unsigned char character)
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

static int serial_receive_char(int base_address, unsigned long *character)
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
