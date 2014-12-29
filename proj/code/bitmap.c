#include "bitmap.h"
#include "video_gr.h"
#include <stdlib.h>
#include <minix/driver.h>

#define BITMAP_SIGNATURE		0x4D42

bitmap_t *bitmap_load(const char* filename)
{
	bitmap_t *bmp;
	if ((bmp = (bitmap_t *)malloc(sizeof(bitmap_t))) == NULL)
	{
		return NULL;
	}
	// open file
	FILE *fp;
	if ((fp = fopen(filename, "rb")) == NULL) // "read + binary" mode
	{
		free(bmp);
		return NULL;
	}
	// read the bitmap file header
	bitmap_file_header_t bitmap_file_header;
	fread(&bitmap_file_header, sizeof(bitmap_file_header_t), 1, fp);

	if (bitmap_file_header.signature != BITMAP_SIGNATURE)
	{
		bitmap_delete(bmp);
		return NULL;
	}

	// read the bitmap information header
	fread(&bmp->bitmap_information_header, sizeof(bitmap_information_header_t), 1, fp);

	/*// Support all number of bits per pixel
	if (bmp->bitmap_information_header.bits_per_pixel != 16)
	{
		bitmap_delete(bmp);
		return NULL;
	}*/

	// move file pointer to the beggining of the bitmap data
	fseek(fp, (uint32_t)bitmap_file_header.pixel_array, SEEK_SET);

	if ((bmp->pixel_array = (void *)malloc(bmp->bitmap_information_header.image_size)) == NULL)
	{
		free(bmp);
		return NULL;
	}

	// read the bitmap image data
	if (bmp->bitmap_information_header.width % 2 == 0)
	{
		fread(bmp->pixel_array, bmp->bitmap_information_header.image_size, 1, fp);
	}
	else
	{
		// Fix for 16-bit mode
		size_t i;
		uint16_t discard;
		for (i = 0; i < bmp->bitmap_information_header.height; ++i)
		{
			fread((uint16_t *)bmp->pixel_array + i * bmp->bitmap_information_header.width, bmp->bitmap_information_header.width * bmp->bitmap_information_header.bits_per_pixel / 8, 1, fp);
			fread(&discard, 2, 1, fp);
		}
	}


	if (bmp->pixel_array == NULL)
	{
		bitmap_delete(bmp);
		return NULL;
	}

	// Close file
	fclose(fp);
	return bmp;
}

void bitmap_draw_alpha(bitmap_t *bitmap, int x, int y, unsigned long alpha_color)
{
	vbe_mode_info_t *vbe_mode_info = vg_get_vbe_mode_info();
	uint16_t *double_buffer = vg_get_double_buffer();
	uint16_t color;
	size_t i, j;
	int x_min = MAX(x, 0);
	int y_min = MAX(y, 0);
	int x_max = MIN(x + (int)bitmap->bitmap_information_header.width, vbe_mode_info->XResolution);
	int y_max = MIN(y + (int)bitmap->bitmap_information_header.height, vbe_mode_info->YResolution);
	for (j = y_min; j < y_max; ++j)
	{
		uint16_t *line = (uint16_t *)bitmap->pixel_array + (x_min - x) + (y_min + bitmap->bitmap_information_header.height - 1 - j) * bitmap->bitmap_information_header.width;
		for (i = x_min; i < x_max; ++i)
		{
			if (*line != alpha_color)
			{
				vg_set_pixel(i, j, *line);
			}
			++line;
		}
	}
}

void bitmap_draw(bitmap_t *bitmap, int x, int y)
{
	vbe_mode_info_t *vbe_mode_info = vg_get_vbe_mode_info();
	uint16_t *double_buffer = vg_get_double_buffer();
	size_t i;
	for (i = 0; i < bitmap->bitmap_information_header.height; ++i)
	{
		// TODO check para ver se não excede a borda direita
		memcpy(double_buffer + x + (y + bitmap->bitmap_information_header.height - 1 - i) * vbe_mode_info->XResolution, (uint16_t *)bitmap->pixel_array + i * MIN(bitmap->bitmap_information_header.width, x + vbe_mode_info->XResolution), bitmap->bitmap_information_header.width * vbe_mode_info->BitsPerPixel / 8);
	}
}

