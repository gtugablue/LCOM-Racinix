#include "vehicle.h"

#define PI 					3.14159265358979323846

vehicle_t *vehicle_create(double width, double length, const vector2D_t *position, double heading, bitmap_t *bitmap, vehicle_keys_t vehicle_keys, uint16_t checkpoint_color)
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
	vehicle->bitmap = bitmap_scale(bitmap, length, width);
	vehicle->vehicle_keys = vehicle_keys;
	vehicle->current_lap = 0;
	vehicle->current_checkpoint = 1;
	vehicle->checkpoint_color = checkpoint_color;
	vehicle_calculate_axle_position(vehicle);
	vehicle_calculate_wheel_position(vehicle);
	return vehicle;
}

void vehicle_tick(vehicle_t *vehicle, track_t *track, vbe_mode_info_t *vmi_p, double delta_time, double drag)
{
	vehicle_update_steering(vehicle, delta_time);

	vehicle_calculate_axle_position(vehicle);

	vehicle_update_axles(vehicle, delta_time);

	vehicle_update_position(vehicle);

	vehicle_update_heading(vehicle);

	vehicle_update_speed(vehicle, delta_time, drag);

	vehicle_calculate_wheel_position(vehicle);

	vehicle_limits_collision_handler(vehicle, vehicle->oldPosition, vehicle_check_limits_collision(vehicle, vmi_p->XResolution, vmi_p->YResolution), vmi_p->XResolution, vmi_p->YResolution);

	if (vehicle_check_checkpoint_collision(vehicle, track))
	{
		vehicle_checkpoint_collision_handler(vehicle, track);
	}

	vehicle_draw(vehicle);
}

void vehicle_update_steering(vehicle_t *vehicle, double delta_time)
{
	if (kbd_keys[vehicle->vehicle_keys.turn_left].pressed && !kbd_keys[vehicle->vehicle_keys.turn_right].pressed)
	{
		vehicle->steering += -(VEHICLE_STEER * vehicle->length) / (abs(vehicle->speed) * VEHICLE_STEERING_VS_SPEED_FACTOR);
	}
	else if (kbd_keys[vehicle->vehicle_keys.turn_right].pressed && !kbd_keys[vehicle->vehicle_keys.turn_left].pressed)
	{
		vehicle->steering += (VEHICLE_STEER * vehicle->length) / (abs(vehicle->speed) * VEHICLE_STEERING_VS_SPEED_FACTOR);
	}
	else
	{
		vehicle->steering = 0.0;
	}
	if (vehicle->steering > VEHICLE_MAX_STEER)
	{
		vehicle->steering = VEHICLE_MAX_STEER;
	}
	else if (vehicle->steering < -VEHICLE_MAX_STEER)
	{
		vehicle->steering = -VEHICLE_MAX_STEER;
	}
}

