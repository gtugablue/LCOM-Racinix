#include "lab6.h"
#include "rtc.h"

void rtc_disable_interrupts()
{
	asm("cli");
}

void rtc_enable_interrupts()
{
	asm("sti");
}

/*unsigned char bcdtobin(unsigned char bin_coded_dec)
{
	return ((bin_coded_dec >> 4) * 10) + (bin_coded_dec & 0x0F);
}*/

int get_config(unsigned long regs[])
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

int get_time(int *hour, int *min, int *sec)
{
	rtc_disable_interrupts();

	unsigned long res;

	do
	{
		if (sys_outb(RTC_ADDR_REG, RTC_CTRL_REG_A)) return 1;
		if (sys_inb(RTC_DATA_REG, &res)) return 1;
	} while(res & 0x80);


	if (sys_outb(RTC_ADDR_REG, 4)) return 1;
	if (sys_inb(RTC_DATA_REG, (unsigned long *)hour)) return 1;

	do
	{
		if (sys_outb(RTC_ADDR_REG, RTC_CTRL_REG_A)) return 1;
		if (sys_inb(RTC_DATA_REG, &res)) return 1;
	} while(res & 0x80);

	if(sys_outb(RTC_ADDR_REG, 2))return 1;
	if(sys_inb(RTC_DATA_REG, (unsigned long *)min))return 1;
/*
	sys_outb(RTC_ADDR_REG, 10);
	REG_A = sys_inb(RTC_DATA_REG);

	while((REG_A&0x80) != 0)
	{
		sys_outb(RTC_ADDR_REG, 10);
		REG_A = sys_inb(RTC_DATA_REG);
	}*/
	if(sys_outb(RTC_ADDR_REG, 0)) return 1;
	if(sys_inb(RTC_DATA_REG, (unsigned long *)sec)) return 1;



/*
	if(sys_outb(RTC_ADDR_REG, RTC_CTRL_REG_B)) return 1;
	if(sys_inb(RTC_DATA_REG, &res)) return 1;

	if((res & 0x04) == 0)
	{
		*sec = bcdtobin(*sec);
		*min = bcdtobin(*min);
		*hour = bcdtobin(*hour);
	}

	if((res & 0x02) == 0)
		*hour += 12;
*/
	rtc_enable_interrupts();
	return 0;
}

int get_date(int *dia, int *mes, int *ano)
{
	unsigned char REG_B;

	rtc_disable_interrupts();

	if(sys_outb(RTC_ADDR_REG, 7)) return 1;
	if(sys_inb(RTC_DATA_REG, (unsigned long *)dia)) return 1;

	if(sys_outb(RTC_ADDR_REG, 8)) return 1;
	if(sys_inb(RTC_DATA_REG, (unsigned long *)mes)) return 1;

	if(sys_outb(RTC_ADDR_REG, 9)) return 1;
	if(sys_inb(RTC_DATA_REG, (unsigned long *)ano)) return 1;


	/*
	if(sys_outb(RTC_ADDR_REG, RTC_CTRL_REG_B)) return 1;
	if(sys_inb(RTC_DATA_REG, &res)) return 1;

	if((REG_B & 0x04) == 0)
	{
		*dia = bcd2bin(*dia);
		*mes = bcd2bin(*mes);
		*ano = bcd2bin(*ano);
	}
	*/
	rtc_enable_interrupts();
	return 0;
}

int get_alarm(int *hour, int *min, int *sec)
{
	unsigned long res;

	rtc_disable_interrupts();

	if(sys_outb(RTC_ADDR_REG, 1)) return 1;
	if(sys_inb(RTC_DATA_REG, (unsigned long *)sec)) return 1;

	if(sys_outb(RTC_ADDR_REG, 3)) return 1;
	if(sys_inb(RTC_DATA_REG, (unsigned long *)min)) return 1;

	if(sys_outb(RTC_ADDR_REG, 5)) return 1;
	if(sys_inb(RTC_DATA_REG, (unsigned long *)hour)) return 1;


	/*
	sys_outb(RTC_ADDR_REG, 11)) return 1;
	REG_B = sys_inb(RTC_DATA_REG)) return 1;

	if((REG_B & 0x04) == 0)
	{
		*sec = bcd2bin(*sec);
		*min = bcd2bin(*min);
		*hour = bcd2bin(*hour);
	}
	*/

	rtc_enable_interrupts();
	return 0;
}

