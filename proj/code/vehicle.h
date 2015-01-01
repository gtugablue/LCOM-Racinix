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
 * @brief Vehicle constructor
 *
 * @param width width that is given to the vehicle
 * @param length length of the vehicle
 * @param position position that the vehicle will have
 * @param heading direction where the car is turned
 * @param bitmap the bitmap that the vehicle will embody
 * @param vehicle_keys the keys to control the vehicle
 * @param checkpoint_color the color of the checkpoint that will match to this vehicle
 *
 * @return A pointer to the vehicle on sucess, NULL otherwise
 */
vehicle_t *vehicle_create(double width, double length, const vector2D_t *position, double heading, bitmap_t *bitmap, vehicle_keys_t vehicle_keys, uint16_t checkpoint_color);

/**
 * @brief Update the car
 *
 * Updates the position and everything that has to do with the car when one condition changes
 *
 * @param vehicle the vehicle that will be updated
 * @param track the track where the vehicle is
 * @param vmi_p the video mode information
 * @param delta_time the time a key was pressed	to later do the rate to see how much did the user want to update the car (velocity, steering,...)
 * @param drag the drag that the speed will suffer
 */
void vehicle_tick(vehicle_t *vehicle, track_t *track, vbe_mode_info_t *vmi_p, double delta_time, double drag);

/**
 * @brief Updates the steering of the vehicle
 *
 * @param vehicle the vehicle that will be updated
 * @param delta_time the time a key was pressed to turn the vehicle
 */
void vehicle_update_steering(vehicle_t *vehicle, double delta_time);

/**
 * @brief Updates the speed of the vehicle
 *
 * @param vehicle the vehicle that will be updated
 * @param delta_time the time a key was pressed update the speed (increase or decrease)
 * @param drag
 */
void vehicle_update_speed(vehicle_t *vehicle, double delta_time, double drag);

/**
 * @brief Updates the axles of the vehicle
 *
 * @param vehicle the vehicle that will be updated
 * @param delta_time the time during which the axles are updated
 */
void vehicle_update_axles(vehicle_t *vehicle, double delta_time);

/**
 * @brief Updates the position of the vehicle
 *
 * @param vehicle the vehicle that will be updated
 */
void vehicle_update_position(vehicle_t *vehicle);

/**
 * @brief Updates the direction where the car is turned
 *
 * @param vehicle the vehicle that will be updated
 */
void vehicle_update_heading(vehicle_t *vehicle);

/**
 * @brief Calculates the axle position
 *
 * @param vehicle vehicle info
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
 * @brief Checks limits collision
 *
 * checks if the car has collided with the limits of the screen
 *
 * @param vehicle vehicle information
 * @param width width of the vehicle
 * @param height height of the vehicle
 *
 * @return the vehicle limits collision
 */
vehicle_limits_collision_t vehicle_check_limits_collision(vehicle_t *vehicle, unsigned width, unsigned height);

/**
 * @brief Handles the behavior car when it has collided with the screen limits
 *
 * @param vehicle vehicle info
 * @param oldPosition the previous position of the vehicle (before the collision)
 * @param vehicle_limits_collision the vehicle collision with the limits
 * @param width widht of the vehicle
 * @param height height of the vehicle
 */
void vehicle_limits_collision_handler(vehicle_t *vehicle, vector2D_t oldPosition, vehicle_limits_collision_t vehicle_limits_collision, unsigned width, unsigned height);

/**
 * @brief Checks if the vehicle has collided
 *
 * Checks if the vehicle has collided with another vehicle
 *
 * @param vehicle info of the first vehicle
 * @param vehicle2 info of the second vehicle
 *
 * @return
 */
int vehicle_check_vehicle_collision(vehicle_t *vehicle, vehicle_t *vehicle2);

/**
 * @brief Handles the vehicle collision
 *
 * In case of collision, handles the behavior of the car
 *
 * @param vehicle info of the first vehicle
 * @param wheel_ID the id of the wheel (to know which wheel it is)
 * @param vehicle2 info of the second vehicle
 *
 */
void vehicle_vehicle_collision_handler(vehicle_t *vehicle, unsigned wheel_ID, vehicle_t *vehicle2);

/**
 * @brief Move away the vehicle to stop collide
 *
 * @param vehicle info of the first vehicle
 * @param wheel_ID id of the wheel
 * @param vehicle2 info of the second vehicle
 */
void vehicle_vehicle_collision_handler_position_fix(vehicle_t *vehicle, unsigned wheel_ID, vehicle_t *vehicle2);

/**
 * @brief Check if the vehicle has collided with the checkpoint
 *
 * @param vehicle vehicle info
 * @param track track info
 *
 * @return Return true on sucess, false otherwise
 */
bool vehicle_check_checkpoint_collision(vehicle_t *vehicle, track_t *track);

/**
 * @brief Handles the behavior of the vehicle if it has collided with the checkpoint
 *
 * @param vehicle vehicle info
 * @param track track info (where the car is)
 *
 */
void vehicle_checkpoint_collision_handler(vehicle_t *vehicle, track_t *track);

/**
 * @brief Draws the vehicle
 *
 * @param vehicle vehicle info
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int vehicle_draw(vehicle_t *vehicle);

/**
 * @brief Deletes the vehicle
 *
 * @param vehicle vehicle info
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
