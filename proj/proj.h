#ifndef _PROJETO_H
#define _PROJETO_H

#include <stdlib.h>
#include <time.h>
#include <stdbool.h>		// Code gets much more beautiful with true/false instead of (!=0)/0
#include <minix/drivers.h>
#include "math.h"
#include "video_gr.h"
#include "vbe.h"
#include "vector2D.h"
#include "keyboard.h"
#include "vehicle.h"
#include "timer.h"
#include "track.h"
#include "mouse.h"
#include "pixmap.h"
#include "xpm.h"
#include <stdarg.h>

#define FPS											29.97
#define INTERP_PERIOD								0.07f
#define RACINIX_MAIN_MENU_NUM_BTN					5
#define RACINIX_MAIN_MENU_BTN_HEIGHT				30
#define RACINIX_MAIN_MENU_CHAR_WIDTH				5

// States
enum
{
	RACINIX_STATE_MAIN_MENU,
	RACINIX_STATE_PICK_TRACK,
	RACINIX_STATE_DESIGN_TRACK,
	RACINIX_STATE_RACE_FREEZE_TIME,
	RACINIX_STATE_RACE,
	RACINIX_STATE_RACE_END,
	RACINIX_STATE_END
};

int racinix_start();

int racinix_exit();

int racinix_start_race();

int racinix_dispatcher();

int racinix_event_handler(int event, ...);

int racinix_main_menu_event_handler(va_list var_args);

int racinix_keyboard_int_handler();

int racinix_timer_int_handler(vbe_mode_info_t *vmi, track_t *track, vehicle_t *vehicle1, vehicle_t *vehicle2);

int racinix_mouse_int_handler();

void draw_mouse(unsigned width, unsigned height);

int orientation(vector2D_t p, vector2D_t q, vector2D_t r);

int convexHull(vector2D_t points[], unsigned n, vector2D_t hull[]);

bool isLeft( vector2D_t P0, vector2D_t P1, vector2D_t P2);

void swapPoints(vector2D_t *a, int i, int j);

int partition(vector2D_t *a, int left, int right, int pivot);

void quickSort(vector2D_t *a, int left, int right);

// Events
enum
{
	RACINIX_EVENT_KEYSTROKE, // int key, bool pressed
	RACINIX_EVENT_MOUSE_MOVEMENT,
	RACINIX_EVENT_MOUSE_LEFT_BTN, // bool pressed
	RACINIX_EVENT_TIMER
};

#endif
