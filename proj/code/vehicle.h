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

/** @defgroup vehicle Vehicle Module
 * @{
 *
 * Module responsible for handle everything that is related to the vehicle (like velocity, rotation,...)
 *
 */

/** @name Vehicle Keys structure */
/** @{
 *
 * Vehicle Keys structure
 */
typedef struct
{
	int accelerate;
	int brake;
	int turn_left;
	int turn_right;
} vehicle_keys_t;
/** @} end of vehicle_keys_t */


/** @name Vehicle structure */
/** @{
 *
 * Vehicle structure
 */
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
/** @} end of vehicle_t */


/** @name Vehicle limits collision structure */
/** @{
 *
 * Vehicle limits collision structure
 */
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
/** @} end of vehicle_limits_collision_t */


// Creates the vehicle and scales the bitmap according to its size (for efficiency purposes)
/**
 * @brief
 *
 * @param width
 * @param length
 * @param position
 * @param heading
 * @param bitmap
 * @param vehicle_keys
 * @param checkpoint_color
 *
 * @return
 */
vehicle_t *vehicle_create(double width, double length, const vector2D_t *position, double heading, bitmap_t *bitmap, vehicle_keys_t vehicle_keys, uint16_t checkpoint_color);

/**
 * @brief
 *
 * @param vehicle
 * @param track
 * @param vmi_p
 * @param delta_time
 * @param drag
 */
void vehicle_tick(vehicle_t *vehicle, track_t *track, vbe_mode_info_t *vmi_p, double delta_time, double drag);

/**
 * @brief
 *
 * @param vehicle
 * @param delta_time
 */
void vehicle_update_steering(vehicle_t *vehicle, double delta_time);

/**
 * @brief
 *
 * @param vehicle
 * @param delta_time
 * @param drag
 */
void vehicle_update_speed(vehicle_t *vehicle, double delta_time, double drag);

/**
 * @brief
 *
 * @param vehicle
 * @param delta_time
 */
void vehicle_update_axles(vehicle_t *vehicle, double delta_time);

/**
 * @brief
 *
 * @param vehicle
 */
void vehicle_update_position(vehicle_t *vehicle);

/**
 * @brief
 *
 * @param vehicle
 */
void vehicle_update_heading(vehicle_t *vehicle);

/**
 * @brief
 *
 * @param vehicle
 */
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

/**
 * @brief
 *
 * @param vehicle
 * @param width
 * @param height
 *
 * @return
 */
vehicle_limits_collision_t vehicle_check_limits_collision(vehicle_t *vehicle, unsigned width, unsigned height);

/**
 * @brief
 *
 * @param vehicle
 * @param oldPosition
 * @param vehicle_limits_collision
 * @param width
 * @param height
 */
void vehicle_limits_collision_handler(vehicle_t *vehicle, vector2D_t oldPosition, vehicle_limits_collision_t vehicle_limits_collision, unsigned width, unsigned height);

/**
 * @brief
 *
 * @param vehicle
 * @param vehicle2
 *
 * @return
 */
int vehicle_check_vehicle_collision(vehicle_t *vehicle, vehicle_t *vehicle2);

/**
 * @brief
 *
 * @param vehicle
 * @param wheel_ID
 * @param vehicle2
 *
 */
void vehicle_vehicle_collision_handler(vehicle_t *vehicle, unsigned wheel_ID, vehicle_t *vehicle2);

/**
 * @brief
 *
 * @param vehicle
 * @param wheel_ID
 * @param vehicle2
 */
void vehicle_vehicle_collision_handler_position_fix(vehicle_t *vehicle, unsigned wheel_ID, vehicle_t *vehicle2);

/**
 * @brief
 *
 * @param vehicle
 * @param track
 *
 * @return
 */
bool vehicle_check_checkpoint_collision(vehicle_t *vehicle, track_t *track);

/**
 * @brief
 *
 * @param vehicle
 * @param track
 *
 */
void vehicle_checkpoint_collision_handler(vehicle_t *vehicle, track_t *track);

/**
 * @brief
 *
 * @param vehicle
 *
 * @return
 */
int vehicle_draw(vehicle_t *vehicle);

/**
 * @brief
 *
 * @param vehicle
 *
 */
void vehicle_delete(vehicle_t *vehicle);

/** @name Vehicle collision info */
/** @{ */
enum
{
	VEHICLE_NO_COLLISION,
	VEHICLE_LEFT_COLLISION,
	VEHICLE_BOTTOM_COLLISION,
	VEHICLE_RIGHT_COLLISION,
	VEHICLE_TOP_COLLISION
};
/** @} end of Vehicle collision info */


/** @} end of vehicle */
#endif
