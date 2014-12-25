#ifndef _RACE_H
#define _RACE_H

#include "vehicle.h"
#include "track.h"
#include "video_gr.h"
#include "font.h"

#define RACE_START_COUNTER_HEIGHT				120
#define RACE_START_TEXT							"GO!"
#define RACE_START_TEXT_FADE_TIME				0.6

#define RACE_SERIAL_PROTO_BASE					10
#define RACE_SERIAL_PROTO_FLOAT_MULTIPLIER		1000
#define RACE_SERIAL_PROTO_TOKEN					" "
#define RACE_SERIAL_PROTO_RACE					"RACE"
#define RACE_SERIAL_PROTO_VEHICLE_INFO			"VI" // VI <x_pos> <y_pos> <speed> <heading> <current_lap> <current_checkpoint>

typedef struct
{
	track_t *track;
	unsigned num_players;
	unsigned char first; // Player in the lead

	// Serial port
	bool serial_port;
	unsigned port_number;
	unsigned long seed;

	bitmap_t **vehicle_bitmaps;
	vehicle_keys_t *vehicle_keys;
	uint16_t *vehicle_colors;
	vehicle_t **vehicles;
	double time; // A negative value means the race is in freeze time and about to start.
	unsigned num_laps;
	vbe_mode_info_t *vbe_mode_info;
	font_t *font;
} race_t;

race_t *race_create(track_t *track, unsigned num_players, bool serial_port, bitmap_t **vehicle_bitmaps, vehicle_keys_t *vehicle_keys, uint16_t *vehicle_colors, double freeze_time, unsigned num_laps, vbe_mode_info_t *vbe_mode_info, font_t *font);

void race_set_serial_port_info(race_t *race, unsigned port_number, unsigned long seed);

int race_start(race_t *race);

int race_tick(race_t *race, double delta_time, unsigned fps);

int race_serial_receive(race_t *race);

void race_delete(race_t *race);

#endif