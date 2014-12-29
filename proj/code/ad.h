#ifndef _AD_H
#define _AD_H

#include "font.h"
#include "video_gr.h"

#define AD_FONT_HEIGHT				15
#define AD_BOTTOM_MARGIN			15

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

ad_t *ad_create(const char *file, double display_speed, font_t *font, uint16_t color);

void ad_generate_new(ad_t *ad);

void ad_tick(ad_t *ad, double delta_time);

void ad_show(ad_t *ad);

void ad_delete(ad_t *ad);

#endif
