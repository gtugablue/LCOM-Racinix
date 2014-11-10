#include "test4.h"
#include <stdbool.h>

#define BIT(n) (0x01<<(n))
#define IS_BIT_SET(n, bit)	(((n) & BIT(bit)) ? 1 : 0)

typedef enum
{
	GESTURE_INIT,
	GESTURE_DRAW
} gesture_state_t;

typedef enum
{
	LDOWN,
	LUP,
	MOVE
} event_type_t;

typedef struct
{
	event_type_t type;
} event_t;

static void print_packet_info(mouse_data_packet_t mouse_data_packet);
static void print_config(mouse_status_packet_t *mouse_status_packet);
static int test_packet_mouse_int_handler(unsigned short* cnt);
static int test_gesture_mouse_int_handler(short length, unsigned short tolerance);
static bool check_horizontal_line(short length, unsigned short tolerance, event_t *event, int x_delta, int y_delta);
static bool sameSign(int x, int y);

int test_packet(unsigned short cnt){
	unsigned hook_id = MOUSE_HOOK_BIT;
	if (mouse_subscribe_int(&hook_id) == -1)
	{
		return 1;
	}
	if (mouse_set_stream_mode(NUM_TRIES))
	{
		return 1;
	}
	if (mouse_enable_stream_mode(NUM_TRIES))
	{
		return 1;
	}
	unsigned char packet[MOUSE_PACKET_SIZE];
	int r, ipc_status;
	message msg;
	while(cnt > 0)
	{
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			// Driver receive fail
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			if (_ENDPOINT_P(msg.m_source) == HARDWARE) /* hardware interrupt notification */
			{
				if (msg.NOTIFY_ARG & BIT(MOUSE_HOOK_BIT)) {
					if (test_packet_mouse_int_handler(&cnt))
					{
						return 1;
					}
				}
			}
		}
	}
	if (mouse_disable_stream_mode(NUM_TRIES))
	{
		return 1;
	}
	if (mouse_unsubscribe_int(hook_id))
	{
		return 1;
	}
	return 0;
}	

int test_async(unsigned short idle_time) {
	/* To be completed ... */
}

int test_config(void) {
	if (mouse_subscribe_int(&hook_id) == -1)
		{
			return 1;
		}
	if (mouse_disable_stream_mode(NUM_TRIES))
	{
		return 1;
	}
	mouse_status_packet_t mouse_status_packet;
	if (mouse_get_status(NUM_TRIES, &mouse_status_packet))
	{
		return 1;
	}
	print_config(&mouse_status_packet);
	return 0;
}	

int test_gesture(short length, unsigned short tolerance) {
	unsigned hook_id = MOUSE_HOOK_BIT;
	if (mouse_subscribe_int(&hook_id) == -1)
	{
		return 1;
	}
	if (mouse_set_stream_mode(NUM_TRIES))
	{
		return 1;
	}
	if (mouse_enable_stream_mode(NUM_TRIES))
	{
		return 1;
	}
	unsigned char packet[MOUSE_PACKET_SIZE];
	int r, ipc_status;
	message msg;
	while(true)
	{
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			// Driver receive fail
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			if (_ENDPOINT_P(msg.m_source) == HARDWARE) /* hardware interrupt notification */
			{
				if (msg.NOTIFY_ARG & BIT(MOUSE_HOOK_BIT)) {
					int result = test_gesture_mouse_int_handler(length, tolerance);
					if (result == -1)
					{
						return 1;
					}
					else if (result == true)
					{
						break;
					}
				}
			}
		}
	}
	printf("Horizontal gesture detected. Exiting...\n");
	if (mouse_disable_stream_mode(NUM_TRIES))
	{
		return 1;
	}
	if (mouse_unsubscribe_int(hook_id))
	{
		return 1;
	}
	return 0;
}

bool check_horizontal_line(short length, unsigned short tolerance, event_t *event, int x_delta, int y_delta)
{
	static gesture_state_t gesture_state = GESTURE_INIT;
	static int horizontal_status = 0;
	static int vertical_status = 0;
	switch (gesture_state)
	{
	case GESTURE_INIT:
	{
		if (event->type == LDOWN)
		{
			gesture_state = GESTURE_DRAW;
		}
		break;
	}
	case GESTURE_DRAW:
	{
		switch (event->type)
		{
		case LUP:
		{
			horizontal_status = 0;
			gesture_state = GESTURE_INIT;
			break;
		}
		case MOVE:
		{
			vertical_status += abs(y_delta);
			if (sameSign(x_delta, length) && vertical_status <= tolerance)
			{
				horizontal_status += x_delta;
				if (abs(horizontal_status) > abs(length) && sameSign(horizontal_status, length))
				{
					// GESTURE DETECTED
					return true;
				}
			}
			else
			{
				// RESET
				horizontal_status = 0;
				vertical_status = 0;
			}
			break;
		}
		default:
			break;
		}
		break;
	}
	}
	return false;
}

static bool sameSign(int x, int y)
{
	return (x ^ y) > 0;	// Efficient way of determining whether two numbers have the same sign (works in all architectures)
}

static void print_packet_info(mouse_data_packet_t mouse_data_packet)
{
	printf("B1=0x%X\t", mouse_data_packet.bytes[0]);
	printf("B2=0x%X\t", mouse_data_packet.bytes[1]);
	printf("B3=0x%X\t", mouse_data_packet.bytes[2]);
	printf("LB=%d\t", mouse_data_packet.left_button);
	printf("MB=%d\t", mouse_data_packet.middle_button);
	printf("RB=%d\t", mouse_data_packet.right_button);
	printf("XOV=%d\t", mouse_data_packet.x_overflow);
	printf("YOV=%d\t", mouse_data_packet.y_overflow);
	printf("X=%d\t", mouse_data_packet.x_delta);
	printf("Y=%d", mouse_data_packet.y_delta);
	printf("\n");
	return;
}

void print_config(mouse_status_packet_t *mouse_status_packet_t)
{
	// TODO
	printf("0x%X\n", mouse_status_packet_t->bytes[0]);
	return;
}

static int test_packet_mouse_int_handler(unsigned short* cnt)
{
	if(mouse_int_handler(NUM_TRIES))
	{
		return 1;
	}
	mouse_data_packet_t mouse_data_packet;
	if(mouse_get_packet(&mouse_data_packet))
	{
		--*cnt;
		print_packet_info(mouse_data_packet);
	}
	return 0;
}

static int test_gesture_mouse_int_handler(short length, unsigned short tolerance)
{
	if(mouse_int_handler(NUM_TRIES))
	{
		return -1;
	}
	mouse_data_packet_t mouse_data_packet;
	if(mouse_get_packet(&mouse_data_packet))
	{
		event_t event;
		if (mouse_data_packet.left_button)
		{
			event.type = LDOWN;
			if (check_horizontal_line(length, tolerance, &event, mouse_data_packet.x_delta, mouse_data_packet.y_delta))
			{
				return true;
			}
		}
		else
		{
			event.type = LUP;
			if (check_horizontal_line(length, tolerance, &event, mouse_data_packet.x_delta, mouse_data_packet.y_delta))
			{
				return true;
			}
		}
		if (mouse_data_packet.x_delta != 0 || mouse_data_packet.y_delta != 0)
		{
			event.type = MOVE;
			if (check_horizontal_line(length, tolerance, &event, mouse_data_packet.x_delta, mouse_data_packet.y_delta))
			{
				return true;
			}
		}
	}
	return false;
}
