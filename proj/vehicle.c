#include "vehicle.h"
#define PI 3.14159265358979323846

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
	double steering = 0.0;
	if (kbd_keys[KEY_ARR_LEFT].pressed && !kbd_keys[KEY_ARR_RIGHT].pressed)
	{
		steering = -(VEHICLE_STEER_ANGLE * vehicle->length) / abs(vehicle->speed);
	}
	else if (kbd_keys[KEY_ARR_RIGHT].pressed && !kbd_keys[KEY_ARR_LEFT].pressed)
	{
		steering = (VEHICLE_STEER_ANGLE * vehicle->length) / abs(vehicle->speed);
	}
	vector2D_t old_position = vehicle->position;

	// Calculate axle positions
	vector2D_t back_axle, front_axle;
	vehicle_calculate_axle_position(vehicle, &back_axle, &front_axle);
	vector2D_t wheels[4];
	vehicle_calculate_wheel_position(vehicle, &back_axle, &front_axle, wheels);

	vg_fill(0x0);
	vg_draw_line(back_axle.x, back_axle.y, front_axle.x, front_axle.y, 0x4);
	vg_draw_circle(wheels[0].x, wheels[0].y, 2, 0x4);
	vg_draw_circle(wheels[1].x, wheels[1].y, 2, 0x4);
	vg_draw_circle(wheels[2].x, wheels[2].y, 2, 0x4);
	vg_draw_circle(wheels[3].x, wheels[3].y, 2, 0x4);

	// Move axles
	back_axle = vectorAdd(
			back_axle,
			vectorMultiply(
					vectorCreate(
							cos(vehicle->heading),
							sin(vehicle->heading)
					),
					vehicle->speed * delta_time
			)
	);
	front_axle = vectorAdd(
			front_axle,
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
					back_axle,
					front_axle
			),
			2
	);

	vehicle->heading = atan2(front_axle.y - back_axle.y, front_axle.x - back_axle.x);

	double change;
	vg_draw_line(0, 0, 500, 0, 0x33);
	if (kbd_keys[KEY_ARR_DOWN].pressed)
	{
		if (vehicle->speed > 0)
		{
			vehicle->speed -= VEHICLE_BREAK * delta_time;
		}
		else
		{
			vehicle->speed -= VEHICLE_REVERSE * delta_time;
		}
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

void vehicle_calculate_axle_position(vehicle_t *vehicle, vector2D_t *back_axle, vector2D_t *front_axle)
{
	*back_axle = vectorSubtract(
			vehicle->position,
			vectorMultiply(
					vectorCreate(
							cos(vehicle->heading),
							sin(vehicle->heading)
					),
					vehicle->length / 2
			)
	);
	*front_axle = vectorAdd(
			vehicle->position,
			vectorMultiply(
					vectorCreate(
							cos(vehicle->heading),
							sin(vehicle->heading)
					),
					vehicle->length / 2
			)
	);
}

void vehicle_calculate_wheel_position(vehicle_t *vehicle, vector2D_t *back_axle, vector2D_t *front_axle, vector2D_t wheels[])
{
	wheels[0] = vectorSubtract(
			*back_axle,
			vectorMultiply(
					vectorCreate(
							cos(vehicle->heading + PI / 2),
							sin(vehicle->heading + PI / 2)
					),
					vehicle->width / 2
			)
	);
	wheels[1] = vectorSubtract(
			*back_axle,
			vectorMultiply(
					vectorCreate(
							cos(vehicle->heading - PI / 2),
							sin(vehicle->heading - PI / 2)
					),
					vehicle->width / 2
			)
	);
	wheels[2] = vectorAdd(
			*front_axle,
			vectorMultiply(
					vectorCreate(
							cos(vehicle->heading + PI / 2),
							sin(vehicle->heading + PI / 2)
					),
					vehicle->width / 2
			)
	);
	wheels[3] = vectorAdd(
			*front_axle,
			vectorMultiply(
					vectorCreate(
							cos(vehicle->heading - PI / 2),
							sin(vehicle->heading - PI / 2)
					),
					vehicle->width / 2
			)
	);
}

void vehicle_delete(vehicle_t *vehicle)
{
	free(vehicle);
}
