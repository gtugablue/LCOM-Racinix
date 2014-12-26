#include "race.h"
#include <string.h>
#include "sys/times.h"
#include "serial.h"
#include <stdbool.h>

#define PI 					3.14159265358979323846

static void race_update_vehicle(race_t *race, vehicle_t *vehicle, double delta_time);
static void race_show_info(race_t *race, unsigned fps);
static int race_serial_transmit(race_t *race);
static void race_sort_vehicles(race_t *race, unsigned char vehicle_IDs[]);
static void race_update_first(race_t *race);

race_t *race_create(track_t *track, unsigned num_players, bool serial_port, bitmap_t **vehicle_bitmaps, vehicle_keys_t *vehicle_keys, uint16_t *vehicle_colors, double freeze_time, unsigned num_laps, vbe_mode_info_t *vbe_mode_info, font_t *font)
{
	race_t *race;
	if ((race = malloc(sizeof(race_t))) == NULL)
	{
		return NULL;
	}
	if ((race->vehicles = malloc(num_players * sizeof(vehicle_t *))) == NULL)
	{
		race_delete(race);
		return NULL;
	}
	race->track = track;
	race->num_players = num_players;
	race->first = 0;
	race->serial_port = serial_port;
	race->vehicle_bitmaps = vehicle_bitmaps;
	race->vehicle_keys = vehicle_keys;
	race->vehicle_colors = vehicle_colors;
	race->time = -freeze_time;
	race->num_laps = num_laps;
	race->vbe_mode_info = vbe_mode_info;
	race->font = font;
	return race;
}

void race_set_serial_port_info(race_t *race, unsigned port_number, long seed)
{
	race->port_number = port_number;
	race->seed = seed;
}

int race_start(race_t *race)
{
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
		race->vehicles[i] = vehicle_create(VEHICLE_WIDTH, VEHICLE_LENGTH, &starting_position, heading, race->vehicle_bitmaps[i], race->vehicle_keys[i], race->vehicle_colors[i]);
	}
	return 0;
}

