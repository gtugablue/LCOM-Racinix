#include "rtc.h"
#include "test6.h"

unsigned long data[14];
static int RTC_HOOK = 1;

int test_conf()
{
	int i;
	for(i = 0; i < 14; i++)
	{
		sys_outb(RTC_ADDR_REG, i);
		sys_inb(RTC_DATA_REG, &data[i]);
		i++;
	}

	printf("A: 0x%x \n B: 0x%x \n C: 0x%x \n D: 0x%x \n", data[10], data[11], data[12], data[13]);

}

//unsigned int long  b = rtc_leByte(RTC_STAT_B);
//b = (b | RTC_PIE);
//rtc_WriteByte(RTC_STAT_B, b);
//
//}

int test_date()
{
	int ipc_status;
	message msg;
	unsigned int i=0;
	unsigned rtc_hook_id;
	rtc_subscribe_int(&rtc_hook_id);

	while(1)
	{
		if (driver_receive(ANY, &msg, &ipc_status) != 0)
		{
			if (is_ipc_notify(ipc_status))
			{ /* received notification */
				switch (_ENDPOINT_P(msg.m_source))
				{
				case HARDWARE: /* hardware interrupt notification */
					if (msg.NOTIFY_ARG & (1 << RTC_HOOK))
					{ /* subscribed interrupt */
						i++;
					}
					break;
				default:
					break;
				}
			}
		}
	}

	rtc_unsubscribe_int(rtc_hook_id);
	sys_outb(RTC_ADDR_REG, RTC_CTRL_REG_B); ///
}

int test_int(/* something */)
{

}

