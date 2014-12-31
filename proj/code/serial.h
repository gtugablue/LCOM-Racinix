#ifndef _SERIAL_H
#define _SERIAL_H

#include "uart.h"
#include "queue.h"

#define SERIAL_NUM_TRIES				10
#define SERIAL_STRING_TERMINATION_CHAR	'\0'
#define SERIAL_HOOK_BIT					5

/** @defgroup serial Serial Port
 * @{
 *
 * Module responsible for everything that includes serial port
 *
 */

/**
 * @brief
 *
 * @param hook_id
 * @param port_number
 * @param trigger_level
 *
 * @return
 */
int serial_subscribe_int(unsigned *hook_id, unsigned char port_number, unsigned char trigger_level);

/**
 * @brief
 *
 * @param port_number
 * @param bits
 * @param stop
 * @param parity
 * @param rate
 *
 * @return
 */
int serial_set(unsigned char port_number, unsigned long bits, unsigned long stop, long parity, unsigned long rate);

/**
 * @brief
 *
 * @param port_number
 * @param string
 *
 * @return
 */
int serial_interrupt_transmit_string(unsigned char port_number, unsigned char *string);

/**
 * @brief
 *
 * @param port_number
 * @param string
 *
 * @return
 */
int serial_interrupt_receive_string(unsigned char port_number, unsigned char **string);

/**
 * @brief
 *
 * @param port_number
 *
 * @return
 */
int serial_get_num_queued_strings(unsigned char port_number);

/**
 * @brief
 *
 * @param port_number
 *
 * @return
 */
int serial_int_handler(unsigned char port_number);

/**
 * @brief
 *
 * @param port_number
 * @param string
 *
 * @return
 */
int serial_polled_transmit_string(unsigned char port_number, unsigned char *string);

/**
 * @brief
 *
 * @param port_number
 * @param string
 *
 * @return
 */
int serial_polled_receive_string(unsigned char port_number, unsigned char **string);

/**
 * @brief
 *
 * @param hook_id
 * @param port_number
 *
 * @return
 */
int serial_unsubscribe_int(unsigned hook_id, unsigned char port_number);


/** @} end of serial */
#endif