bitmap_t *bitmap_scale(bitmap_t *bitmap, unsigned new_width, unsigned new_height)
{
	double scale_x = (double)new_width / bitmap->bitmap_information_header.width;
	double scale_y = (double)new_height / bitmap->bitmap_information_header.height;
	int old_x, old_y;
	bitmap_t *scaled_bitmap;
	if ((scaled_bitmap = (bitmap_t *)malloc(sizeof(bitmap_t))) == NULL)
	{
		return NULL;
	}
	memcpy(&scaled_bitmap->bitmap_information_header, &bitmap->bitmap_information_header, sizeof(bitmap_information_header_t));
	scaled_bitmap->bitmap_information_header.width = new_width;
	scaled_bitmap->bitmap_information_header.height = new_height;
	scaled_bitmap->bitmap_information_header.image_size = new_width * new_height * scaled_bitmap->bitmap_information_header.bits_per_pixel / 8;
	if ((scaled_bitmap->pixel_array = malloc(scaled_bitmap->bitmap_information_header.image_size)) == NULL)
	{
		return NULL;
	}
	size_t x, y;
	for (x = 0; x < scaled_bitmap->bitmap_information_header.width; ++x)
	{
		for (y = 0; y < scaled_bitmap->bitmap_information_header.height; ++y)
		{
			// Scaling matrix
			old_x = (int)(x / scale_x);
			old_y = (int)(y / scale_y);

			if ((unsigned)old_x < bitmap->bitmap_information_header.width && (unsigned)old_y < bitmap->bitmap_information_header.height) // The 'unsigned' cast is here for efficiency purposes, as it reduces the number of comparisons from 4 to 2
			{
				*((uint16_t *)scaled_bitmap->pixel_array + x + y * scaled_bitmap->bitmap_information_header.width) = *((uint16_t *)bitmap->pixel_array + old_x + old_y * bitmap->bitmap_information_header.width);
			}
			else
			{
				*((uint16_t *)scaled_bitmap->pixel_array + x + y * scaled_bitmap->bitmap_information_header.width) = VIDEO_GR_64K_TRANSPARENT;
			}
		}
	}
	return scaled_bitmap;
}

bitmap_t *bitmap_transform(bitmap_t *bitmap, unsigned new_width, unsigned new_height, double angle)
{
	double angle_cos = cos(angle);
	double angle_sin = sin(angle);
	double scale_x = (double)new_width / bitmap->bitmap_information_header.width;
	double scale_y = (double)new_height / bitmap->bitmap_information_header.height;
	double transform_x, transform_y;
	int old_x, old_y;
	bitmap_t *rotated_bitmap;
	if ((rotated_bitmap = (bitmap_t *)malloc(sizeof(bitmap_t))) == NULL)
	{
		return NULL;
	}
	memcpy(&rotated_bitmap->bitmap_information_header, &bitmap->bitmap_information_header, sizeof(bitmap_information_header_t));
	//rotated_bitmap->bitmap_information_header = bitmap->bitmap_information_header;
	rotated_bitmap->bitmap_information_header.width = rotated_bitmap->bitmap_information_header.height = sqrt(ceil(scale_x * bitmap->bitmap_information_header.width * bitmap->bitmap_information_header.width) + ceil(scale_y * bitmap->bitmap_information_header.height * bitmap->bitmap_information_header.height));
	rotated_bitmap->bitmap_information_header.image_size = rotated_bitmap->bitmap_information_header.width * rotated_bitmap->bitmap_information_header.height * rotated_bitmap->bitmap_information_header.bits_per_pixel / 8;
	if ((rotated_bitmap->pixel_array = malloc(rotated_bitmap->bitmap_information_header.image_size)) == NULL)
	{
		return NULL;
	}
	int x, y;
	for (x = 0; x < rotated_bitmap->bitmap_information_header.width; ++x)
	{
		for (y = 0; y < rotated_bitmap->bitmap_information_header.height; ++y)
		{
			transform_x = (double)(x - (int16_t)rotated_bitmap->bitmap_information_header.width / 2);
			transform_y = (double)(y - (int16_t)rotated_bitmap->bitmap_information_header.height / 2);

			// Rotation matrix
			old_x = (int)((transform_x * angle_cos - transform_y * angle_sin) / scale_x + (double)bitmap->bitmap_information_header.width / 2);
			old_y = (int)((transform_y * angle_cos + transform_x * angle_sin) / scale_y + (double)bitmap->bitmap_information_header.height / 2);

			if ((unsigned)old_x < bitmap->bitmap_information_header.width && (unsigned)old_y < bitmap->bitmap_information_header.height) // The 'unsigned' cast is here for efficiency purposes, as it reduces the number of comparisons from 4 to 2
			{
				*((uint16_t *)rotated_bitmap->pixel_array + x + y * rotated_bitmap->bitmap_information_header.width) = *((uint16_t *)bitmap->pixel_array + old_x + old_y * bitmap->bitmap_information_header.width);
			}
			else
			{
				*((uint16_t *)rotated_bitmap->pixel_array + x + y * rotated_bitmap->bitmap_information_header.width) = VIDEO_GR_64K_TRANSPARENT;
			}
		}
	}
	return rotated_bitmap;
}

