#include "test4.h"

#define NDEBUG

#define BIT(n) (0x01<<(n))
#define IS_BIT_SET(n, bit)	(((n) & BIT(bit)) ? 1 : 0)

static void print_packet_info(unsigned char packet[]);

int test_packet(unsigned short cnt){
	unsigned hook_id = MOUSE_HOOK_BIT;
	if (mouse_subscribe_int(&hook_id) == -1)
	{
		return 1;
	}
#ifndef NDEBUG
	printf("MOUSE_HOOK_BIT = %d\n", MOUSE_HOOK_BIT);
	printf("hook_id = %d\n", hook_id);
	printf("Setting stream mode...\n");
#endif
	if (mouse_set_stream_mode(NUM_TRIES))
	{
		return 1;
	}
	if (mouse_enable_stream_mode(NUM_TRIES))
	{
		return 1;
	}
#ifndef NDEBUG
	printf("Stream mode successfully set.\n");
#endif
	unsigned char packet[MOUSE_PACKET_SIZE];
	int r, ipc_status;
	message msg;
#ifndef NDEBUG
	printf("Loop about to start...\n");
#endif
	while(cnt > 0)
	{
#ifndef NDEBUG
		printf("Looping... cnt = %d\n", cnt);
#endif
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			// Driver receive fail
#ifndef NDEBUG
			printf("Driver receive fail.\n");
#endif
			continue;
		}
#ifndef NDEBUG
		printf("Driver receive success.\n");
#endif
		if (is_ipc_notify(ipc_status)) { /* received notification */
			if (_ENDPOINT_P(msg.m_source) == HARDWARE) /* hardware interrupt notification */
			{
				if (msg.NOTIFY_ARG & BIT(MOUSE_HOOK_BIT)) {
#ifndef NDEBUG
					printf("\n-- MOUSE INTERRUPT START --\n");
#endif
					if(mouse_int_handler(NUM_TRIES))
					{
#ifndef NDEBUG
						printf("Interrupt handler error.\n");
#endif
						return 1;
					}
					if(mouse_get_packet(packet))
					{
						--cnt;
#ifndef NDEBUG
						printf("Packet is ready, printing it...\n");
#endif
						print_packet_info(packet);
#ifndef NDEBUG
						printf("Packet successfully printed.\n");
#endif
					}
#ifndef NDEBUG
					printf("\n-- MOUSE INTERRUPT END --\n");
#endif
				}
			}
		}
	}
#ifndef NDEBUG
	printf("Disabling stream mode...\n");
#endif
	if(mouse_disable_stream_mode(NUM_TRIES))
	{
		return 1;
	}
#ifndef NDEBUG
	printf("Stream mode successfully disabled.\n");
	printf("Unsubscribing mouse interrupts...\n");
#endif
	if (mouse_unsubscribe_int(hook_id))
	{
		return 1;
	}
#ifndef NDEBUG
	printf("Mouse interrupts successfully unsubscribed.\n");
#endif
	return 0;
}	

int test_async(unsigned short idle_time) {
	/* To be completed ... */
}
	
int test_config(void) {
	/* To be completed ... */
}	
	
int test_gesture(short length, unsigned short tolerance) {
    /* To be completed ... */
}

static void print_packet_info(unsigned char packet[])
{
	printf("B1=0x%X\t", packet[0]);
	printf("B2=0x%X\t", packet[1]);
	printf("B3=0x%X\t", packet[2]);
	printf("LB=%d\t", IS_BIT_SET(packet[0], MOUSE_1ST_BYTE_LEFT_BTN_BIT));
	printf("MB=%d\t", IS_BIT_SET(packet[0], MOUSE_1ST_BYTE_MIDDLE_BTN_BIT));
	printf("RB=%d\t", IS_BIT_SET(packet[0], MOUSE_1ST_BYTE_RIGHT_BTN_BIT));
	printf("XOV=%d\t", IS_BIT_SET(packet[0], MOUSE_1ST_BYTE_X_OF_BIT));
	printf("YOV=%d\t", IS_BIT_SET(packet[0], MOUSE_1ST_BYTE_Y_OF_BIT));
	printf("X=%d\t", MOUSE_PACKET_COUNTER((short)packet[1], IS_BIT_SET(packet[0], MOUSE_1ST_BYTE_X_SIGN_BIT)));
	printf("Y=%d", MOUSE_PACKET_COUNTER((short)packet[2], IS_BIT_SET(packet[0], MOUSE_1ST_BYTE_Y_SIGN_BIT)));
	printf("\n");
	return;
}
