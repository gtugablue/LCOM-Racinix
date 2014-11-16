#ifndef _VEHICLE_H
#define _VEHICLE_H

#include "vector2D.h"
#include <stdbool.h>
#include <stdlib.h>
#include "keyboard.h"
#include "math.h"
#include "video_gr.h"

#define VEHICLE_ACCELERATE	150.0
#define VEHICLE_REVERSE		50.0
#define VEHICLE_DRAG		0.5
#define VEHICLE_BREAK		200.0
#define VEHICLE_STEER_ANGLE	2.5
#define VEHICLE_STOP_SPEED	10.0

typedef struct
{
	double width;
	double length;
	vector2D_t position;
	double speed;
	double heading;
} vehicle_t;

vehicle_t *vehicle_create(double width, double length, const vector2D_t *position, double heading);

void vehicle_tick(vehicle_t *vehicle, double delta_time, double max_velocity);

void vehicle_calculate_axle_position(vehicle_t *vehicle, vector2D_t *back_axle, vector2D_t *front_axle);

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
 * @param back_axle position of the center of the back axle
 * @param front_axle position of the center of the front axle
 * @param wheels[4] address of memory where to write the wheel positions
 */
void vehicle_calculate_wheel_position(vehicle_t *vehicle, vector2D_t *back_axle, vector2D_t *front_axle, vector2D_t wheels[]);

void vehicle_delete(vehicle_t *vehicle);

#endif
