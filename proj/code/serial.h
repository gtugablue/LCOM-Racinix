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
 * @brief Subscribes the serial interrupts
 *
 * @param *hook_id the memory address where the hook_id will be stored
 * @param port_number the number of the serial port
 * @param trigger_level the number of characters in the reception
 *
 * @return Return hook_id upon success, -1 upon failure
 */
int serial_subscribe_int(unsigned *hook_id, unsigned char port_number, unsigned char trigger_level);

/**
 * @brief Change the serial port configurations
 *
 * @param port_number number of the serial port
 * @param bits the number of bits per character
 * @param stop the number of stop bits
 * @param parity the parity
 * @param rate the bit-rate
 *
 * @return Return 0 on success, non-zero otherwise
 */
int serial_set(unsigned char port_number, unsigned long bits, unsigned long stop, long parity, unsigned long rate);

/**
 * @brief Transmits strings in interruption mode
 *
 * @param port_number number of the serial port
 * @param string string to be transmitted
 *
 * @return Return 0 on success, non-zero otherwise
 */
int serial_interrupt_transmit_string(unsigned char port_number, unsigned char *string);

/**
 * @brief Receives strings in interruption mode
 *
 * @param port_number number of the serial port
 * @param string string to be received
 *
 * @return Return 0 on success, non-zero otherwise
 */
int serial_interrupt_receive_string(unsigned char port_number, unsigned char **string);

/**
 * @brief Calculate the number of strings in queue ready to be read
 *
 * @param port_number number of the serial port
 *
 * @return Return 0 on success, non-zero otherwise
 */
int serial_get_num_ready_strings(unsigned char port_number);

/**
 * @brief Handle interrupts of the serial port
 *
 * @param port_number number of the serial port
 *
 * @return Return 0 on success, non-zero otherwise
 */
int serial_int_handler(unsigned char port_number);

/**
 * @brief Transmits strings in polled mode
 *
 * @param port_number number of the serial port
 * @param *string address to the string to be transmitted
 *
 * @return Return 0 on success, non-zero otherwise
 */
int serial_polled_transmit_string(unsigned char port_number, unsigned char *string);

/**
 * @brief Receives strings in polled mode
 *
 * @param port_number number of the serial port
 * @param string string to be received
 *
 * @return Return 0 on success, non-zero otherwise
 */
int serial_polled_receive_string(unsigned char port_number, unsigned char **string);

/**
 * @brief Unsubscribe serial port interrupts
 *
 * @param hook_id the memory address where the hook_id will be stored
 * @param port_number number of the serial port
 *
 * @return Return 0 on success, non-zero otherwise
 */
int serial_unsubscribe_int(unsigned hook_id, unsigned char port_number);


/** @} end of serial */
#endif
