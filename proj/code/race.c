#include "race.h"
#include <string.h>
#include "sys/times.h"
#include "serial.h"
#include <stdbool.h>

#define PI 					3.14159265358979323846

static void race_update_vehicle(race_t *race, vehicle_t *vehicle, double delta_time);
static void race_show_countdown(race_t *race);
static void race_show_speedometers(race_t *race);
static void race_show_speedometer(race_t *race, vector2D_t location, double speed, uint16_t color);
static void race_show_info(race_t *race, unsigned fps);
static int race_serial_transmit(race_t *race);
static void race_sort_vehicles(race_t *race, unsigned char vehicle_IDs[]);
static void race_update_first(race_t *race);

race_t *race_create(track_t *track, unsigned num_players, bool serial_port, bitmap_t **vehicle_bitmaps, bitmap_t *bitmap_speedometer, vehicle_keys_t *vehicle_keys, uint16_t *vehicle_colors, double freeze_time, unsigned num_laps, vbe_mode_info_t *vbe_mode_info, font_t *font)
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
	race->state = RACE_STATE_FREEZETIME;
	race->track = track;
	race->num_players = num_players;
	race->first = 0;
	race->serial_port = serial_port;
	race->vehicle_bitmaps = vehicle_bitmaps;
	race->bitmap_speedometer = bitmap_speedometer;
	race->vehicle_keys = vehicle_keys;
	race->vehicle_colors = vehicle_colors;
	race->time = -freeze_time;
	race->num_laps = num_laps;
	race->vbe_mode_info = vbe_mode_info;
	race->font = font;
	race->host = true;
	return race;
}

void race_set_serial_port_info(race_t *race, unsigned port_number, long seed, bool host)
{
	race->port_number = port_number;
	race->seed = seed;
	race->host = host;
	if (host)
	{
		race->state = RACE_STATE_WAITING;
	}
	else
	{
		race->state = RACE_STATE_FREEZETIME;
	}
}

int race_start(race_t *race)
{
	vector2D_t starting_position_increment = vectorDivide(vectorSubtract(race->track->outside_spline[0], race->track->inside_spline[0]), race->num_players + 1);
	vector2D_t starting_position_offset, temp_vector;
	vector2D_t starting_position;
	double heading = atan2(race->track->spline[0].y - race->track->spline[race->track->spline_size - 1].y, race->track->spline[0].x - race->track->spline[race->track->spline_size - 1].x);
	temp_vector = vectorRotate(starting_position_increment, PI / 2);
	normalize(&temp_vector);
	size_t i, j;
	if (race->host)
	{
		j = 0;
	}
	else
	{
		j = 1;
	}
	for (i = 0, j = race->host ? 0 : 1; i < race->num_players; ++i, j = (j + 1) % race->num_players)
	{
		starting_position_offset = vectorMultiply(temp_vector, -VEHICLE_LENGTH / 2);
		starting_position = vectorAdd(vectorAdd(race->track->inside_spline[0], vectorMultiply(starting_position_increment, j + 1)), starting_position_offset);
		race->vehicles[i] = vehicle_create(VEHICLE_WIDTH, VEHICLE_LENGTH, &starting_position, heading, race->vehicle_bitmaps[i], race->vehicle_keys[i], race->vehicle_colors[i]);
	}
	return 0;
}

int race_tick(race_t *race, double delta_time, unsigned fps)
{
	vg_fill(RACINIX_COLOR_GRASS);
	track_draw(race->track);
	size_t i;
	unsigned char string[50];

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

	switch (race->state)
	{
	case RACE_STATE_WAITING:
	{
		if (race->time >= 0)
		{
			race->state = RACE_STATE_RACING;
		}
		for (i = 0; i < race->num_players; ++i)
		{
			vehicle_draw(race->vehicles[i]);
		}
		font_show_string(race->font, RACE_WAITING_TEXT, FONT_BITMAP_HEIGHT, race->vbe_mode_info->XResolution / 2, (race->vbe_mode_info->YResolution - FONT_BITMAP_HEIGHT) / 2, FONT_ALIGNMENT_MIDDLE, VIDEO_GR_WHITE, 4);
		break;
	}
	case RACE_STATE_FREEZETIME:
	{
		if (race->time >= 0)
		{
			race->state = RACE_STATE_RACING;
		}
		for (i = 0; i < race->num_players; ++i)
		{
			vehicle_draw(race->vehicles[i]);
		}
		race->time += delta_time;
		break;
	}
	case RACE_STATE_RACING:
	{
		if (race->vehicles[race->first]->current_lap >= race->num_laps)
		{
			// Race ended
			race->state = RACE_STATE_END;
		}
		else
		{
			race_show_countdown(race);
			// Update vehicles
			if (race->serial_port)
			{
				race_update_vehicle(race, race->vehicles[0], delta_time);
				vehicle_draw(race->vehicles[1]);
				if (race->host)
				{
					if (race_serial_transmit(race))
					{
						return 1;
					}
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

			race_show_speedometers(race);

			race_update_first(race);
		}
		race->time += delta_time;
		break;
	}
	case RACE_STATE_END:
	{
		for (i = 0; i < race->num_players; ++i)
		{
			vehicle_draw(race->vehicles[i]);
		}
		if (race->serial_port)
		{
			if (race->first == 0)
			{
				sprintf(string, "YOU WIN!");
			}
			else
			{
				sprintf(string, "YOU LOSE!");
			}
		}
		else
		{
			sprintf(string, "PLAYER %d WINS!", race->first + 1);
		}
		font_show_string(race->font, string, FONT_BITMAP_HEIGHT, race->vbe_mode_info->XResolution / 2, (race->vbe_mode_info->YResolution - FONT_BITMAP_HEIGHT) / 2, FONT_ALIGNMENT_MIDDLE, VIDEO_GR_WHITE, 4);
		break;
	}
	}

	if (race->state != RACE_STATE_WAITING)
	{
		race_show_countdown(race);
	}

	race_show_info(race, fps);
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
		if (strcmp(token, RACE_SERIAL_PROTO_VEHICLE_INFO) == 0) // VI
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

			if (!race->host)
			{
				if (race_serial_transmit(race))
				{
					return 1;
				}
			}
		}
		else if (strcmp(token, RACE_SERIAL_PROTO_READY) == 0) // READY
		{
			if (race->state == RACE_STATE_WAITING)
			{
				race->state = RACE_STATE_FREEZETIME;
			}
		}
		else
		{
			return 1;
		}
	}
	return 0;
}