bitmap_t *bitmap_rotate(bitmap_t *bitmap, double angle)
{
	double angle_cos = cos(angle);
	double angle_sin = sin(angle);
	double transform_x, transform_y;
	int old_x, old_y;
	bitmap_t *rotated_bitmap;
	if ((rotated_bitmap = (bitmap_t *)malloc(sizeof(bitmap_t))) == NULL)
	{
		return NULL;
	}
	memcpy(&rotated_bitmap->bitmap_information_header, &bitmap->bitmap_information_header, sizeof(bitmap_information_header_t));
	//rotated_bitmap->bitmap_information_header = bitmap->bitmap_information_header;
	rotated_bitmap->bitmap_information_header.width = rotated_bitmap->bitmap_information_header.height = sqrt(ceil(bitmap->bitmap_information_header.width * bitmap->bitmap_information_header.width) + ceil(bitmap->bitmap_information_header.height * bitmap->bitmap_information_header.height));
	rotated_bitmap->bitmap_information_header.image_size = rotated_bitmap->bitmap_information_header.width * rotated_bitmap->bitmap_information_header.height * rotated_bitmap->bitmap_information_header.bits_per_pixel / 8;
	if ((rotated_bitmap->pixel_array = malloc(rotated_bitmap->bitmap_information_header.image_size)) == NULL)
	{
		return NULL;
	}
	int x, y;
	for (x = 0; x < rotated_bitmap->bitmap_information_header.width; ++x)
	{
		for (y = 0; y < rotated_bitmap->bitmap_information_header.height; ++y)
		{
			transform_x = (double)(x - (int16_t)rotated_bitmap->bitmap_information_header.width / 2);
			transform_y = (double)(y - (int16_t)rotated_bitmap->bitmap_information_header.height / 2);

			// Rotation matrix
			old_x = (int)((transform_x * angle_cos - transform_y * angle_sin) + (double)bitmap->bitmap_information_header.width / 2);
			old_y = (int)((transform_y * angle_cos + transform_x * angle_sin) + (double)bitmap->bitmap_information_header.height / 2);

			if (old_x >= 0 && old_x < bitmap->bitmap_information_header.width && old_y >= 0 && old_y < bitmap->bitmap_information_header.height)
			{
				*((uint16_t *)rotated_bitmap->pixel_array + x + y * rotated_bitmap->bitmap_information_header.width) = *((uint16_t *)bitmap->pixel_array + old_x + old_y * bitmap->bitmap_information_header.width);
			}
			else
			{
				*((uint16_t *)rotated_bitmap->pixel_array + x + y * rotated_bitmap->bitmap_information_header.width) = VIDEO_GR_64K_TRANSPARENT;
			}
		}
	}
	return rotated_bitmap;
}

void bitmap_delete(bitmap_t *bitmap)
{
	if (bitmap != NULL)
	{
		if (bitmap->pixel_array != NULL)
		{
			free(bitmap->pixel_array);
		}
		free(bitmap);
		bitmap = NULL;
	}
}
