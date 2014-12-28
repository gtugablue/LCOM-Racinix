#include "minix/syslib.h"
#include "minix/driver.h"

void rtc_test_conf();
void rtc_test_ints(unsigned int n);
void rtc_test_state();
void rtc_enable_interrupts();
void rtc_disable_interrupts();
//static unsigned char rtc_bcd_to_bin(unsigned char bin_coded_dec);
int rtc_get_config(unsigned long regs[]);
int rtc_get_time(unsigned long *hour, unsigned long *min, unsigned long *sec);
int rtc_get_date(unsigned long *dia, unsigned long *mes, unsigned long *ano);
int rtc_get_alarm(unsigned long *hour, unsigned long *min, unsigned long *sec);
unsigned char rtc_bin2bcd(unsigned char bin);
int rtc_set_delta_alarm(unsigned int n);
void rtc_print2bin(int j);
