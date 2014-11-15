#include "vehicle.h"

vehicle_t *vehicle_create(double width, double length, const vector2D_t *position, double heading)
{
	vehicle_t *vehicle = malloc(sizeof(vehicle_t));
	if (vehicle == NULL)
	{
		return NULL;
	}
	vehicle->width = width;
	vehicle->length = length;
	vehicle->position = *position;
	vehicle->speed = 0.0;
	vehicle->heading = heading;
	return vehicle;
}

void vehicle_tick(vehicle_t *vehicle, double delta_time, double max_velocity)
{
	// TODO
	double steering;
	if (kbd_keys[KEY_ARR_LEFT].pressed && !kbd_keys[KEY_ARR_RIGHT].pressed)
	{
		steering = -VEHICLE_STEER_ANGLE / vehicle->speed;
		if (vehicle->speed < 0)
		{
			steering = -steering;
		}
	}
	else if (kbd_keys[KEY_ARR_RIGHT].pressed && !kbd_keys[KEY_ARR_LEFT].pressed)
	{
		steering = VEHICLE_STEER_ANGLE / vehicle->speed;
		if (vehicle->speed < 0)
		{
			steering = -steering;
		}
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
	vg_fill(0x02);
	vg_draw_circle(front_wheels.x, front_wheels.y, 3, 0x4);
	vg_draw_circle(back_wheels.x, back_wheels.y, 3, 0x0);

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
			front_wheels,
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
	vg_draw_line(0, 0, 500, 0, 0x33);
	if (kbd_keys[KEY_ARR_DOWN].pressed)
	{
		vehicle->speed -= VEHICLE_BREAK * delta_time;
	}
	else if (kbd_keys[KEY_ARR_UP].pressed)
	{
		vehicle->speed += VEHICLE_ACCELERATE * delta_time;
	}
	else if (abs(vehicle->speed) < VEHICLE_STOP_SPEED)
	{
		vehicle->speed = 0;
	}

	if (vehicle->speed > VEHICLE_DRAG * delta_time)
	{
		vehicle->speed -= VEHICLE_DRAG * delta_time * vehicle->speed;
	}
	else if (vehicle->speed < -VEHICLE_DRAG * delta_time)
	{
		vehicle->speed -= VEHICLE_DRAG * delta_time * vehicle->speed;
	}
	else
	{
		vehicle->speed = 0;
	}
	vg_draw_line(0, 0, vehicle->speed, 0, 0x00);
}

void vehicle_delete(vehicle_t *vehicle)
{
	free(vehicle);
}