int race_serial_transmit_ready_state(race_t *race)
{
	// RACE READY
	char *string;
	if (asprintf(&string, "%s %s",
			RACE_SERIAL_PROTO_RACE,
			RACE_SERIAL_PROTO_READY
	) == -1)
	{
		free(string);
		return 1;
	}
	if (serial_interrupt_transmit_string(race->port_number, string))
	{
		return 1;
	}

	free(string);
}

void race_delete(race_t *race)
{
	if (race != NULL)
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
#ifdef RACE_SHOW_FPS
	sprintf(string, "FPS: %d", fps);
	font_show_string(race->font, string, 20, 11, race->vbe_mode_info->YResolution - 31, FONT_ALIGNMENT_LEFT, VIDEO_GR_WHITE, 2);
#endif

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

static void race_show_countdown(race_t *race)
{
	if (race->time < RACE_START_TEXT_FADE_TIME)
	{
		if (race->time < 0)
		{
			unsigned char string[50];
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
}

static void race_show_speedometers(race_t *race)
{
	double y;
	if (race->time > 0.7)
	{
		y = race->vbe_mode_info->YResolution - 1;
	}
	else
	{
		y = race->vbe_mode_info->YResolution - 1 + race->bitmap_speedometer->bitmap_information_header.height * (1 - race->time / 0.7);
	}
	if (race->num_players == 2 && !race->serial_port)
	{
		race_show_speedometer(race, vectorCreate(race->vbe_mode_info->XResolution - 3 * race->bitmap_speedometer->bitmap_information_header.width / 2 - 2 * RACE_SPEEDOMETER_MARGIN, y), race->vehicles[0]->speed, race->vehicle_colors[0]);
		race_show_speedometer(race, vectorCreate(race->vbe_mode_info->XResolution - race->bitmap_speedometer->bitmap_information_header.width / 2 - RACE_SPEEDOMETER_MARGIN, y), race->vehicles[1]->speed, race->vehicle_colors[1]);
	}
	else
	{
		race_show_speedometer(race, vectorCreate(race->vbe_mode_info->XResolution - (race->bitmap_speedometer->bitmap_information_header.width + RACE_SPEEDOMETER_MARGIN), y), race->vehicles[0]->speed, race->vehicle_colors[0]);
	}
}

static void race_show_speedometer(race_t *race, vector2D_t location, double speed, uint16_t color)
{
	bitmap_draw_alpha(race->bitmap_speedometer, location.x - race->bitmap_speedometer->bitmap_information_header.width / 2, location.y - race->bitmap_speedometer->bitmap_information_header.height, VIDEO_GR_64K_TRANSPARENT);
	vector2D_t polygon[3];
	double angle = (fabs(speed) / 100 > PI) ? (PI) : (fabs(speed) / 100);
	vector2D_t center_radius = vectorRotate(vectorCreate(race->bitmap_speedometer->bitmap_information_header.height * 0.04, 0), PI / 2 + angle);
	polygon[0] = vectorAdd(location, vectorRotate(vectorMultiply(vectorCreate(-1, 0), race->bitmap_speedometer->bitmap_information_header.height * 0.8), angle));
	polygon[1] = vectorSubtract(location, center_radius);
	polygon[2] = vectorAdd(location, center_radius);
	vg_draw_polygon(polygon, 3, color);
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
