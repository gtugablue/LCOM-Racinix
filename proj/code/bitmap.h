#ifndef _BITMAP2_H // _BITMAP_H is already defined by Minix
#define _BITMAP2_H

#include <minix/syslib.h>

/** @defgroup bitmap bitmap
 * @{
 *
 * Module responsible for loading, transforming and showing 16 bits-per-pixel bitmaps.
 */

/** @name Packed Bitmap File Header block */
/**@{
 *
 * Stores general information about the Bitmap Image File.
 */

typedef struct
{
	uint16_t signature;
	uint32_t file_size;
	uint16_t reserved[2];
	void *pixel_array;
} __attribute__((packed)) bitmap_file_header_t;

/** @} end of bitmap_file_header_t*/

/** @name Packed Bitmap Information Header block */
/**@{
 *
 * Stores detailed information about the bitmap image and defines the pixel format
 */

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

/** @} end of bitmap_information_header_t*/

/** @name Bitmap structure */
typedef struct
{
	bitmap_information_header_t bitmap_information_header;
	void *pixel_array;
} bitmap_t;

/**
 * @brief Bitmap constructor
 *
 * @param file path and name of the file where the bitmap is located
 * @return A pointer to the new bitmap on success, NULL otherwise
 */
bitmap_t *bitmap_load(const char *file);

/**
 * @brief Resizes and scales a bitmap
 *
 * A new bitmap is generated, so it must be properly deleted later.
 *
 * @param bitmap pointer to the bitmap
 * @param new_width new bitmap width
 * @param new_height new bitmap height
 * @param angle rotation angle
 * @return A pointer to the new bitmap on success, NULL otherwise
 */
bitmap_t *bitmap_transform(bitmap_t *bitmap, unsigned new_width, unsigned new_height, double angle);

/**
 * @brief Scales a bitmap
 *
 * A new bitmap is generated, so it must be properly deleted later.
 *
 * @param bitmap pointer to the bitmap
 * @param new_width new bitmap width
 * @param new_height new bitmap height
 * @return A pointer to the new bitmap on success, NULL otherwise
 */
bitmap_t *bitmap_scale(bitmap_t *bitmap, unsigned new_width, unsigned new_height);

/**
 * @brief Rotates a bitmap
 *
 * A new bitmap is generated, so it must be properly deleted later.
 *
 * @param bitmap pointer to the bitmap
 * @param angle rotation angle
 * @return A pointer to the new bitmap on success, NULL otherwise
 */
bitmap_t *bitmap_rotate(bitmap_t *bitmap, double angle);

/**
 * @brief Draws a given bitmap with transparency
 *
 * Significantly slower than bitmap_draw() and requires a color to be considered as transparent.
 * Parts of the bitmap outside the screen will not be drawn.
 *
 * @param bitmap pointer to the bitmap
 * @param x top-left pixel x coordinate
 * @param y top-left pixel y coordinate
 * @param alpha_color to be considered transparent
 */
void bitmap_draw_alpha(bitmap_t *bitmap, int x, int y, unsigned long alpha_color);

/**
 * @brief Draws a given bitmap
 *
 * Significantly faster than bitmap_draw_alpha().
 * Parts of the bitmap outside the screen will not be drawn.
 *
 * @param bitmap pointer to the bitmap
 * @param x top-left pixel x coordinate
 * @param y top-left pixel y coordinate
 */
void bitmap_draw(bitmap_t *bitmap, int x, int y);

/**
 * @brief Deletes the bitmap
 *
 * Frees the pixel array and the bitmap itself
 *
 * @param bitmap pointer to the bitmap
 */
void bitmap_delete(bitmap_t *bitmap);

/** @} end of bitmap */

#endif
