#include "ad.h"
#include <string.h>
#include <stdlib.h>

ad_t *ad_create(const char *file, double display_speed, font_t *font, uint16_t color)
{
	ad_t *ad;
	if ((ad = malloc(sizeof(ad_t))) == NULL)
	{
		return NULL;
	}
	FILE *fp;
	fp = fopen(file, "r");
	char string[500];
	ad->ads = NULL;
	ad->num_ads = 0;
	while (fgets(string, 498, fp) != NULL)
	{
		if ((ad->ads = realloc(ad->ads, (ad->num_ads + 1) * sizeof(char **))) == NULL)
		{
			return NULL;
		}
		size_t len = strlen(string);
		if (string[len - 1] == '\n')
		{
			string[len - 1] = '\0';
		}
		if ((ad->ads[ad->num_ads] = malloc((len + 1) * sizeof(char *))) == NULL)
		{
			return NULL;
		}
		strcpy(ad->ads[ad->num_ads], string);
		++ad->num_ads;
	}
	ad->display_speed = display_speed;
	ad->font = font;
	ad->color = color;
	ad->time = -1;
	return ad;
}

void ad_generate_new(ad_t *ad)
{
	ad->current_ad = rand() % ad->num_ads;
	ad->time = 0;
}

void ad_show(ad_t *ad)
{
	if (ad->time >= 0 && ad->num_ads > 0)
	{
		unsigned width = font_calculate_string_width(ad->font, ad->ads[ad->current_ad], AD_FONT_HEIGHT);
		vbe_mode_info_t *vbe_mode_info = vg_get_vbe_mode_info();
		font_show_string(ad->font, ad->ads[ad->current_ad], AD_FONT_HEIGHT, vbe_mode_info->XResolution - 1 - ad->time * ad->display_speed, vbe_mode_info->YResolution - 1 - AD_BOTTOM_MARGIN, FONT_ALIGNMENT_LEFT, ad->color, 0);

	}
}

void ad_delete(ad_t *ad)
{
	// TODO
	free(ad);
	ad = NULL;
}
