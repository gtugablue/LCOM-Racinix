#ifndef _TRACK_H
#define _TRACK_H

#include <stdlib.h>
#include "vector2D.h"
#include <stdbool.h>
#include "math.h"
#include "video_gr.h"
#include <stdbool.h>

#define TRACK_INTERP_PERIOD							0.07
#define TRACK_GRASS_DRAG							0.3
#define TRACK_CONTROL_POINT_PERTURBATION_MAX		40.0
#define TRACK_CONTROL_POINT_PERTURBATION_EXP		1.0
#define TRACK_THICKNESS								65
#define RACINIX_COLOR_TRACK							rgb(20, 20, 20)
#define RACINIX_COLOR_GRASS							rgb(0, 90, 0)

#define TRACK_GENERATION_SIZE_FACTOR				0.6
#define TRACK_GENERATION_MIN_POINTS					10
#define TRACK_GENERATION_MAX_POINTS					20
#define TRACK_GENERATION_MIN_POINT_DISTANCE			100
#define TRACK_GENERATION_MAX_ANGLE					100
#define TRACK_GENERATION_NUM_TRIES					100
#define TRACK_GENERATION_POINT_SEP_FACTOR			1.4
//#define TRACK_GENERATION_ALLOW_INTERSECTIONS		Uncomment to allow the track to intersect itself

/** @defgroup track Track module
 * @{
 *
 * Module responsible for loading and handle everything that is used on the track
 *
 */

/** @name Track structure */
/** @{
 *
 * Track structure
 */
typedef struct
{
	bool *track_points;
	unsigned width;
	unsigned height;
	vector2D_t* spline;
	vector2D_t* inside_spline;
	vector2D_t* outside_spline;
	unsigned spline_size;
	vector2D_t* control_points;
	unsigned num_control_points;
} track_t;
/** @} end of track_t */


/**
 * @brief
 *
 * @param width
 * @param height
 *
 * @return
 */
track_t *track_create(unsigned width, unsigned height);

/**
 * @brief
 *
 * @param track
 * @param seed
 *
 * @return
 */
int track_random_generate(track_t *track, unsigned long seed);

/**
 * @brief
 *
 * @param track
 *
 * @return
 */
int track_generate_spline(track_t *track);

/**
 * @brief
 *
 * @param track
 *
 * @return
 */
int track_update_track_points(track_t *track);

/**
 * @brief
 *
 * @param track
 */
void track_draw(track_t *track);

/**
 * @brief
 *
 * @param track
 */
void track_draw_spline(track_t *track);

/**
 * @brief
 *
 * @param track
 */
void track_draw_control_points(track_t *track);

/**
 * @brief
 *
 * @param track
 */
void track_draw_finish_line(track_t *track);

/**
 * @brief
 *
 * @param track
 * @param x
 * @param y
 * @param width
 * @param height
 *
 * @return
 */
double track_get_point_drag(track_t *track, int x, int y, unsigned width, unsigned height);

/**
 * @brief
 *
 * @param track
 * @param seed
 *
 * @return
 */
int track_generate_control_points(track_t *track, unsigned long *seed);

/**
 * @brief
 *
 * @param track
 */
void track_generate_set_start_line(track_t *track);

/**
 * @brief
 *
 * @param track
 * @param point
 *
 * @return
 */
unsigned track_get_closest_control_point(track_t *track, vector2D_t point);

/**
 * @brief
 *
 * @param track
 * @param point
 *
 * @return
 */
unsigned track_get_closest_spline_point(track_t *track, vector2D_t point);

/**
 * @brief
 *
 * @param track
 * @param after_point_ID
 *
 * @return
 */
int track_add_control_point(track_t *track, unsigned after_point_ID);

/**
 * @brief
 *
 * @param track
 * @param point_ID
 *
 * @return
 */
int track_erase_control_point(track_t *track, unsigned point_ID);

/**
 * @brief
 *
 * @param track
 * @param spline_point_ID
 *
 * @return
 */
unsigned track_spline_to_control_point(track_t *track, unsigned spline_point_ID);

/**
 * @brief
 *
 * @param track
 */
void track_delete(track_t *track);

/** @} end of track */
#endif
