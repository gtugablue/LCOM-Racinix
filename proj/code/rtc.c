#include "rtc.h"
#include "minix/driver.h"

#define BIT(n) (0x01<<(n))

static unsigned char rtc_bcd_to_bin(unsigned char bin_coded_dec);
static unsigned char rtc_bin_to_bcd(unsigned char bin);
static inline void rtc_disable_interrupts();
static inline void rtc_enable_interrupts();

int rtc_subscribe_int(unsigned *hook_id, bool PIE, bool AIE, bool UIE)
{
	unsigned char hook_bit = (unsigned char)*hook_id;
	if (hook_bit >= (1 << 8))
	{
		return -1;
	}
	if (sys_irqsetpolicy(RTC_IRQ_LINE, IRQ_REENABLE | IRQ_EXCLUSIVE, hook_id))
	{
		return -1;
	}
	unsigned long res;

	if (sys_outb(RTC_ADDR_REG, RTC_CTRL_REG_B)) return 1;
	if (sys_inb(RTC_DATA_REG, &res)) return 1;
	res &= ~BIT(RTC_REGB_PIE_BIT);
	res &= ~BIT(RTC_REGB_AIE_BIT);
	res &= ~BIT(RTC_REGB_UIE_BIT);
	res |= PIE << RTC_REGB_PIE_BIT;
	res |= AIE << RTC_REGB_AIE_BIT;
	res |= UIE << RTC_REGB_UIE_BIT;
	if (sys_outb(RTC_ADDR_REG, RTC_CTRL_REG_B)) return 1;
	if (sys_outb(RTC_DATA_REG, res)) return 1;

	return hook_bit;
}

int rtc_get_config(unsigned long regs[])
{
	rtc_disable_interrupts(); // sem interrupcoes
	if(sys_outb(RTC_ADDR_REG, RTC_CTRL_REG_A))return 1;
	if(sys_inb(RTC_DATA_REG, &regs[0]))return 1;
	if(sys_outb(RTC_ADDR_REG, RTC_CTRL_REG_B))return 1;
	if(sys_inb(RTC_DATA_REG, &regs[1]))return 1;
	if(sys_outb(RTC_ADDR_REG, RTC_CTRL_REG_C))return 1;
	if(sys_inb(RTC_DATA_REG, &regs[2]))return 1;
	if(sys_outb(RTC_ADDR_REG,  RTC_CTRL_REG_D))return 1;
	if(sys_inb(RTC_DATA_REG, &regs[3]))return 1;

	rtc_enable_interrupts();
}

int rtc_get_time(unsigned long *hour, unsigned long *min, unsigned long *sec)
{
	rtc_disable_interrupts();

	unsigned long res;

	do
	{
		if (sys_outb(RTC_ADDR_REG, RTC_CTRL_REG_A)) return 1;
		if (sys_inb(RTC_DATA_REG, &res)) return 1;
	} while(res & 0x80);


	if (sys_outb(RTC_ADDR_REG, RTC_ADDRESS_HOURS)) return 1;
	if (sys_inb(RTC_DATA_REG, hour)) return 1;

	do
	{
		if (sys_outb(RTC_ADDR_REG, RTC_CTRL_REG_A)) return 1;
		if (sys_inb(RTC_DATA_REG, &res)) return 1;
	} while(res & 0x80);

	if(sys_outb(RTC_ADDR_REG, RTC_ADDRESS_MINUTES))return 1;
	if(sys_inb(RTC_DATA_REG, min))return 1;

	do
	{
		sys_outb(RTC_ADDR_REG, RTC_CTRL_REG_A);
		sys_inb(RTC_DATA_REG, &res);
	} while(res & 0x80);

	if(sys_outb(RTC_ADDR_REG, RTC_ADDRESS_SECONDS)) return 1;
	if(sys_inb(RTC_DATA_REG, sec)) return 1;


	if(sys_outb(RTC_ADDR_REG, RTC_CTRL_REG_B)) return 1;
	if(sys_inb(RTC_DATA_REG, &res)) return 1;

	if(!(res & 0x04))
	{
		*sec = rtc_bcd_to_bin(*sec);
		*min = rtc_bcd_to_bin(*min);
		*hour = rtc_bcd_to_bin(*hour);
	}

	//if(res & 0x02)
	//	*hour += 12;

	rtc_enable_interrupts();
	return 0;
}

int rtc_get_date(unsigned long *dia, unsigned long *mes, unsigned long *ano)
{
	unsigned long res;
	rtc_disable_interrupts();

	if(sys_outb(RTC_ADDR_REG, RTC_ADDRESS_DAY_MONTH)) return 1;
	if(sys_inb(RTC_DATA_REG, dia)) return 1;

	if(sys_outb(RTC_ADDR_REG, RTC_ADDRESS_MONTH)) return 1;
	if(sys_inb(RTC_DATA_REG, mes)) return 1;

	if(sys_outb(RTC_ADDR_REG, RTC_ADDRESS_YEAR)) return 1;
	if(sys_inb(RTC_DATA_REG, ano)) return 1;


	if(sys_outb(RTC_ADDR_REG, RTC_CTRL_REG_B)) return 1;
	if(sys_inb(RTC_DATA_REG, &res)) return 1;

	if(!(res & 0x04))
	{
		*dia = rtc_bcd_to_bin(*dia);
		*mes = rtc_bcd_to_bin(*mes);
		*ano = rtc_bcd_to_bin(*ano);
	}

	rtc_enable_interrupts();

	return 0;
}

