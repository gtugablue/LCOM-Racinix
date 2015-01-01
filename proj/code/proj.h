#ifndef _PROJETO_H
#define _PROJETO_H

#include <stdlib.h>
#include <time.h>
#include <stdbool.h>		// Code gets much more beautiful with true/false instead of (!=0)/0
#include <minix/driver.h>
#include "math.h"
#include "video_gr.h"
#include "vbe.h"
#include "vector2D.h"
#include "keyboard.h"
#include "vehicle.h"
#include "timer.h"
#include "track.h"
#include "mouse.h"
#include <stdarg.h>
#include "bitmap.h"
#include "context_menu.h"
#include "font.h"
#include "race.h"
#include "serial.h"
#include "ad.h"
#include "rtc.h"

#define RACINIX_FOLDER_ROOT										"/home/lcom/proj/code/"
#define RACINIX_FOLDER_IMAGES									RACINIX_FOLDER_ROOT "images/"
#define RACINIX_FOLDER_FONTS									RACINIX_FOLDER_ROOT "fonts/"
#define RACINIX_FILE_ADS										RACINIX_FOLDER_ROOT "ads.txt"
#define RACINIX_FPS												60
#define RACINIX_DELTA_TIME										1.0 / TIMER_DEFAULT_FREQ
#define RACINIX_MOUSE_SENSITIVITY								1.7
#define RACINIX_VIDEO_MODE										VBE_MODE_GRAPHICS_1024_768_64K
#define RACINIX_MAIN_MENU_NUM_BTN								5
#define RACINIX_MAIN_MENU_CHAR_HEIGHT							30
#define RACINIX_MAIN_MENU_CHAR_WIDTH							10
#define RACINIX_MAIN_MENU_BTN_TEXT_SHADE						3
#define RACINIX_MAIN_MENU_BTN_TEXT_HOVER_OFFSET					2
#define RACINIX_TRACK_DESIGN_SELECT_POINT_RANGE					30
#define RACINIX_TRACK_DESIGN_MIN_POINTS							3
#define RACINIX_COLOR_MENU_BUTTONS								VIDEO_GR_BLUE
#define RACINIX_COLOR_CONTROL_POINT								VIDEO_GR_WHITE
#define RACINIX_RACE_FREEZE_TIME								5
#define RACINIX_RACE_NUM_LAPS									3
#define RACINIX_COLOR_ORANGE									rgb(255, 174, 0)
#define RACINIX_AD_MIN_DELAY_SECONDS							25
#define RACINIX_AD_MAX_DELAY_SECONDS							35

#define RACINIX_SERIAL_PORT_NUMBER								1
#define RACINIX_SERIAL_TRIGGER_LEVEL							2
#define RACINIX_SERIAL_NUM_BITS									8
#define RACINIX_SERIAL_NUM_STOP_BITS							2
#define RACINIX_SERIAL_PARITY									0
#define RACINIX_SERIAL_BAUD_RATE								115200
#define RACINIX_SERIAL_PROTO_BASE								RACE_SERIAL_PROTO_BASE
#define RACINIX_SERIAL_PROTO_FLOAT_MULTIPLIER					RACE_SERIAL_PROTO_FLOAT_MULTIPLIER
#define RACINIX_SERIAL_PROTO_TOKEN								RACE_SERIAL_PROTO_TOKEN
#define RACINIX_SERIAL_PROTO_RACE								RACE_SERIAL_PROTO_RACE
#define RACINIX_SERIAL_PROTO_NEW_RACE							"NEW_RACE" // NEW_RACE TI
#define RACINIX_SERIAL_PROTO_TRACK_INFO							"TI" // TI RND/MAN
#define RACINIX_SERIAL_PROTO_TRACK_RANDOM						"RND" // RND <seed>
#define RACINIX_SERIAL_PROTO_TRACK_MANUAL						"MNL" // MNL <num_points> <x1> <y1> <x2> <y2> ... <xn> <yn>
#define RACINIX_SERIAL_PROTO_END_RACE							"END_RACE" // END_RACE


/** @name States of the game */
/** @{ */
enum
{
	RACINIX_STATE_MAIN_MENU,
	RACINIX_STATE_DESIGN_TRACK,
	RACINIX_STATE_RACE,
	RACINIX_STATE_ERROR,
	RACINIX_STATE_END
};
/** @} end of click location info */

/** @name Main menu states */
/** @{ */
enum
{
	RACINIX_STATE_MAIN_MENU_BASE,
	RACINIX_STATE_MAIN_MENU_PICK_TRACK,
	RACINIX_STATE_MAIN_MENU_CREDITS
};
/** @} end of Main menu states */

