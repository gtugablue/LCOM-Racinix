#ifndef _SERIAL_H
#define _SERIAL_H

#include "uart.h"
#include "queue.h"

#define SERIAL_NUM_TRIES		10

int serial_subscribe_int(unsigned *hook_id, unsigned char port_number);

int serial_set(unsigned char port_number, unsigned long bits, unsigned long stop, long parity, unsigned long rate);

queue_t *serial_get_queue();

int serial_int_handler();

int serial_transmit_string(unsigned char port_number, unsigned char *string);

int serial_receive_string(unsigned char port_number, unsigned char **string);

int serial_unsubscribe_int(unsigned hook_id, unsigned char port_number);

#endif
