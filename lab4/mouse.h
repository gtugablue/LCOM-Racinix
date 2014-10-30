#include "kbc.h"
#include <stdbool.h>

// Mouse Commands
#define MOUSE_RESET						0xFF
#define MOUSE_RESEND					0xFE
#define MOUSE_SET_DEFAULT				0xF6
#define MOUSE_DISABLE_STREAM_MODE		0xF5
#define MOUSE_ENABLE_DATA_PACKETS		0xF4
#define MOUSE_SET_SAMPLE_RATE			0xF3
#define MOUSE_SET_REMOTE_MODE			0xF0
#define MOUSE_READ_DATA					0xEB
#define MOUSE_SET_STREAM_MODE			0xEA
#define MOUSE_STATUS_REQUEST			0xE9
#define MOUSE_SET_RESOLUTION			0xE8
#define MOUSE_SET_SCALING_2_1			0xE7
#define MOUSE_SET_SCALING_1_1			0xE6

// Mouse responses
#define MOUSE_RESPONSE_ACK				0xFA	// Everything OK
#define MOUSE_RESPONSE_NACK				0xFE	// Invalid byte
#define MOUSE_RESPONSE_ERROR			0xFC	// Second consecutive invalid byte

// Data Packet
#define MOUSE_1ST_BYTE_Y_OF_BIT			7
#define MOUSE_1ST_BYTE_X_OF_BIT			6
#define MOUSE_1ST_BYTE_Y_SIGN_BIT		5
#define MOUSE_1ST_BYTE_X_SIGN_BIT		4
#define MOUSE_1ST_BYTE_ALWAYS_1_BIT		3
#define MOUSE_1ST_BYTE_MIDDLE_BTN_BIT	2
#define MOUSE_1ST_BYTE_RIGHT_BTN_BIT	1
#define MOUSE_1ST_BYTE_LEFT_BTN_BIT		0

#define MOUSE_PACKET_SIZE				3

#define MOUSE_PACKET_COUNTER(cnt, sign)	(((0 - (sign)) << 8) | (cnt))

int mouse_subscribe_int(unsigned *hook_id);

bool mouse_get_packet(unsigned char return_packet[]);

int mouse_write(unsigned num_tries, unsigned char command);

//int mouse_send_argument(unsigned num_tries, unsigned char argument);

int mouse_write_and_argument(unsigned num_tries, unsigned char command, unsigned char argument);
// TODO ^

int mouse_int_handler(unsigned num_tries);

int mouse_set_stream_mode(unsigned num_tries);

int mouse_enable_stream_mode(unsigned num_tries);

int mouse_disable_stream_mode(unsigned num_tries);

int mouse_reset(unsigned num_tries);

int mouse_unsubscribe_int(unsigned hook_id);
