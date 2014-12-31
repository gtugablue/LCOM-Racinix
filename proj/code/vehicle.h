#ifndef _VEHICLE_H
#define _VEHICLE_H

#include "vector2D.h"
#include <stdbool.h>
#include <stdlib.h>
#include "keyboard.h"
#include "math.h"
#include "video_gr.h"
#include "vbe.h"
#include "track.h"

#define VEHICLE_ACCELERATE							100.0
#define VEHICLE_REVERSE								VEHICLE_ACCELERATE / 2
#define VEHICLE_BREAK								VEHICLE_ACCELERATE
#define VEHICLE_STEER								0.2
#define VEHICLE_STOP_SPEED							10.0
#define VEHICLE_NUM_WHEELS							4
#define VEHICLE_MAX_STEER							0.6
#define VEHICLE_LIMITS_COLLISION_FRICTION			200.0
#define VEHICLE_VEHICLE_COLLISION_FRICTION			100
#define VEHICLE_DRAG								0.3
#define VEHICLE_NITROUS								500			///////mine
#define VEHICLE_VEHICLE_COLLISION_MOMENTUM_FACTOR	0.0010
#define VEHICLE_WIDTH								20
#define VEHICLE_LENGTH								38
#define VEHICLE_STEERING_VS_SPEED_FACTOR			1.8

//#define VEHICLE_DRAW_VELOCITY // Uncomment to show velocity vector

typedef struct
{
	int accelerate;
	int brake;
	int turn_left;
	int turn_right;
	int nitrous;
} vehicle_keys_t;

typedef struct
{
	double width;
	double length;
	vector2D_t position;
	double speed;
	double heading;
	double steering;
	vector2D_t back_axle;					// Stored for efficiency purposes
	vector2D_t front_axle;					// Stored for efficiency purposes
	vector2D_t wheels[VEHICLE_NUM_WHEELS]; 	// Stored for efficiency purposes
	vector2D_t oldPosition;					// Stored for efficiency purposes
	bitmap_t *bitmap;
	vehicle_keys_t vehicle_keys;
	unsigned current_lap;
	unsigned current_checkpoint;
	uint16_t checkpoint_color;
} vehicle_t;

typedef union
{
	struct
	{
		bool left : 1;
		bool right : 1;
		bool top : 1;
		bool bottom : 1;
	};
	uint8_t all;
} vehicle_limits_collision_t;

// Creates the vehicle and scales the bitmap according to its size (for efficiency purposes)
vehicle_t *vehicle_create(double width, double length, const vector2D_t *position, double heading, bitmap_t *bitmap, vehicle_keys_t vehicle_keys, uint16_t checkpoint_color);

void vehicle_tick(vehicle_t *vehicle, track_t *track, vbe_mode_info_t *vmi_p, double delta_time, double drag);

void vehicle_update_steering(vehicle_t *vehicle, double delta_time);

void vehicle_update_speed(vehicle_t *vehicle, double delta_time, double drag);

void vehicle_update_axles(vehicle_t *vehicle, double delta_time);

void vehicle_update_position(vehicle_t *vehicle);

void vehicle_update_heading(vehicle_t *vehicle);

void vehicle_calculate_axle_position(vehicle_t *vehicle);

/**
 * @brief Calculate the position of the 4 wheels
 *
 * Wheel order:
 * 0 - back left wheel
 * 1 - back right wheel
 * 2 - front left wheel
 * 3 - front right wheel
 *
 * @param vehicle vehicle info
 */
void vehicle_calculate_wheel_position(vehicle_t *vehicle);

vehicle_limits_collision_t vehicle_check_limits_collision(vehicle_t *vehicle, unsigned width, unsigned height);

void vehicle_limits_collision_handler(vehicle_t *vehicle, vector2D_t oldPosition, vehicle_limits_collision_t vehicle_limits_collision, unsigned width, unsigned height);

int vehicle_check_vehicle_collision(vehicle_t *vehicle, vehicle_t *vehicle2);

void vehicle_vehicle_collision_handler(vehicle_t *vehicle, unsigned wheel_ID, vehicle_t *vehicle2);

void vehicle_vehicle_collision_handler_position_fix(vehicle_t *vehicle, unsigned wheel_ID, vehicle_t *vehicle2);

bool vehicle_check_checkpoint_collision(vehicle_t *vehicle, track_t *track);

void vehicle_checkpoint_collision_handler(vehicle_t *vehicle, track_t *track);

int vehicle_draw(vehicle_t *vehicle);

void vehicle_delete(vehicle_t *vehicle);

enum
{
	VEHICLE_NO_COLLISION,
	VEHICLE_LEFT_COLLISION,
	VEHICLE_BOTTOM_COLLISION,
	VEHICLE_RIGHT_COLLISION,
	VEHICLE_TOP_COLLISION
};

#endif
