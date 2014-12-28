#include "lab6.h"
#include "rtc.h"

static unsigned char rtc_bcd_to_bin(unsigned char bin_coded_dec);

void rtc_disable_interrupts()
{
	asm("cli");
}

void rtc_enable_interrupts()
{
	asm("sti");
}

static unsigned char rtc_bcd_to_bin(unsigned char bin_coded_dec)
{
	return ((bin_coded_dec >> 4) * 10) + (bin_coded_dec & 0x0F);
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

unsigned char rtc_bin2bcd(unsigned char bin)
{
	unsigned char unidades, dezenas;
	unidades = bin % 10;
	dezenas = bin / 10;
	dezenas = dezenas << 4;
	return dezenas + unidades;
}

int rtc_set_delta_alarm(unsigned int n)
{
	unsigned long sec, min, hour;
	unsigned long res;

	rtc_get_time(&hour, &min, &sec);

	unsigned time = hour * 60 * 60 + min * 60 + sec;
	time +=n;

	hour = time / (60 * 60);
	min = (time / 60) % 60;
	sec = time % 60;

	rtc_disable_interrupts();

	if(sys_outb(RTC_ADDR_REG, RTC_CTRL_REG_B)) return 1;
	if(sys_inb(RTC_DATA_REG, &res)) return 1;

	if(!(res & 0x04))
	{
		sec = rtc_bin2bcd(sec);
		min = rtc_bin2bcd(min);
		hour = rtc_bin2bcd(hour);
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

void rtc_print2bin(int i)
{
	int j;
	for (j=0; j<8;j++)
	{
		printf("%d",i&0x01);
		i=i>>1;
	}
}

void rtc_test_conf()
{
	unsigned long registers[4];
	unsigned long sec, min, hour, a_sec, a_min, a_hour, dia, mes, ano;

	printf("Executing rtc_test_conf()\n");

	rtc_get_config(registers);
	rtc_get_time(&hour, &min, &sec);
	rtc_get_date(&dia, &mes,  &ano);
	rtc_get_alarm (&a_hour, &a_min, &a_sec);
	printf("RTC:\n");

	printf("REG_A: ");
	rtc_print2bin(registers[0]);
	printf("\n");
	printf("REG_B: ");
	rtc_print2bin(registers[1]);
	printf("\n");
	printf("REG_C: ");
	rtc_print2bin(registers[2]);
	printf("\n");
	printf("REG_D: ");
	rtc_print2bin(registers[3]);
	printf("\n\n");

	/*printf("Reg_A = 0x%x\n", registers[0]);
	printf("Reg_B = 0x%x\n", registers[1]);
	printf("Reg_C = 0x%x\n", registers[2]);
	printf("Reg_D = 0x%x\n", registers[3]);*/

	printf("Date: %d-%d-%d\n", dia, mes, ano);
	printf("Time: %d:%d:%d\n", hour, min, sec);
	printf("Alarm: %d:%d:%d\n\n", a_hour, a_min, a_sec);
}

void rtc_test_ints(unsigned int n) {
	printf("Executing test_ints(%u)\n", n);

	unsigned long registers[4];
	unsigned long sec, min, hour, a_sec, a_min, a_hour, dia, mes, ano;

	printf("Executing rtc_test_conf()\n");

	rtc_set_delta_alarm(n);

	rtc_get_config (registers);
	rtc_get_time(&hour, &min, &sec);
	rtc_get_date(&dia, &mes,  &ano);
	rtc_get_alarm (&a_hour, &a_min, &a_sec);
	printf("RTC:\n");

	printf("REG_A: ");
	rtc_print2bin(registers[0]);
	printf("\n");
	printf("REG_B: ");
	rtc_print2bin(registers[1]);
	printf("\n");
	printf("REG_C: ");
	rtc_print2bin(registers[2]);
	printf("\n");
	printf("REG_D: ");
	rtc_print2bin(registers[3]);
	printf("\n\n");

	/*printf("Reg_A = 0x%x\n", registers[0]);
	printf("Reg_B = 0x%x\n", registers[1]);
	printf("Reg_C = 0x%x\n", registers[2]);
	printf("Reg_D = 0x%x\n", registers[3]);*/

	printf("Date: %d-%d-%d\n", dia, mes, ano);
	printf("Time: %d:%d:%d\n", hour, min, sec);
	printf("Alarm: %d:%d:%d\n\n", a_hour, a_min, a_sec);
}

void rtc_test_state()
{
	printf("Executing test_state()\n");
}

int main (int argc, char *argv[])
{
	sef_startup();
	/* Enable IO-sensitive operations for ourselves */
	sys_enable_iop(SELF);

	rtc_test_conf();
	rtc_set_delta_alarm(300);
	rtc_test_conf();

	return 0;
}
