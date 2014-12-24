#ifndef _MOUSE_H
#define _MOUSE_H

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

// Status Packet
#define MOUSE_STATUS_REMOTE_MODE_BIT	6
#define MOUSE_STATUS_ENABLED_BIT		5
#define MOUSE_STATUS_SCALING_2_1_BIT	4
#define MOUSE_STATUS_LEFT_BTN_BIT		2
#define MOUSE_STATUS_MIDDLE_BTN_BIT		1
#define MOUSE_STATUS_RIGHT_BTN_BIT		0

// Sample rates
#define MOUSE_SAMPLE_RATE_1				10
#define MOUSE_SAMPLE_RATE_2				20
#define MOUSE_SAMPLE_RATE_3				40
#define MOUSE_SAMPLE_RATE_4				60
#define MOUSE_SAMPLE_RATE_5				80
#define MOUSE_SAMPLE_RATE_6				100
#define MOUSE_SAMPLE_RATE_7				200

#define MOUSE_PACKET_SIZE				3
#define MOUSE_STATUS_SIZE				3

typedef struct
{
	unsigned char bytes[3];
	bool x_overflow;
	bool y_overflow;
	bool left_button;
	bool middle_button;
	bool right_button;
	int x_delta;
	int y_delta;
} mouse_data_packet_t;

typedef struct
{
	unsigned char bytes[3];
	bool remote_mode;
	bool enabled;
	bool scaling_2_1;
	bool left_button;
	bool middle_button;
	bool right_button;
	unsigned char resolution;
	unsigned char sample_rate;
} mouse_status_packet_t;

/** @defgroup mouse mouse
 * @{
 *
 * Functions for using the PS/2 mouse
 */

/**
 * @brief Subscribe the mouse interrupts
 *
 * Subscribes the mouse interrupts with a specified hook bit and hook id
 *
 * @param hook_id address of memory where the hook bit is located and where to write the hook id
 *
 * @return Return 0 upon success, -1 on "Not acknowledge" or >0 otherwise
 */
int mouse_subscribe_int(unsigned *hook_id);

/**
 * @brief Get packet
 *
 * Checks if there is a synchronized packet ready and retrieves it
 *
 * @param *mouse_data_packet where to write the packet, if it is ready
 *
 * @return True if there is a synchronized packet ready, false otherwise
 */
bool mouse_get_packet(mouse_data_packet_t *mouse_data_packet);

/**
 * @brief Get status packet
 *
 * @param num_tries number of tries to make whenever something fails
 * @param *mouse_status_packet where to write the packet, if it is ready
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int mouse_get_status(unsigned num_tries, mouse_status_packet_t *mouse_status_packet);

/**
 * @brief Write to the mouse
 *
 * Writes the command specified in the arguments directly to the mouse.
 *
 * @param num_tries number of tries to make whenever something fails
 * @param command command to send to the mouse
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int mouse_write(unsigned num_tries, unsigned char command);

/**
 * @brief Send argument to the mouse
 *
 * Sends an argument to the mouse (only 1 try).
 * A command should be sent before invoking this function.
 *
 * @param num_tries number of tries to make whenever something fails
 * @param argument argument to send to the mouse
 */
int mouse_send_argument(unsigned num_tries, unsigned char argument);

int mouse_read(unsigned num_tries, unsigned char* output);

int mouse_int_handler(unsigned num_tries);

int mouse_set_stream_mode(unsigned num_tries);

int mouse_enable_stream_mode(unsigned num_tries);

int mouse_disable_stream_mode(unsigned num_tries);

int mouse_reset(unsigned num_tries);

void mouse_discard_interrupts(unsigned num_tries, unsigned char hook_bit);

int mouse_unsubscribe_int(unsigned hook_id);

#endif
