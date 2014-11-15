#ifndef _VEHICLE_H
#define _VEHICLE_H

#include "vector2D.h"
#include <stdbool.h>
#include <stdlib.h>
#include "keyboard.h"
#include "math.h"

#define VEHICLE_ACCELERATE	50.0
#define VEHICLE_DRAG		50.0
#define VEHICLE_BREAK		200.0
#define VEHICLE_STEER_ANGLE	0.05

typedef struct
{
	double width;
	double length;
	vector2D_t position;
	double speed;
	double heading;
} vehicle_t;

vehicle_t *vehicle_create(double width, double length, const vector2D_t *position, double speed, double heading);

void vehicle_tick(vehicle_t *vehicle, double delta_time, double max_velocity);

void vehicle_delete(vehicle_t *vehicle);

#endif