int rtc_get_alarm(unsigned long *hour, unsigned long *min, unsigned long *sec)
{
	unsigned long res;

	rtc_disable_interrupts();

	if(sys_outb(RTC_ADDR_REG, RTC_ADDRESS_SECONDS_ALARM)) return 1;
	if(sys_inb(RTC_DATA_REG, sec)) return 1;

	if(sys_outb(RTC_ADDR_REG, RTC_ADDRESS_MINUTES_ALARM)) return 1;
	if(sys_inb(RTC_DATA_REG, min)) return 1;

	if(sys_outb(RTC_ADDR_REG, RTC_ADDRESS_HOURS_ALARM)) return 1;
	if(sys_inb(RTC_DATA_REG, hour)) return 1;



	if (sys_outb(RTC_ADDR_REG, RTC_CTRL_REG_B)) return 1;
	if (sys_inb(RTC_DATA_REG, &res)) return 1;

	if(!(res & 0x04))
	{
		*sec = rtc_bcd_to_bin(*sec);
		*min = rtc_bcd_to_bin(*min);
		*hour = rtc_bcd_to_bin(*hour);
	}


	rtc_enable_interrupts();
	return 0;
}

int rtc_set_delta_alarm(unsigned n)
{
	unsigned long sec, min, hour;
	unsigned long res;

	rtc_get_time(&hour, &min, &sec);

	unsigned long time = hour * 60 * 60 + min * 60 + sec;
	time +=n;

	hour = time / (60 * 60);
	min = (time / 60) % 60;
	sec = time % 60;

	rtc_disable_interrupts();

	if(sys_outb(RTC_ADDR_REG, RTC_CTRL_REG_B)) return 1;
	if(sys_inb(RTC_DATA_REG, &res)) return 1;

	if(!(res & 0x04))
	{
		sec = rtc_bin_to_bcd(sec);
		min = rtc_bin_to_bcd(min);
		hour = rtc_bin_to_bcd(hour);
	}

	if(sys_outb(RTC_ADDR_REG, RTC_ADDRESS_SECONDS_ALARM)) return 1;
	if(sys_outb(RTC_DATA_REG, (unsigned long *)sec)) return 1;

	if(sys_outb(RTC_ADDR_REG, RTC_ADDRESS_MINUTES_ALARM)) return 1;
	if(sys_outb(RTC_DATA_REG, (unsigned long *)min)) return 1;

	if(sys_outb(RTC_ADDR_REG, RTC_ADDRESS_HOURS_ALARM)) return 1;
	if(sys_outb(RTC_DATA_REG, (unsigned long *)hour)) return 1;

	rtc_enable_interrupts();
	return 0;
}

int rtc_int_handler(bool *PIE, bool *AIE, bool *UIE)
{
	unsigned long res;
	if (sys_outb(RTC_ADDR_REG, RTC_CTRL_REG_C)) return 1;
	if (sys_inb(RTC_DATA_REG, &res)) return 1;
	*PIE = res & BIT(RTC_REGB_PIE_BIT);
	*AIE = res & BIT(RTC_REGB_AIE_BIT);
	*UIE = res & BIT(RTC_REGB_UIE_BIT);
	return 0;
}

int rtc_unsubscribe_int(unsigned hook_id)
{
	unsigned long res;
	if(sys_outb(RTC_ADDR_REG, RTC_CTRL_REG_B)) return 1;
	if(sys_inb(RTC_DATA_REG, &res)) return 1;
	res &= ~BIT(RTC_REGB_PIE_BIT);
	res &= ~BIT(RTC_REGB_AIE_BIT);
	res &= ~BIT(RTC_REGB_UIE_BIT);
	if(sys_outb(RTC_ADDR_REG, RTC_CTRL_REG_B)) return 1;
	if(sys_outb(RTC_DATA_REG, res)) return 1;

	if (sys_irqrmpolicy(&hook_id))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

static unsigned char rtc_bcd_to_bin(unsigned char bin_coded_dec)
{
	return ((bin_coded_dec >> 4) * 10) + (bin_coded_dec & 0x0F);
}

static unsigned char rtc_bin_to_bcd(unsigned char bin)
{
	unsigned char unidades, dezenas;
	unidades = bin % 10;
	dezenas = bin / 10;
	dezenas = dezenas << 4;
	return dezenas + unidades;
}

static inline void rtc_disable_interrupts()
{
	asm("cli");
}

static inline void rtc_enable_interrupts()
{
	asm("sti");
}