void vehicle_update_speed(vehicle_t *vehicle, double delta_time, double drag)
{
	if (kbd_keys[vehicle->vehicle_keys.brake].pressed)
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
	else if (kbd_keys[vehicle->vehicle_keys.accelerate].pressed)
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

int vehicle_check_vehicle_collision(vehicle_t *vehicle, vehicle_t *vehicle2)
{
	/*if (vectorDistance(vehicle->position, vehicle2->position) < MIN(vehicle->length, vehicle->width) / 2 + MIN(vehicle2->length, vehicle2->width) / 2)
	{
		return true; // Inside smaller bounding box
	}*/
	if (vectorDistance(vehicle->position, vehicle2->position) > MAX(vehicle->length, vehicle->width) / 2 + MAX(vehicle2->length, vehicle2->width) / 2)
	{
		return -1; // Outside larger bounding box
	}

	// Outside smaller bounding box but inside larger bounding box
	size_t i;
	for (i = 0; i < VEHICLE_NUM_WHEELS; ++i)
	{
		if (isPointInPolygon(vehicle2->wheels, VEHICLE_NUM_WHEELS, vehicle->wheels[i]))
		{
			return i; // Wheel inside the other vehicle
		}
	}
	return -1;
}

void vehicle_vehicle_collision_handler(vehicle_t *vehicle, unsigned wheel_ID, vehicle_t *vehicle2)
{
	// TODO

	// Torque = r x F
	vector2D_t r = vectorSubtract(vehicle->wheels[wheel_ID], vehicle2->position);
	vector2D_t F = vectorRotate(vectorMultiply(vectorCreate(1, 0), vehicle->speed), vehicle->heading);
	double torque = F.y * r.x - F.x * r.y;
	vehicle->heading -= VEHICLE_VEHICLE_COLLISION_MOMENTUM_FACTOR * (torque / vectorNorm(r));
	vehicle2->heading += VEHICLE_VEHICLE_COLLISION_MOMENTUM_FACTOR * (torque / vectorNorm(r));

	vector2D_t vehicle2_unit_velocity = vectorRotate(vectorCreate(1, 0), vehicle2->heading);
	double angle = atan2(vehicle2_unit_velocity.y - F.y, vehicle2_unit_velocity.x - F.x);
	vector2D_t Fprojected = vectorMultiply(vehicle2_unit_velocity, fabs(vehicle2->speed - vehicle->speed) * vectorNorm(F) * cos(angle));
	if (vectorNorm(Fprojected) < 10000)
	{
		vehicle->speed -= vectorNorm(Fprojected) / VEHICLE_VEHICLE_COLLISION_FRICTION;
		vehicle2->speed += vectorNorm(Fprojected) / VEHICLE_VEHICLE_COLLISION_FRICTION;
	}

	vehicle_vehicle_collision_handler_position_fix(vehicle, wheel_ID, vehicle2);
}

void vehicle_vehicle_collision_handler_position_fix(vehicle_t *vehicle, unsigned whefel_ID, vehicle_t *vehicle2)
{
	int wheel_ID1, wheel_ID2;
	size_t i;
	double step = 0.01;
	for (i = 0; i < 100; ++i)
	{
		wheel_ID1 = vehicle_check_vehicle_collision(vehicle, vehicle2);
		wheel_ID2 = vehicle_check_vehicle_collision(vehicle2, vehicle);
		if (wheel_ID1 != -1 || wheel_ID2 != -1)
		{
			vector2D_t r = vectorSubtract(vehicle2->position, vehicle->position);
			vehicle->position = vectorSubtract(vehicle->position, vectorDivide(r, vectorNorm(r) / step));
			vehicle2->position = vectorAdd(vehicle2->position, vectorDivide(r, vectorNorm(r) / step));
			vehicle_calculate_wheel_position(vehicle);
			vehicle_calculate_wheel_position(vehicle2);
			continue;
		}
		break;
	}
}

void vehicle_limits_collision_handler(vehicle_t *vehicle, vector2D_t oldPosition, vehicle_limits_collision_t vehicle_limits_collision, unsigned width, unsigned height)
{
	if (vehicle_limits_collision.left)
	{
		vehicle->heading -= (VEHICLE_STEER * vehicle->length * sin(vehicle->heading)) / VEHICLE_LIMITS_COLLISION_FRICTION;
		vehicle_calculate_axle_position(vehicle);
		vehicle_update_position(vehicle);
		vehicle_calculate_wheel_position(vehicle);
		vehicle->position.x -= MIN(MIN(vehicle->wheels[0].x, vehicle->wheels[1].x), MIN(vehicle->wheels[2].x, vehicle->wheels[3].x));
	}
	if (vehicle_limits_collision.right)
	{
		vehicle->heading += (VEHICLE_STEER * vehicle->length * sin(vehicle->heading)) / VEHICLE_LIMITS_COLLISION_FRICTION;
		vehicle_calculate_axle_position(vehicle);
		vehicle_update_position(vehicle);
		vehicle_calculate_wheel_position(vehicle);
		vehicle->position.x -= MAX(MAX(vehicle->wheels[0].x, vehicle->wheels[1].x), MAX(vehicle->wheels[2].x, vehicle->wheels[3].x)) - width;
	}
	if (vehicle_limits_collision.top)
	{
		vehicle->heading += (VEHICLE_STEER * vehicle->length * cos(vehicle->heading)) / VEHICLE_LIMITS_COLLISION_FRICTION;
		vehicle_calculate_axle_position(vehicle);
		vehicle_update_position(vehicle);
		vehicle_calculate_wheel_position(vehicle);
		vehicle->position.y -= MIN(MIN(vehicle->wheels[0].y, vehicle->wheels[1].y), MIN(vehicle->wheels[2].y, vehicle->wheels[3].y));
	}
	if (vehicle_limits_collision.bottom)
	{
		vehicle->heading -= (VEHICLE_STEER * vehicle->length * cos(vehicle->heading)) / VEHICLE_LIMITS_COLLISION_FRICTION;
		vehicle_calculate_axle_position(vehicle);
		vehicle_update_position(vehicle);
		vehicle_calculate_wheel_position(vehicle);
		vehicle->position.y -= MAX(MAX(vehicle->wheels[0].y, vehicle->wheels[1].y), MAX(vehicle->wheels[2].y, vehicle->wheels[3].y)) - height;
	}
}

bool vehicle_check_checkpoint_collision(vehicle_t *vehicle, track_t *track)
{
	if (vectorDistance(vehicle->position, track->control_points[vehicle->current_checkpoint]) < 50)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void vehicle_checkpoint_collision_handler(vehicle_t *vehicle, track_t *track)
{
	vehicle->current_checkpoint = (vehicle->current_checkpoint + 1) % (track->num_control_points);
	if (vehicle->current_checkpoint == 1)
	{
		++vehicle->current_lap;
	}
}

int vehicle_draw(vehicle_t *vehicle)
{
	bitmap_t *rotated_bitmap;
	if ((rotated_bitmap = bitmap_rotate(vehicle->bitmap, vehicle->heading)) == NULL)
	{
		return 1;
	}
	bitmap_draw_alpha(rotated_bitmap, vehicle->position.x - rotated_bitmap->bitmap_information_header.width / 2, vehicle->position.y - rotated_bitmap->bitmap_information_header.height / 2, VIDEO_GR_64K_TRANSPARENT);
	bitmap_delete(rotated_bitmap);

#ifdef VEHICLE_DRAW_VELOCITY
	vector2D_t velocity = vectorRotate(vectorCreate(0.25 * vehicle->speed, 0), vehicle->heading);
	velocity = vectorAdd(vehicle->position, velocity);
	vg_draw_line(vehicle->position.x, vehicle->position.y, velocity.x, velocity.y, 0xFFFF);
	vector2D_t arrow = vectorRotate(vectorCreate(7 * ((vehicle->speed > 0) - (vehicle->speed < 0)), 0), vehicle->heading);
	vg_draw_line(velocity.x, velocity.y, vectorAdd(velocity, vectorRotate(arrow, 5 * PI / 6)).x, vectorAdd(velocity, vectorRotate(arrow, 5 * PI / 6)).y, VIDEO_GR_WHITE);
	vg_draw_line(velocity.x, velocity.y, vectorAdd(velocity, vectorRotate(arrow, 7 * PI / 6)).x, vectorAdd(velocity, vectorRotate(arrow, 7 * PI / 6)).y, VIDEO_GR_WHITE);
#endif
	return 0;
}

void vehicle_delete(vehicle_t *vehicle)
{
	free(vehicle->bitmap);
	free(vehicle);
	vehicle = NULL;
}
