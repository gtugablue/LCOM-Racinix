#ifndef _BITMAP2_H // _BITMAP_H is already defined by Minix
#define _BITMAP2_H

#include <minix/syslib.h>

typedef struct
{
	uint16_t signature;
	uint32_t file_size;
	uint16_t reserved[2];
	void *pixel_array;
} __attribute__((packed)) bitmap_file_header_t;

typedef struct
{
	uint32_t information_header_size;
	uint32_t width;
	uint32_t height;
	uint16_t num_color_planes;
	uint16_t bits_per_pixel;
	uint32_t compression_method;
	uint32_t image_size;
	uint32_t horizontal_resolution;
	uint32_t vertical_resolution;
	uint32_t num_colours;
	uint32_t num_important_colours;
} __attribute__((packed)) bitmap_information_header_t;

typedef struct
{
	bitmap_information_header_t bitmap_information_header;
	void *pixel_array;
} bitmap_t;

bitmap_t *bitmap_load(const char *file);

void bitmap_draw_alpha(bitmap_t *bitmap, int x, int y);

void bitmap_draw(bitmap_t *bitmap, int x, int y); // For efficiency purposes

void bitmap_delete(bitmap_t *bitmap);

#endif
