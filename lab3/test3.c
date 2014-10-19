#include "test3.h"
#include "i8042.h"
#include <minix/keymap.h>

#define KEY_ESC		0x01

static int irq_hook_id;

// Static functions declaration
static int kbd_scancode_handler(unsigned long scancode);
static int kbd_response_handler(unsigned long response);
static int kbd_is_response(unsigned long interruption);
static void kbd_scancode_to_name(unsigned long scancode, unsigned char* name);
static int kbc_wait_for_input_buffer(unsigned long *status);
static int kbd_int_handler();

static int kbd_is_response(unsigned long interruption)
{
	if(interruption == I8042_ACK || interruption == I8042_RESEND || interruption == I8042_ERROR)
	{
		return 1;
	}
	return 0;
}

static void kbd_scancode_to_name(unsigned long scancode, unsigned char* name)
{
	size_t i;
	for (i = 0; i < sizeof(i8042_keycodes) / sizeof(struct i8042_keycode_t); ++i)
	{
		if (i8042_keycodes[i].scancode == (unsigned)(scancode & ~BIT(I8042_BREAK_CODE_BIT)))
		{
			sprintf(name, "%s", i8042_keycodes[i].name);
			return;
		}
	}
	sprintf(name, "%s", "Unknown");
	return;
}

static int kbc_wait_for_input_buffer(unsigned long *status)
{
	size_t i;
	for (i = 0; i < KBC_TRIES; ++i)
	{
		if (sys_inb(I8042_STAT_REG, status))
		{
			return 1;
		}
		if (((unsigned)*status & (I8042_STATUS_PARITY_BIT | I8042_STATUS_TIMEOUT_BIT | I8042_STATUS_IBF_BIT)) == 0)
		{
			return 0;
		}
		tickdelay(micros_to_ticks(I8042_KBD_TIMEOUT));
	}
	return 1;
}

static int kbd_int_handler()
{
	unsigned long interruption;
	if (sys_inb(I8042_OUT_BUF, &interruption))
	{
		return 1;
	}
	if (kbd_is_response(interruption))
	{
		kbd_response_handler(interruption);
	}
	else
	{
		kbd_scancode_handler(interruption);
	}
	return 0;
}

static int kbd_scancode_handler(unsigned long scancode)
{
	unsigned char* name;
	if ((name = malloc(i8042_KEYCODE_T_MAX_NAME)) == NULL)
	{
		return 1;
	}
	kbd_scancode_to_name(scancode, name);
	if (IS_BREAK_CODE(scancode))
	{
		printf("breakcode: 0x%X; name: %s\n", scancode, name);
	}
	else
	{
		printf("makecode: 0x%X; name: %s\n", scancode, name);
	}
	free(name);
	return 0;
}

static int kbd_response_handler(unsigned long response)
{
	return 0;
}

int kbd_test_scan(unsigned short ass)
{
	if (kbd_subscribe_int() == -1)
	{
		return 1;
	}
	int r, ipc_status;
	message msg;
	while (1)
	{
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			if (_ENDPOINT_P(msg.m_source) == HARDWARE) /* hardware interrupt notification */
			{
				if (msg.NOTIFY_ARG & BIT(I8042_HOOK_BIT)) { /////// /* subscribed interrupt */
					kbd_int_handler();
				}
			}
		}
	}
	return 0;
}

int kbd_test_leds(unsigned short n, unsigned short *leds) {
	/* To be completed */
}
int kbd_test_timed_scan(unsigned short n) {
	/* To be completed */
}

int kbd_subscribe_int()
{
	irq_hook_id = I8042_HOOK_BIT;
	if (sys_irqsetpolicy(I8042_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &irq_hook_id) == OK)
	{
		return I8042_HOOK_BIT;
	}
	return -1;
}