/** @name States of the track design */
/** @{ */
enum
{
	RACINIX_STATE_TRACK_DESIGN_NEW,
	RACINIX_STATE_TRACK_DESIGN_DESIGNING,
	RACINIX_STATE_TRACK_DESIGN_MOVING
};
/** @} end of States of the track design */

/**
 * @brief Start the program
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int racinix_start();


/**
 * @brief Exit the program
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int racinix_exit();


/**
 * @brief Starts the race
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int racinix_start_race();

/**
 * @brief Dispatches the events
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int racinix_dispatcher();

/**
 * @brief Handle the event (that is given as argument)
 *
 * @param event event to be analyzed
 * @param var more_variables related to the event
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int racinix_event_handler(int event, ...);

/**
 * @brief Handle the main menu and everything that it involves
 *
 * @param event event to be analyzed
 * @param var_args variables related to the event
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int racinix_main_menu_event_handler(int event, va_list *var_args);

/**
 * @brief Handle the race and everything that it involves
 *
 * @param event event to be analyzed
 * @param var_args variables related to the event
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int racinix_race_event_handler(int event, va_list *var_args);

/**
 * @brief Handle the design of the track event
 *
 * @param event event to be analyzed
 * @param var_args variables related to the event
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int racinix_track_design_event_handler(int event, va_list *var_args);

/**
 * @brief Updates the vehicle on the race
 *
 * @param vehicle vehicle info
 */
void racinix_update_vehicle(vehicle_t *vehicle);

/**
 * @brief Handle the keyboard interrupts
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int racinix_keyboard_int_handler();

/**
 * @brief Handle the timer interrupts
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int racinix_timer_int_handler();

/**
 * @brief Handle the mouse interrupts
 *
 * @param mouse_data_packet
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int racinix_mouse_int_handler(mouse_data_packet_t *mouse_data_packet);

/**
 * @brief Handle the serial port interrupts
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int racinix_serial_int_handler();

/**
 * @brief Handle the RTC interrupts
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int racinix_rtc_int_handler();

/**
 * @brief Reprograms the alarm of the RTC
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int racinix_rtc_reprogram_alarm();

/**
 * @brief
 *
 * @param string
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int racinix_main_menu_serial_receive(char *string);

/**
 * @brief Update the mouse (on the screen)
 *
 * @param mouse_data_packet
 */
void racinix_mouse_update(mouse_data_packet_t *mouse_data_packet);

/**
 * @brief Draw the mouse
 */
void racinix_draw_mouse();

/**
 * @brief Draw the menu
 *
 * @param button_ID
 * @param buttons
 */
void racinix_draw_menu(size_t button_ID, const unsigned char *buttons[]);

/**
 * @brief Transmits the control points when it's in serial port
 *
 * @param track track info
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int racinix_serial_transmit_track_control_points(track_t *track);

/**
 * @brief
 *
 * @param buttons
 *
 * @return Return 0 upon success, non-zero otherwise
 */
unsigned racinix_main_menu_get_hovered_button(const unsigned char *buttons[]);

/**
 * @brief Read the ads (from RTC)
 *
 * @param file address to the file
 * @param ads address to an array of the ads
 *
 * @return Return 0 upon success, non-zero otherwise
 */
unsigned racinix_read_ads(const char *file, char *ads[]);

/**
 * @brief Generates the seed
 *
 * @return Return 0 upon success, non-zero otherwise
 */
unsigned long racinix_generate_seed();

/**
 * @brief
 *
 * @return Return 0 upon success, non-zero otherwise
 */
const unsigned char *racinix_get_root_folder();

// Events
/** @name Events */
/** @{ */
enum
{
	RACINIX_EVENT_KEYSTROKE, // int key, bool pressed
	RACINIX_EVENT_MOUSE_MOVEMENT, // mouse_data_packet_t *mouse_data_packet
	RACINIX_EVENT_MOUSE_LEFT_BTN, // bool pressed
	RACINIX_EVENT_MOUSE_RIGHT_BTN, // bool pressed
	RACINIX_EVENT_NEW_FRAME, // unsigned fps
	RACINIX_EVENT_SERIAL_RECEIVE // char *string
};
/** @} end of Events */


// Main menu buttons
/** @name Main menu buttons */
/** @{ */
enum
{
	RACINIX_MAIN_MENU_BUTTON_1_PLAYER,
	RACINIX_MAIN_MENU_BUTTON_2_PLAYERS_SAME_PC,
	RACINIX_MAIN_MENU_BUTTON_2_PLAYERS_SERIAL_PORT,
	RACINIX_MAIN_MENU_BUTTON_CREDITS,
	RACINIX_MAIN_MENU_BUTTON_EXIT
};
/** @} end of Main menu buttons */

#endif
