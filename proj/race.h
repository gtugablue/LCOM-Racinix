#ifndef _RACE_H
#define _RACE_H

#include "vehicle.h"
#include "track.h"
#include "video_gr.h"
#include "font.h"

typedef struct
{
	track_t *track;
	unsigned num_players;
	bitmap_t **vehicle_bitmaps;
	vehicle_keys_t *vehicle_keys;
	uint16_t *vehicle_colors;
	vehicle_t **vehicles;
	double time; // A negative value means the race is in freeze time and about to start.
	unsigned num_laps;
	vbe_mode_info_t *vbe_mode_info;
	font_t *font;
} race_t;

race_t *race_create(track_t *track, unsigned num_players, bitmap_t **vehicle_bitmaps, vehicle_keys_t *vehicle_keys, uint16_t *vehicle_colors, double freeze_time, unsigned num_laps, vbe_mode_info_t *vbe_mode_info, font_t *font);

int race_start(race_t *race);

int race_tick(race_t *race, double delta_time, unsigned fps);

void race_delete(race_t *race);

#endif
