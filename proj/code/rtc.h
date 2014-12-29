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

int rtc_subscribe_int(unsigned *hook_id, bool PIE, bool AIE, bool UIE);

int rtc_get_config(unsigned long regs[]);

int rtc_get_time(unsigned long *hour, unsigned long *min, unsigned long *sec);

int rtc_get_date(unsigned long *dia, unsigned long *mes, unsigned long *ano);

int rtc_get_alarm(unsigned long *hour, unsigned long *min, unsigned long *sec);

int rtc_set_delta_alarm(unsigned n);

int rtc_int_handler(bool *PIE, bool *AIE, bool *UIE);

int rtc_unsubscribe_int(unsigned hook_id);

#endif