/*
unsigned char bin2bcd(unsigned char bin)
{
	unsigned char unidades, dezenas;
	unidades = bin % 10;
	dezenas = bin / 10;
	dezenas = dezenas << 4;
	return dezenas + unidades;
}
*/

int set_delta_alarm(unsigned int n)
{
	int sec, min, hour;
	unsigned long res;

	get_time(&hour, &min, &sec);

	unsigned time = hour * 60 * 60 + min * 60 + sec;
	time +=n;

	hour = time / (60 * 60);
	min = (time / 60) % 60;
	sec = time % 60;

	rtc_disable_interrupts();

	if(sys_outb(RTC_ADDR_REG, RTC_CTRL_REG_B)) return 1;
	if(sys_inb(RTC_DATA_REG, &res)) return 1;

	/*
	if((REG_B & 0x04) == 0)
	{
		sec = bin2bcd(sec);
		min = bin2bcd(min);
		hour = bin2bcd(hour);
	}
	*/

	if(sys_outb(RTC_ADDR_REG, 1)) return 1;
	if(sys_outb(RTC_DATA_REG, (unsigned long *)sec)) return 1;

	if(sys_outb(RTC_ADDR_REG, 3)) return 1;
	if(sys_outb(RTC_DATA_REG, (unsigned long *)min)) return 1;

	if(sys_outb(RTC_ADDR_REG, 5)) return 1;
	if(sys_outb(RTC_DATA_REG, (unsigned long *)hour)) return 1;

	rtc_enable_interrupts();
	return 0;
}

void print2bin(int i)
{
	int j;
	for (j=0; j<8;j++)
	{
		printf("%d",i&0x01);
		i=i>>1;
	}
}


void test_confi()
{
	unsigned long registers[4];
	int sec, min, hour, a_sec, a_min, a_hour, dia, mes, ano;

	printf("Executing test_conf()\n");

	get_config(registers);
	get_time(&hour, &min, &sec);
	get_date(&dia, &mes,  &ano);
	get_alarm (&a_hour, &a_min, &a_sec);
	printf("RTC:\n");

	printf("REG_A: ");
	print2bin(registers[0]);
	printf("\n");
	printf("REG_B: ");
	print2bin(registers[1]);
	printf("\n");
	printf("REG_C: ");
	print2bin(registers[2]);
	printf("\n");
	printf("REG_D: ");
	print2bin(registers[3]);
	printf("\n\n");

	/*printf("Reg_A = 0x%x\n", registers[0]);
	printf("Reg_B = 0x%x\n", registers[1]);
	printf("Reg_C = 0x%x\n", registers[2]);
	printf("Reg_D = 0x%x\n", registers[3]);*/

	printf("Date: %d-%d-%d\n", dia, mes, ano);
	printf("Time: %d:%d:%d\n", hour, min, sec);
	printf("Alarm: %d:%d:%d\n\n", a_hour, a_min, a_sec);
}

void test_ints(unsigned int n) {
    printf("Executing test_ints(%u)\n", n);

	unsigned long registers[4];
	int sec, min, hour, a_sec, a_min, a_hour, dia, mes, ano;

	printf("Executing test_conf()\n");

	set_delta_alarm(n);

	get_config (registers);
	get_time(&hour, &min, &sec);
	get_date(&dia, &mes,  &ano);
	get_alarm (&a_hour, &a_min, &a_sec);
	printf("RTC:\n");

	printf("REG_A: ");
	print2bin(registers[0]);
	printf("\n");
	printf("REG_B: ");
	print2bin(registers[1]);
	printf("\n");
	printf("REG_C: ");
	print2bin(registers[2]);
	printf("\n");
	printf("REG_D: ");
	print2bin(registers[3]);
	printf("\n\n");

	/*printf("Reg_A = 0x%x\n", registers[0]);
	printf("Reg_B = 0x%x\n", registers[1]);
	printf("Reg_C = 0x%x\n", registers[2]);
	printf("Reg_D = 0x%x\n", registers[3]);*/

	printf("Date: %d-%d-%d\n", dia, mes, ano);
	printf("Time: %d:%d:%d\n", hour, min, sec);
	printf("Alarm: %d:%d:%d\n\n", a_hour, a_min, a_sec);
}

void test_state()
{
    printf("Executing test_state()\n");
}

int main (int argc, char *argv[])
{
	sef_startup();
	test_confi();
	return 0;
}
