#include "vehicle.h"
#include "xpm.h"
#include "pixmap.h"

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
	vehicle->steering = 0.0;
	vehicle_calculate_axle_position(vehicle);
	vehicle_calculate_wheel_position(vehicle);
	return vehicle;
}

void vehicle_tick(vehicle_t *vehicle, vbe_mode_info_t *vmi_p, double delta_time, double drag, vehicle_keys_t vehicle_keys)
{
	vehicle_update_steering(vehicle, delta_time, vehicle_keys);

	vehicle_calculate_axle_position(vehicle);

	vehicle_update_axles(vehicle, delta_time);

	vehicle_update_position(vehicle);

	vehicle_update_heading(vehicle);

	vehicle_update_speed(vehicle, delta_time, drag, vehicle_keys);

	vehicle_calculate_wheel_position(vehicle);

	vehicle_limits_collision_handler(vehicle, vehicle->oldPosition, vehicle_check_limits_collision(vehicle, vmi_p->XResolution, vmi_p->YResolution), vmi_p->XResolution, vmi_p->YResolution);

	vehicle_draw(vehicle, vmi_p);
}

void vehicle_update_steering(vehicle_t *vehicle, double delta_time, vehicle_keys_t vehicle_keys)
{
	if (vehicle_keys.turn_left && !vehicle_keys.turn_right)
	{
		vehicle->steering += -(VEHICLE_STEER * vehicle->length) / (2 * abs(vehicle->speed));
	}
	else if (vehicle_keys.turn_right && !vehicle_keys.turn_left)
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

void vehicle_update_speed(vehicle_t *vehicle, double delta_time, double drag, vehicle_keys_t vehicle_keys)
{
	if (vehicle_keys.brake)
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
	else if (vehicle_keys.accelerate)
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

void vehicle_update_axles(vehicle_t *vehicle, double delta_time)
{
	vehicle->back_axle = vectorAdd(
			vehicle->back_axle,
			vectorMultiply(
					vectorCreate(
							cos(vehicle->heading),
							sin(vehicle->heading)
					),
					vehicle->speed * delta_time
			)
	);
	vehicle->front_axle = vectorAdd(
			vehicle->front_axle,
			vectorMultiply(
					vectorCreate(
							cos(vehicle->heading + vehicle->steering),
							sin(vehicle->heading + vehicle->steering)
					),
					vehicle->speed * delta_time
			)
	);
}

void vehicle_update_position(vehicle_t *vehicle)
{
	vehicle->oldPosition = vehicle->position;
	vehicle->position = vectorDivide(
			vectorAdd(
					vehicle->back_axle,
					vehicle->front_axle
			),
			2
	);
}

void vehicle_update_heading(vehicle_t *vehicle)
{
	vehicle->heading = atan2(vehicle->front_axle.y - vehicle->back_axle.y, vehicle->front_axle.x - vehicle->back_axle.x);
}

void vehicle_calculate_axle_position(vehicle_t *vehicle)
{
	vehicle->back_axle = vectorSubtract(
			vehicle->position,
			vectorMultiply(
					vectorCreate(
							cos(vehicle->heading),
							sin(vehicle->heading)
					),
					vehicle->length / 2
			)
	);
	vehicle->front_axle = vectorAdd(
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

void vehicle_calculate_wheel_position(vehicle_t *vehicle)
{
	vehicle->wheels[0] = vectorSubtract(
			vehicle->back_axle,
			vectorMultiply(
					vectorCreate(
							cos(vehicle->heading + PI / 2),
							sin(vehicle->heading + PI / 2)
					),
					vehicle->width / 2
			)
	);
	vehicle->wheels[1] = vectorSubtract(
			vehicle->back_axle,
			vectorMultiply(
					vectorCreate(
							cos(vehicle->heading - PI / 2),
							sin(vehicle->heading - PI / 2)
					),
					vehicle->width / 2
			)
	);
	vehicle->wheels[2] = vectorAdd(
			vehicle->front_axle,
			vectorMultiply(
					vectorCreate(
							cos(vehicle->heading + PI / 2),
							sin(vehicle->heading + PI / 2)
					),
					vehicle->width / 2
			)
	);
	vehicle->wheels[3] = vectorAdd(
			vehicle->front_axle,
			vectorMultiply(
					vectorCreate(
							cos(vehicle->heading - PI / 2),
							sin(vehicle->heading - PI / 2)
					),
					vehicle->width / 2
			)
	);
}

vehicle_limits_collision_t vehicle_check_limits_collision(vehicle_t *vehicle, unsigned width, unsigned height)
{
	vehicle_limits_collision_t vehicle_limits_collision;
	vehicle_limits_collision.all = 0;
	size_t i;
	for (i = 0; i < VEHICLE_NUM_WHEELS; ++i)
	{
		if (vehicle->wheels[i].x < 0)
		{
			vehicle_limits_collision.left = true;
		}
		else if (vehicle->wheels[i].x >= width)
		{
			vehicle_limits_collision.right = true;
		}
		if (vehicle->wheels[i].y < 0)
		{
			vehicle_limits_collision.top = true;
		}
		else if (vehicle->wheels[i].y >= height)
		{
			vehicle_limits_collision.bottom = true;
		}
	}
	return vehicle_limits_collision;
}

bool vehicle_check_vehicle_collision(vehicle_t *vehicle, vehicle_t *vehicle2)
{
	if (vectorDistance(vehicle->position, vehicle2->position) < MIN(vehicle->length, vehicle->width) / 2 + MIN(vehicle2->length, vehicle2->width) / 2)
	{
		return true; // Inside smaller bounding box
	}
	if (vectorDistance(vehicle->position, vehicle2->position) > MAX(vehicle->length, vehicle->width) / 2 + MAX(vehicle2->length, vehicle2->width) / 2)
	{
		return false; // Outside larger bounding box
	}

	// Outside smaller bounding box but inside larger bounding box
	size_t i;
	for (i = 0; i < VEHICLE_NUM_WHEELS; ++i)
	{
		if (isPointInPolygon(vehicle2->wheels, VEHICLE_NUM_WHEELS, vehicle->wheels[i]))
		{
			return true; // Wheel inside the other vehicle
		}
	}
}

void vehicle_vehicle_collision_handler(vehicle_t *vehicle, vehicle_t *vehicle2)
{
	// TODO
	vehicle->speed = 0.0;
	vehicle->position = vehicle->oldPosition;
	vehicle2->speed = 0.0;
	vehicle2->position = vehicle2->oldPosition;
}

void vehicle_limits_collision_handler(vehicle_t *vehicle, vector2D_t oldPosition, vehicle_limits_collision_t vehicle_limits_collision, unsigned width, unsigned height)
{
	if (vehicle_limits_collision.left)
	{
		vehicle->heading -= (VEHICLE_STEER * vehicle->length * sin(vehicle->heading)) / VEHICLE_COLLISION_FRICTION;
		vehicle_calculate_axle_position(vehicle);
		vehicle_update_position(vehicle);
		vehicle_calculate_wheel_position(vehicle);
		vehicle->position.x -= MIN(MIN(vehicle->wheels[0].x, vehicle->wheels[1].x), MIN(vehicle->wheels[2].x, vehicle->wheels[3].x));
	}
	if (vehicle_limits_collision.right)
	{
		vehicle->heading += (VEHICLE_STEER * vehicle->length * sin(vehicle->heading)) / VEHICLE_COLLISION_FRICTION;
		vehicle_calculate_axle_position(vehicle);
		vehicle_update_position(vehicle);
		vehicle_calculate_wheel_position(vehicle);
		vehicle->position.x -= MAX(MAX(vehicle->wheels[0].x, vehicle->wheels[1].x), MAX(vehicle->wheels[2].x, vehicle->wheels[3].x)) - width;
	}
	if (vehicle_limits_collision.top)
	{
		vehicle->heading += (VEHICLE_STEER * vehicle->length * cos(vehicle->heading)) / VEHICLE_COLLISION_FRICTION;
		vehicle_calculate_axle_position(vehicle);
		vehicle_update_position(vehicle);
		vehicle_calculate_wheel_position(vehicle);
		vehicle->position.y -= MIN(MIN(vehicle->wheels[0].y, vehicle->wheels[1].y), MIN(vehicle->wheels[2].y, vehicle->wheels[3].y));
	}
	if (vehicle_limits_collision.bottom)
	{
		vehicle->heading -= (VEHICLE_STEER * vehicle->length * cos(vehicle->heading)) / VEHICLE_COLLISION_FRICTION;
		vehicle_calculate_axle_position(vehicle);
		vehicle_update_position(vehicle);
		vehicle_calculate_wheel_position(vehicle);
		vehicle->position.y -= MAX(MAX(vehicle->wheels[0].y, vehicle->wheels[1].y), MAX(vehicle->wheels[2].y, vehicle->wheels[3].y)) - height;
	}
}

int vehicle_draw(vehicle_t *vehicle, vbe_mode_info_t *vmi_p)
{
	/*size_t i;
	for (i = 0; i < VEHICLE_NUM_WHEELS; ++i)
	{
		vg_draw_circle(vehicle->wheels[i].x, vehicle->wheels[i].y, 2, 0x0);
	}
	for (i = 0; i < VEHICLE_NUM_WHEELS; ++i)
	{
		vg_draw_line(vehicle->wheels[i % VEHICLE_NUM_WHEELS].x, vehicle->wheels[i % VEHICLE_NUM_WHEELS].y, vehicle->wheels[(i + 1) % VEHICLE_NUM_WHEELS].x, vehicle->wheels[(i + 1) % VEHICLE_NUM_WHEELS].y, 0x4);
	}
	for (i = 0; i < VEHICLE_NUM_WHEELS / 2; ++i)
	{
		vg_draw_line(vehicle->wheels[i % VEHICLE_NUM_WHEELS].x, vehicle->wheels[i % VEHICLE_NUM_WHEELS].y, vehicle->wheels[(i + 2) % VEHICLE_NUM_WHEELS].x, vehicle->wheels[(i + 2) % VEHICLE_NUM_WHEELS].y, 0x4);
	}*/

	int xpm_width, xpm_height;
	char *xpm = read_xpm(pixmap_get(6), &xpm_width, &xpm_height, vmi_p->XResolution, vmi_p->YResolution);
	unsigned short xpm_width2 = xpm_width;
	unsigned short xpm_height2 = xpm_height;
	char *pixmap;
	if ((pixmap = pixmap_rotate(vmi_p, xpm, &xpm_width2, &xpm_height2, vehicle->heading)) == NULL)
	{
		return 1;
	}
	xpm_width = xpm_width2;
	xpm_height = xpm_height2;
	vg_draw_pixmap(vehicle->position.x - xpm_width / 2, vehicle->position.y - xpm_height / 2, pixmap, (unsigned short)xpm_width, (unsigned short)xpm_height);
	free(xpm);
	free(pixmap);
}

void vehicle_delete(vehicle_t *vehicle)
{
	free(vehicle);
}
