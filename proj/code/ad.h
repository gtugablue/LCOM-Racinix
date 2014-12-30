#ifndef _AD_H
#define _AD_H

#include "font.h"
#include "video_gr.h"

#define AD_FONT_HEIGHT				15
#define AD_BOTTOM_MARGIN			15

/** @defgroup ad ad
 * @{
 *
 * Module responsible for displaying ads
 */

/** @name Ad structure */

typedef struct
{
	unsigned num_ads;
	char **ads;
	double time; // If it's negative, do not show the ad
	double display_speed;
	int current_ad;
	font_t *font;
	uint16_t color;
} ad_t;

/**
 * @brief Ad constructor
 *
 * @param file path and name of the file where the list of ads is located (separated by a newline character)
 * @param display_speed ad horizontal scrolling velocity
 * @return A pointer to the new ad on success, NULL otherwise
 */
ad_t *ad_create(const char *file, double display_speed, font_t *font, uint16_t color);

/**
 * @brief New random ad generator
 *
 * Generates a new random ad from the list of ads initially loaded from the ads file.
 *
 * @param ad pointer to the ad
 */
void ad_generate_new(ad_t *ad);

/**
 * @brief Updates the ad position
 *
 * @param ad pointer to the ad
 * @param delta_time determines the amount of horizontal movement that corresponds to this tick
 */
void ad_tick(ad_t *ad, double delta_time);

/**
 * @brief Shows the ad
 *
 * @param ad pointer to the ad
 */
void ad_show(ad_t *ad);

/**
 * @brief Deletes the ad
 *
 * Frees all ads and the ad structure itself.
 *
 * @param ad pointer to the ad
 */
void ad_delete(ad_t *ad);

/** @} end of ad */

#endif
