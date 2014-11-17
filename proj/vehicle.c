#include "vehicle.h"
#define PI 					3.14159265358979323846

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
	vehicle->steering;
	return vehicle;
}

void vehicle_tick(vehicle_t *vehicle, double delta_time, double drag)
{
	vehicle_update_steering(vehicle, delta_time);
	vector2D_t old_position = vehicle->position;

	// Calculate axle positions
	vector2D_t back_axle, front_axle;
	vehicle_calculate_axle_position(vehicle, &back_axle, &front_axle);
	vector2D_t wheels[VEHICLE_NUM_WHEELS];
	vehicle_calculate_wheel_position(vehicle, &back_axle, &front_axle, wheels);

	size_t i;
	for (i = 0; i < VEHICLE_NUM_WHEELS; ++i)
	{
		vg_draw_circle(wheels[i].x, wheels[i].y, 2, 0x0);
	}
	for (i = 0; i < VEHICLE_NUM_WHEELS; ++i)
	{
		vg_draw_line(wheels[i % VEHICLE_NUM_WHEELS].x, wheels[i % VEHICLE_NUM_WHEELS].y, wheels[(i + 1) % VEHICLE_NUM_WHEELS].x, wheels[(i + 1) % VEHICLE_NUM_WHEELS].y, 0x4);
	}
	for (i = 0; i < VEHICLE_NUM_WHEELS / 2; ++i)
	{
		vg_draw_line(wheels[i % VEHICLE_NUM_WHEELS].x, wheels[i % VEHICLE_NUM_WHEELS].y, wheels[(i + 2) % VEHICLE_NUM_WHEELS].x, wheels[(i + 2) % VEHICLE_NUM_WHEELS].y, 0x4);
	}

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
							cos(vehicle->heading + vehicle->steering),
							sin(vehicle->heading + vehicle->steering)
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

	vehicle_update_speed(vehicle, delta_time, drag);
}

void vehicle_update_steering(vehicle_t *vehicle, double delta_time)
{
	if ((kbd_keys[KEY_ARR_LEFT].pressed || kbd_keys[KEY_A].pressed) && !(kbd_keys[KEY_ARR_RIGHT].pressed || kbd_keys[KEY_D].pressed))
	{
		vehicle->steering += -(VEHICLE_STEER * vehicle->length) / (2 * abs(vehicle->speed));
	}
	else if ((kbd_keys[KEY_ARR_RIGHT].pressed || kbd_keys[KEY_D].pressed) && !(kbd_keys[KEY_ARR_LEFT].pressed || kbd_keys[KEY_A].pressed))
	{
		vehicle->steering += (VEHICLE_STEER * vehicle->length) / (2 * abs(vehicle->speed));
	}
	else
	{
		vehicle->steering = 0.0;
	}
	if (vehicle->steering > VEHICLE_MAX_STEER)
	{
		vehicle->steering = (VEHICLE_MAX_STEER * vehicle->length) / abs(vehicle->speed);
	}
	else if (vehicle->steering < -VEHICLE_MAX_STEER)
	{
		vehicle->steering = -(VEHICLE_MAX_STEER * vehicle->length) / abs(vehicle->speed);
	}
}

void vehicle_update_speed(vehicle_t *vehicle, double delta_time, double drag)
{
	if (kbd_keys[KEY_ARR_DOWN].pressed || kbd_keys[KEY_S].pressed)
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
	else if (kbd_keys[KEY_ARR_UP].pressed || kbd_keys[KEY_W].pressed)
	{
		vehicle->speed += VEHICLE_ACCELERATE * delta_time;
	}
	else if (abs(vehicle->speed) < VEHICLE_STOP_SPEED)
	{
		vehicle->speed = 0;
	}

	if (vehicle->speed > drag * delta_time)
	{
		vehicle->speed -= drag * delta_time * vehicle->speed;
	}
	else if (vehicle->speed < -drag * delta_time)
	{
		vehicle->speed -= drag * delta_time * vehicle->speed;
	}
	else
	{
		vehicle->speed = 0;
	}
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
