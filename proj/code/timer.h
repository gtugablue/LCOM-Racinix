#ifndef __TIMER_H
#define __TIMER_H

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/driver.h>
#include <stdbool.h>
#include "i8254.h"

#define TIMER_DEFAULT_FREQ	60

#define WORD_MSB(x)			((x) >> 8)
#define WORD_LSB(x)			((x) & 0xFF)
#define MAX_BCD_IN_WORD		9999
#define MAX_BINARY_IN_WORD	((1 << 16) - 1)

/** @defgroup timer timer
 * @{
 *
 * Functions for using the i8254 timers
 */

/**
 * @brief Configures a timer to generate a square wave
 * 
 * Does not change the LSB (BCD/binary) of the timer's control word.
 * 
 * @param timer Timer to configure. (Ranges from 0 to 2)
 * @param freq Frequency of the square wave to generate
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_set_square(unsigned long timer, unsigned long freq);

/**
 * @brief Subscribes and enables Timer 0 interrupts
 *
 * @return Returns bit order in interrupt mask; negative value on failure
 */
int timer_subscribe_int(void );

/**
 * @brief Unsubscribes Timer 0 interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_unsubscribe_int();

/**
 * @brief Timer 0 interrupt handler
 *
 * Increments counter
 */
void timer_int_handler();

/**
 * @brief Reads the input timer configuration via read-back command
 *
 * @param timer Timer whose config to read (Ranges from 0 to 2)
 * @param st    Address of memory position to be filled with the timer config
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_get_conf(unsigned long timer, unsigned long *st);

/**
 * @brief Enables the speaker
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_enable_speaker();

/**
 * @brief Disables the speaker
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_disable_speaker();

/**
 * @brief Shows timer configuration
 * 
 * Displays in a human friendly way, the configuration of a timer
 *  as read via the read-back command, by providing the values 
 *  (and meanings) of the different components of a timer configuration 
 *
 * @param conf configuration to display in human friendly way
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_display_conf(unsigned char conf);

/**
 * @brief Tests programming timer in square wave mode
 *
 * Programs Timer 0 to generate square mode with input frequency
 *
 * @param freq Frequency of square wave to generate
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_test_square(unsigned long timer, unsigned long freq);

/**
 * @brief Tests Timer 0 interrupt handling
 *
 * Subscribes Timer 0 interrupts and prints a message once
 *  per second for the specified time interval
 * Must not be used with set_repetitive_task() and the timer 0
 *  frequency must be set to the one Minix sets it by default
 *
 * @param time Length of time interval while interrupts are subscribed
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_test_int(unsigned long time);

/**
 * @brief Tests display of timer config
 *
 * Just calls timer_get_conf() followed by timer_display_conf()
 *
 * @param timer Timer whose config to read (Ranges from 0 to 2)
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_test_config(unsigned long timer);

/**
 * @brief Sets a repetitive task
 *
 * Sets Timer 0 with a given frequency and subscribes its interrupts
 * When an interrupt is received a specific function is called
 * Useful for timing frames
 *
 * @param freq Frequency of square wave to generate in Timer 0
 * @param func Function (with no arguments) to call when an
 * 	interrupt is received
 * @return Return 0 upon success and non-zero otherwise
 */
int set_repetitive_task(unsigned long freq, void(*func)());

/**
 * @brief Stops the repetitive task
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int stop_repetitive_task();

/** @} end of timer */
#endif /* __TIMER_H */
