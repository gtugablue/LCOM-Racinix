#ifndef _RACE_H
#define _RACE_H

#include "vehicle.h"
#include "track.h"

typedef struct
{
	track_t *track;
	unsigned num_vehicles;
	vehicle_t *vehicles[];
	double time; // A negative value means the race is in freeze time and about to start.
	unsigned num_laps;
} race_t;

race_t *race_create(track_t *track, vehicle_t *vehicles[], unsigned num_vehicles, double freeze_time, unsigned num_laps);

int race_start(race_t *race);

int race_tick(race_t *race, double delta_time);

void race_delete(race_t *race);

#endif
