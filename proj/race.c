#include "race.h"

race_t *race_create(track_t *track, vehicle_t *vehicles[], unsigned num_vehicles, double freeze_time, unsigned num_laps)
{
	race_t *race;
	if ((race = malloc(sizeof(race_t))) == NULL)
	{
		return NULL
	}

	race->track = track;
	race->num_vehicles = num_vehicles;
	race->vehicles = vehicles;
	race->time = -freeze_time;
	race->num_laps = num_laps;
}

int race_start(race_t *race)
{

}

int race_tick(race_t *race, double delta_time)
{

}

void race_delete(race_t *race)
{
	delete(rate);
}
