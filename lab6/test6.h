#include "minix/syslib.h"
#include "minix/driver.h"

#define RTC_CTRL_REG_A 			10
#define RTC_CTRL_REG_B  		11
#define RTC_CTRL_REG_C  		12
#define RTC_CTRL_REG_D  		13

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71

//#define RTC_RS0 (1 << 0)
//#define RTC_RS1 (1 << 1)
//#define RTC_RS2 (1 << 2)
//#define RTC_RS3 (1 << 3)
//#define RTC_UIP (1 << 7)

//#define RTC_SET   (1 << 7)
//#define RTC_PIE   (0x40)


int test_conf();
int test_date();
int test_int(/*something*/);
int rtc_subscribe_int();
int rtc_unsubscribe_int();
