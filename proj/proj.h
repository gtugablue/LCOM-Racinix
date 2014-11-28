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

#define FPS											30
#define INTERP_PERIOD								0.07f
#define RACINIX_MOUSE_SENSITIVITY					1.7
#define RACINIX_VIDEO_MODE							VBE_MODE_GRAPHICS_1024_768_64K
#define RACINIX_MAIN_MENU_NUM_BTN					6
#define RACINIX_MAIN_MENU_CHAR_HEIGHT				30
#define RACINIX_MAIN_MENU_CHAR_WIDTH				10

#define RACINIX_COLOR_MENU_BUTTONS					VIDEO_GR_BLUE
#define RACINIX_COLOR_GRASS							rgb(0, 90, 0)

// States
enum
{
	RACINIX_STATE_MAIN_MENU,
	RACINIX_STATE_PICK_TRACK,
	RACINIX_STATE_DESIGN_TRACK,
	RACINIX_STATE_RACE,
	RACINIX_STATE_END
};

// Race states
enum
{
	RACINIX_STATE_RACE_RACING_1_PLAYER,
	RACINIX_STATE_RACE_RACING_2_PLAYERS_SAME_PC,
	RACINIX_STATE_RACE_RACING_2_PLAYERS_SERIAL_PORT
};

int racinix_start();

int racinix_exit();

int racinix_start_race();

int racinix_dispatcher();

int racinix_event_handler(int event, ...);

int racinix_main_menu_event_handler(int event, va_list *var_args);

int racinix_race_event_handler(int event, va_list *var_args);

void racinix_update_vehicle(vehicle_t *vehicle);

int racinix_keyboard_int_handler();

int racinix_timer_int_handler();

int racinix_mouse_int_handler(mouse_data_packet_t *mouse_data_packet);

void racinix_mouse_update(mouse_data_packet_t *mouse_data_packet);

void racinix_draw_mouse();

// Events
enum asda
{
	RACINIX_EVENT_KEYSTROKE, // int key, bool pressed
	RACINIX_EVENT_MOUSE_MOVEMENT, // mouse_data_packet_t *mouse_data_packet
	RACINIX_EVENT_MOUSE_LEFT_BTN, // bool pressed
	RACINIX_EVENT_NEW_FRAME,
	RACINIX_EVENT_NEW_RACE // unsigned num_players, bool serial_port
};

enum
{
	RACINIX_MAIN_MENU_BUTTON_1_PLAYER,
	RACINIX_MAIN_MENU_BUTTON_2_PLAYERS_SAME_PC,
	RACINIX_MAIN_MENU_BUTTON_2_PLAYERS_SERIAL_PORT,
	RACINIX_MAIN_MENU_BUTTON_SETTINGS,
	RACINIX_MAIN_MENU_BUTTON_CREDITS,
	RACINIX_MAIN_MENU_BUTTON_EXIT
};

#endif
