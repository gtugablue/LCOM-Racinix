#ifndef _RTC_H
#define _RTC_H

#include <stdbool.h>

#define RTC_CTRL_REG_A 			10
#define RTC_CTRL_REG_B  		11
#define RTC_CTRL_REG_C  		12
#define RTC_CTRL_REG_D  		13

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71

#define RTC_ADDRESS_SECONDS 		0
#define RTC_ADDRESS_SECONDS_ALARM 	1
#define RTC_ADDRESS_MINUTES			2
#define RTC_ADDRESS_MINUTES_ALARM	3
#define RTC_ADDRESS_HOURS 			4
#define RTC_ADDRESS_HOURS_ALARM 	5
#define RTC_ADDRESS_DAY_WEEK 		6
#define RTC_ADDRESS_DAY_MONTH		7
#define RTC_ADDRESS_MONTH			8
#define RTC_ADDRESS_YEAR			9

#define RTC_IRQ_LINE 8 //irq_line

// Register B
#define RTC_REGB_SET_BIT			7
#define RTC_REGB_PIE_BIT			6
#define RTC_REGB_AIE_BIT			5
#define RTC_REGB_UIE_BIT			4
#define RTC_REGB_SQWE_BIT			3
#define RTC_REGB_DM_BIT				2
#define RTC_REGB_24_12_BIT			1
#define RTC_REGB_DSE_BIT			0

#define RTC_HOOK_BIT				14

/** @defgroup rtc RTC
 * @{
 *
 * Module that handle everything that is related to Real Time Clock
 *
 */

/**
 * @brief Subscribes RTC interrupts
 *
 * @param *hook_id the memory address where the hook_id will be stored
 * @param PIE BIT 6 of register B
 * @param AIE BIT 5 of register B
 * @param UIE BIT 4 of register B
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int rtc_subscribe_int(unsigned *hook_id, bool PIE, bool AIE, bool UIE);

/**
 * @brief Get the configuration of the regs
 *
 * @param regs the array where it will store the configurations
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int rtc_get_config(unsigned long regs[]);

/**
 * @brief Get the time of the day
 *
 * @param hour address to the hour of the day
 * @param min address to the minutes of the day
 * @param sec address to the seconds of the day
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int rtc_get_time(unsigned long *hour, unsigned long *min, unsigned long *sec);

/**
 * @brief Get the date
 *
 * @param dia address to the day of the month
 * @param mes address to the month of the year
 * @param ano address to the year
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int rtc_get_date(unsigned long *dia, unsigned long *mes, unsigned long *ano);

/**
 * @brief Get the alarm
 *
 * @param hour address to the hour of the day
 * @param min address to the minutes of the day
 * @param sec address to the seconds of the day
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int rtc_get_alarm(unsigned long *hour, unsigned long *min, unsigned long *sec);

/**
 * @brief Sets the alarm on
 *
 * Sets the alarm on n seconds from the current time of the day
 *
 * @param n the alarm will be set on when the time is a match to adding n to the current time of the day
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int rtc_set_delta_alarm(unsigned n);

/**
 * @brief Handle the interrupts of the RTC
 *
 * @param PIE address to BIT 6 of register B
 * @param AIE address to BIT 5 of register B
 * @param UIE address to BIT 4 of register B
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int rtc_int_handler(bool *PIE, bool *AIE, bool *UIE);

/**
 * @brief Unsubscribes the RTC interrupts
 *
 * @param hook_id the memory address where the hook_id will be stored
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int rtc_unsubscribe_int(unsigned hook_id);


/** @} end of rtc */
#endif
