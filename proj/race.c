#include "race.h"

#define PI 					3.14159265358979323846

race_t *race_create(track_t *track, unsigned num_players, vehicle_t *vehicles[], double freeze_time, unsigned num_laps)
{
	race_t *race;
	if ((race = malloc(sizeof(race_t))) == NULL)
	{
		return NULL;
	}

	race->track = track;
	race->num_players = num_players;
	race->vehicles = vehicles;
	race->time = -freeze_time;
	race->num_laps = num_laps;
}

int race_start(race_t *race)
{
	if ((race->vehicles = malloc(race->num_players * sizeof(vehicle_t *))) == NULL)
	{
		return 1;
	}

	vector2D_t starting_position_increment = vectorDivide(vectorSubtract(race->track->outside_spline[0], race->track->inside_spline[0]), race->num_players + 1);
	vector2D_t starting_position_offset, temp_vector;
	vector2D_t starting_position;
	double heading = atan2(race->track->spline[0].y - race->track->spline[race->track->spline_size - 1].y, race->track->spline[0].x - race->track->spline[race->track->spline_size - 1].x);


	temp_vector = vectorRotate(starting_position_increment, PI / 2);
	normalize(&temp_vector);

	size_t i;
	for (i = 0; i < race->num_players; ++i)
	{
		starting_position_offset = vectorMultiply(temp_vector, -VEHICLE_LENGTH / 2);
		starting_position = vectorAdd(vectorAdd(race->track->inside_spline[0], vectorMultiply(starting_position_increment, i + 1)), starting_position_offset);
	}
}

int race_tick(race_t *race, double delta_time)
{
	vg_swap_mouse_buffer();
	vg_fill(RACINIX_COLOR_GRASS);
	track_draw(race->track);
	size_t i;
	if (race->num_players == 2 && race->vehicles[0]->current_checkpoint == race->vehicles[1]->current_checkpoint) // Same checkpoint
	{
		vg_draw_circle(race->track->control_points[race->vehicles[0]->current_checkpoint].x, race->track->control_points[race->vehicles[1]->current_checkpoint].y, 5, race->vehicles[0]->checkpoint_color | race->vehicles[1]->checkpoint_color);
	}
	else
	{
		for (i = 0; i < race->num_players; ++i)
		{
			vg_draw_circle(race->track->control_points[race->vehicles[i]->current_checkpoint].x, race->track->control_points[race->vehicles[i]->current_checkpoint].y, 5, race->vehicles[i]->checkpoint_color);
		}
	}
	for (i = 0; i < race->num_players; ++i)
	{
		racinix_update_vehicle(race->vehicles[i]);
	}

	// Vehicle-vehicle collision
	unsigned wheel_ID;
	size_t j;
	for (i = 0; i < race->num_players; ++i)
	{
		for (j = 0; j < race->num_players; ++j)
		{
			if (i != j)
			{
				wheel_ID = vehicle_check_vehicle_collision(race->vehicles[i], race->vehicles[j]);
				if (wheel_ID != -1)
				{
					vehicle_vehicle_collision_handler(race->vehicles[i], wheel_ID, race->vehicles[j]);
				}
			}
		}
	}
	vg_swap_buffer();
}

void race_delete(race_t *race)
{
	free(race);
}
