#include "pixmap.h"
#include <stdlib.h>
#include "video_gr.h"

char **pixmap_get(unsigned short id)
{
	switch(id)
	{
	case 0:
		return pic1;
	case 1:
		return pic2;
	case 2:
		return cross;
	case 3:
		return pic3;
	case 4:
		return penguin;
	default:
		return NULL;
	}
}

char* pixmap_rotate(char* pixmap, unsigned short *width, unsigned short *height, double angle)
{
	double angle_cos = cos(angle);
	double angle_sin = sin(angle);
	double transform_x, transform_y;
	int old_x, old_y;
	unsigned short old_width = *width;
	unsigned short old_height = *height;
	*width = *height = sqrt(old_width * old_width + old_height * old_height);
	char *new_pixmap;
	if ((new_pixmap = malloc(*width * *height * sizeof(char))) == NULL)
	{
		return NULL;
	}
	memset(new_pixmap, VIDEO_GR_TRANSPARENT, *width * *height * sizeof(char));
	int x, y;
	for (x = 0; x < *width; ++x)
	{
		for (y = 0; y < *height; ++y)
		{
			transform_x = (double)(x - *width / 2);
			transform_y = (double)(y - *height / 2);
			old_x = ((int)(transform_x * angle_cos + transform_y * angle_sin)) + old_width / 2;
			old_y = ((int)(transform_y * angle_cos - transform_x * angle_sin)) + old_height / 2;
			if (old_x >= 0 && old_x < old_width && old_y >= 0 && old_y < old_height)
			{
				*(new_pixmap + x + y * *width) = *(pixmap + old_x + old_y * old_width);
			}
		}
	}
	return new_pixmap;
}