int race_tick(race_t *race, double delta_time, unsigned fps)
{
	vg_swap_mouse_buffer();
	vg_fill(RACINIX_COLOR_GRASS);
	track_draw(race->track);
	size_t i;

	// Draw checkpoints
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

	if (race->time >= 0)
	{
		if (race->vehicles[race->first]->current_lap >= race->num_laps)
		{
			// Race ended

			for (i = 0; i < race->num_players; ++i)
			{
				vehicle_draw(race->vehicles[i]);
			}

			char string[50];
			if (race->serial_port)
			{
				if (race->first == 0)
				{
					sprintf(string, "YOU WIN!");
				}
				else
				{
					sprintf(string, "YOU LOOSE!");
				}
			}
			else
			{
				sprintf(string, "PLAYER %d WINS!", race->first + 1);
			}
			font_show_string(race->font, string, FONT_BITMAP_HEIGHT, race->vbe_mode_info->XResolution / 2, (race->vbe_mode_info->YResolution - FONT_BITMAP_HEIGHT) / 2, FONT_ALIGNMENT_MIDDLE, VIDEO_GR_WHITE, 4);
		}
		else
		{
			// Update vehicles
			if (race->serial_port)
			{
				race_update_vehicle(race, race->vehicles[0], delta_time);
				vehicle_draw(race->vehicles[1]);
				if (race_serial_transmit(race))
				{
					return 1;
				}
			}
			else
			{
				for (i = 0; i < race->num_players; ++i)
				{
					race_update_vehicle(race, race->vehicles[i], delta_time);
				}
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

			race_update_first(race);
		}
	}
	else
	{
		for (i = 0; i < race->num_players; ++i)
		{
			vehicle_draw(race->vehicles[i]);
		}
	}

	race_show_info(race, fps);
	vg_swap_buffer();
	race->time += delta_time;

	return 0;
}

int race_serial_receive(race_t *race)
{
	if (race->serial_port && race->vehicles != NULL && race->vehicles[1] != NULL)
	{
		char *token;
		if ((token = strtok(NULL, RACE_SERIAL_PROTO_TOKEN)) == NULL)
		{
			return 1;
		}
		if (strcmp(token, RACE_SERIAL_PROTO_VEHICLE_INFO) == 0)
		{
			if ((token = strtok(NULL, RACE_SERIAL_PROTO_TOKEN)) == NULL)
			{
				return 1;
			}
			race->vehicles[1]->position.x = (double)strtoul(token, NULL, RACE_SERIAL_PROTO_BASE) / RACE_SERIAL_PROTO_FLOAT_MULTIPLIER;

			if ((token = strtok(NULL, RACE_SERIAL_PROTO_TOKEN)) == NULL)
			{
				return 1;
			}
			race->vehicles[1]->position.y = (double)strtoul(token, NULL, RACE_SERIAL_PROTO_BASE) / RACE_SERIAL_PROTO_FLOAT_MULTIPLIER;

			if ((token = strtok(NULL, RACE_SERIAL_PROTO_TOKEN)) == NULL)
			{
				return 1;
			}
			race->vehicles[1]->speed = (double)strtol(token, NULL, RACE_SERIAL_PROTO_BASE) / RACE_SERIAL_PROTO_FLOAT_MULTIPLIER;

			if ((token = strtok(NULL, RACE_SERIAL_PROTO_TOKEN)) == NULL)
			{
				return 1;
			}
			race->vehicles[1]->heading = (double)strtol(token, NULL, RACE_SERIAL_PROTO_BASE) / RACE_SERIAL_PROTO_FLOAT_MULTIPLIER;

			if ((token = strtok(NULL, RACE_SERIAL_PROTO_TOKEN)) == NULL)
			{
				return 1;
			}
			race->vehicles[1]->current_lap = strtoul(token, NULL, RACE_SERIAL_PROTO_BASE);

			if ((token = strtok(NULL, RACE_SERIAL_PROTO_TOKEN)) == NULL)
			{
				return 1;
			}
			race->vehicles[1]->current_checkpoint = strtoul(token, NULL, RACE_SERIAL_PROTO_BASE);
		}
		else
		{
			return 1;
		}
	}
	printf("derp\n");
	return 0;
}

void race_delete(race_t *race)
{
	size_t i;
	for (i = 0; i < race->num_players; ++i)
	{
		vehicle_delete(race->vehicles[i]);
		race->vehicles[i] = NULL;
	}
	free(race->vehicles);
	race->vehicles = NULL;
	free(race);
	race = NULL;
}

static void race_update_vehicle(race_t *race, vehicle_t *vehicle, double delta_time)
{
	double drag = VEHICLE_DRAG;
	size_t i;
	for(i = 0; i < VEHICLE_NUM_WHEELS; ++i)
	{
		drag += track_get_point_drag(race->track, (int)vehicle->wheels[i].x, (int)vehicle->wheels[i].y, race->vbe_mode_info->XResolution, race->vbe_mode_info->YResolution);
	}
	vehicle_tick(vehicle, race->track, race->vbe_mode_info, delta_time, drag);
}

static void race_show_info(race_t *race, unsigned fps)
{
	char string[100];
	sprintf(string, "FPS: %d", fps);
	font_show_string(race->font, string, 20, 11, race->vbe_mode_info->YResolution - 31, FONT_ALIGNMENT_LEFT, VIDEO_GR_WHITE, 2);
	//if (race->time < 0)
	//{
	//	sprintf(string, "%d", (int)abs((int)floor(race->time)));
	//	font_show_string(race->font, string, RACE_START_COUNTER_HEIGHT, race->vbe_mode_info->XResolution / 2, (race->vbe_mode_info->YResolution - RACE_START_COUNTER_HEIGHT) / 2, FONT_ALIGNMENT_MIDDLE, VIDEO_GR_WHITE, 4);
	//}
	if (race->time < RACE_START_TEXT_FADE_TIME)
	{
		if (race->time < 0)
		{
			sprintf(string, "%d", (int)abs((int)floor(race->time)));
			double height = RACE_START_COUNTER_HEIGHT * fmod(fabs(race->time), 1);
			font_show_string(race->font, string, height, race->vbe_mode_info->XResolution / 2, (race->vbe_mode_info->YResolution - height) / 2, FONT_ALIGNMENT_MIDDLE, VIDEO_GR_WHITE, 4);
		}
		else
		{
			double height = RACE_START_COUNTER_HEIGHT * fmod(race->time, 1) / RACE_START_TEXT_FADE_TIME;
			//double height = RACE_START_COUNTER_HEIGHT * (1 - fmod(race->time, 1) / RACE_START_TEXT_FADE_TIME);
			font_show_string(race->font, RACE_START_TEXT, height, race->vbe_mode_info->XResolution / 2, (race->vbe_mode_info->YResolution - height) / 2, FONT_ALIGNMENT_MIDDLE, VIDEO_GR_WHITE, 4);
		}
	}

	// Show scoreboard
	font_show_string(race->font, "CP", 15, race->vbe_mode_info->XResolution - 80, 10, FONT_ALIGNMENT_MIDDLE, VIDEO_GR_WHITE, 0);
	font_show_string(race->font, "LAP", 15, race->vbe_mode_info->XResolution - 30, 10, FONT_ALIGNMENT_MIDDLE, VIDEO_GR_WHITE, 0);

	unsigned char vehicle_IDs[race->num_players];
	race_sort_vehicles(race, vehicle_IDs);

	size_t i;
	for (i = 0; i < race->num_players; ++i)
	{
		unsigned y = 25 * i + 35;

		sprintf(string, "PLAYER %d:", vehicle_IDs[i] + 1);
		font_show_string(race->font, string, 15, race->vbe_mode_info->XResolution - 110, y, FONT_ALIGNMENT_RIGHT, race->vehicle_colors[vehicle_IDs[i]], 2);

		sprintf(string, "%d/%d", race->vehicles[vehicle_IDs[i]]->current_checkpoint, race->track->num_control_points);
		font_show_string(race->font, string, 15, race->vbe_mode_info->XResolution - 80, y, FONT_ALIGNMENT_MIDDLE, VIDEO_GR_WHITE, 0);

		sprintf(string, "%d/%d", race->vehicles[vehicle_IDs[i]]->current_lap + 1, race->num_laps);
		font_show_string(race->font, string, 15, race->vbe_mode_info->XResolution - 30, y, FONT_ALIGNMENT_MIDDLE, VIDEO_GR_WHITE, 0);
	}
}

static int race_serial_transmit(race_t *race)
{
	// VI <x_pos> <y_pos> <speed> <heading> <current_lap> <current_checkpoint>
	char *string;
	if (asprintf(&string, "%s %s %lu %lu %ld %ld %u %u",
			RACE_SERIAL_PROTO_RACE,
			RACE_SERIAL_PROTO_VEHICLE_INFO,
			(unsigned long)(race->vehicles[0]->position.x * RACE_SERIAL_PROTO_FLOAT_MULTIPLIER),
			(unsigned long)(race->vehicles[0]->position.y * RACE_SERIAL_PROTO_FLOAT_MULTIPLIER),
			(long)(race->vehicles[0]->speed * RACE_SERIAL_PROTO_FLOAT_MULTIPLIER),
			(long)(race->vehicles[0]->heading * RACE_SERIAL_PROTO_FLOAT_MULTIPLIER),
			race->vehicles[0]->current_lap,
			race->vehicles[0]->current_checkpoint
			) == -1)
	{
		free(string);
		return 1;
	}
	//printf("transmitting: %s\n", string);
	if (serial_interrupt_transmit_string(race->port_number, string))
	{
		return 1;
	}

	free(string);
	return 0;
}

static void race_sort_vehicles(race_t *race, unsigned char vehicle_IDs[])
{
	vehicle_IDs[0] = race->first;
	if (race->num_players == 2)
	{
		if (race->first == 0)
		{
			vehicle_IDs[1] = 1;
		}
		else
		{
			vehicle_IDs[1] = 0;
		}
	}
}

static void race_update_first(race_t *race)
{
	size_t i;
	for (i = 0; i < race->num_players; ++i)
	{
		if (race->vehicles[i]->current_lap * race->track->num_control_points + (race->vehicles[i]->current_checkpoint - 1) % race->track->num_control_points > race->vehicles[race->first]->current_lap * race->track->num_control_points + (race->vehicles[race->first]->current_checkpoint - 1) % race->track->num_control_points)
		{
			race->first = (race->first + 1) % race->num_players;
		}
	}
}
