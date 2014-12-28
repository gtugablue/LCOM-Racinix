#include "rtc.h"
#include "minix/driver.h"

int rtc_subscribe_int(unsigned *hook_id)
{
	unsigned char hook_bit = (unsigned char)*hook_id;
	if (hook_bit >= (1 << 8))
	{
		return -1;
	}
	if (sys_irqsetpolicy(RTC_IRQ_LINE, IRQ_REENABLE | IRQ_EXCLUSIVE, hook_id))
	{
		return 1;
	}
	return hook_bit;
}

int rtc_unsubscribe_int(unsigned hook_id)
{
	if (sys_irqrmpolicy(&hook_id))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
