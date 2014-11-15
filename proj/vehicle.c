#include "vehicle.h"

vehicle_t *vehicle_create(double width, double length, const vector2D_t *position, double speed, double heading)
{
	vehicle_t *vehicle = malloc(sizeof(vehicle_t));
	if (vehicle == NULL)
	{
		return NULL;
	}
	vehicle->width = width;
	vehicle->length = length;
	vehicle->position = *position;
	vehicle->speed = speed;
	vehicle->heading = heading;
	return vehicle;
}

void vehicle_tick(vehicle_t *vehicle, double delta_time, double max_velocity)
{
	// TODO
	double steering;
	if (kbd_keys[KEY_ARR_LEFT].pressed && !kbd_keys[KEY_ARR_RIGHT].pressed)
	{
		steering = -VEHICLE_STEER_ANGLE;
	}
	else if (kbd_keys[KEY_ARR_RIGHT].pressed && !kbd_keys[KEY_ARR_LEFT].pressed)
	{
		steering = VEHICLE_STEER_ANGLE;
	}
	else
	{
		steering = 0.0;
	}
	vector2D_t old_position = vehicle->position;

	// Find wheel positions;
	vector2D_t back_wheels = vectorSubtract(
			vehicle->position,
			vectorMultiply(
					vectorCreate(
							cos(vehicle->heading),
							sin(vehicle->heading)
					),
					vehicle->length / 2
			)
	);
	vector2D_t front_wheels = vectorAdd(
			vehicle->position,
			vectorMultiply(
					vectorCreate(
							cos(vehicle->heading),
							sin(vehicle->heading)
					),
					vehicle->length / 2
			)
	);

	// Move wheels
	back_wheels = vectorAdd(
			back_wheels,
			vectorMultiply(
					vectorCreate(
							cos(vehicle->heading),
							sin(vehicle->heading)
					),
					vehicle->speed * delta_time
			)
	);
	front_wheels = vectorAdd(
			back_wheels,
			vectorMultiply(
					vectorCreate(
							cos(vehicle->heading + steering),
							sin(vehicle->heading + steering)
					),
					vehicle->speed * delta_time
			)
	);

	// Move vehicle
	vehicle->position = vectorDivide(
			vectorAdd(
					back_wheels,
					front_wheels
			),
			2
	);
	vehicle->heading = atan2(front_wheels.y - back_wheels.y, front_wheels.x - back_wheels.x);

	double change;
	if (kbd_keys[KEY_ARR_DOWN].pressed)
	{
		change = -VEHICLE_BREAK * delta_time;
	}
	else if (kbd_keys[KEY_ARR_UP].pressed)
	{
		change = VEHICLE_ACCELERATE * delta_time;
	}
	else
	{
		change = -VEHICLE_DRAG * delta_time;
	}
	if (vehicle->speed + change > 0)
	{
		vehicle->speed += change;
	}
	else
	{
		vehicle->speed = 0;
	}
}

void vehicle_delete(vehicle_t *vehicle)
{
	free(vehicle);
}
