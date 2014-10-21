#ifndef _KBD_H
#define _KBD_H

#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <minix/drivers.h>
#include "i8042.h"

#define IS_BREAK_CODE(scancode)			((scancode) & (1 << I8042_BREAK_CODE_BIT))
#define TOGGLE_LED(led, status)			((status) ^ BIT(led))

#define KBC_TRIES			10
#define KBD_HOOK_BIT		2

/** @defgroup kbd kbd
 * @{
 *
 * Functions for using the keyboard
 */

/**
 * @brief Checks if a given interruption is a response to a command
 *
 * @params interruption data read from the output buffer after the
 *  interruption occured
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_is_response(unsigned long interruption);

/**
 * @brief Subscribe the keyboard interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_subscribe_int();

/**
 * @brief Toggle a keyboard led status (1 = on; 0 = off)
 *
 * @param led
 * Bit 2 - Caps Lock indicator
 * Bit 1 - Numeric Lock indicator
 * Bit 0 - Scroll Lock indicator
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_toggle_leds(unsigned short leds);

/**
 * @brief Read the status of the KBC
 *
 * @param status memory address to write the 4-byte status at
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_read_status(unsigned long* status);

/**
 * @brief Unsubscribe the keyboard interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_unsubscribe_int();

/**
 * @brief Returns the LEDs status
 *
 * @return Returns the LEDs status
 */
unsigned char kbd_get_led_status();

#endif
