#ifndef __VIDEO_GR_H
#define __VIDEO_GR_H

#define VIDEO_GR_256_TRANSPARENT		0x00
#define VIDEO_GR_256_BLACK				0xFF
#define VIDEO_GR_64K_TRANSPARENT		0x47E0
#define VIDEO_GR_RED					rgb(255, 0, 0)
#define VIDEO_GR_GREEN					rgb(0, 255, 0)
#define	VIDEO_GR_BLUE					rgb(0, 0, 255)
#define VIDEO_GR_BLACK					rgb(0, 0, 0)
#define VIDEO_GR_WHITE					rgb(255, 255, 255)
#define VIDEO_GR_RGB_MAX				(1 << 8)

#include <stdbool.h>
#include <minix/syslib.h>
#include "vector2D.h"
#include "bitmap.h"
#include "vbe.h"

/** @defgroup video_gr video_gr
 * @{
 *
 * Functions for outputing data to screen in graphics mode
 */

/**
 * @brief Initializes the video module in graphics mode
 * 
 * Uses the VBE INT 0x10 interface to set the desired
 *  graphics mode, maps VRAM to the process' address space and
 *  initializes static global variables with the resolution of the screen, 
 *  and the number of colors
 * 
 * @param mode 16-bit VBE mode to set
 * @param PhysBasePtr address of memory where to write the mode info
 * @return Virtual address VRAM was mapped to. NULL, upon failure.
 */
void *vg_init(unsigned short mode);

/**
 * @brief Fills the screen with the input color
 * 
 * @param color color to fill the screen with
 * @return 0 on success, non-zero upon failure
 */
int vg_fill(uint16_t color);

/**
 * @brief Sets input pixel with input color
 * 
 * Sets the color of the pixel at the specified position to the input value, 
 *  by writing to the corresponding location in VRAM
 * 
 * @param x horizontal coordinate, starts at 0 (leftmost pixel)
 * @param y vertical coordinate, starts at 0 (top pixel)
 * @param color color to set the pixel
 * @return 0 on success, non-zero otherwise
 */
inline int vg_set_pixel(unsigned long x, unsigned long y, uint16_t color);

/**
 * @brief
 *
 * @param x
 * @param y
 * @param color
 *
 * @return
 */
inline int vg_set_mouse_pixel(unsigned long x, unsigned long y, uint16_t color);

/**
 * @brief Returns the color of the input pixel
 * 
 * Returns the color of the pixel at the specified position, 
 *  by reading to the corresponding location in VRAM
 * 
 * @param x horizontal coordinate, starts at 0 (leftmost pixel)
 * @param y vertical coordinate, starts at 0 (top pixel)
 * @return color of the pixel at coordinates (x,y), or -1 if some input argument is not valid
 */
inline long vg_get_pixel(unsigned long x, unsigned long y);

/**
 * @brief Draw line segment with specified end points and color
 * 
 * Draws a line segment with the specified end points and the input color, 
 *  by writing to VRAM
 * 
 * @param xi horizontal coordinate of "first" endpoint, starts at 0 (leftmost pixel)  
 * @param yi vertical coordinate of "first" endpoint, starts at 0 (top pixel)
 * @param xf horizontal coordinate of "last" endpoint, starts at 0 (leftmost pixel)  
 * @param yf vertical coordinate of "last" endpoint, starts at 0 (top pixel)  
 * @param color color of the line segment to draw
 * @return 0 upon success, non-zero upon failure
 */
int vg_draw_line(long xi, long yi, long xf, long yf, long color);

/**
 * @brief
 *
 * @param x
 * @param y
 * @param width
 * @param height
 * @param color
 *
 * @return
 */
int vg_draw_rectangle(unsigned long x, unsigned long y, unsigned long width, unsigned long height, uint16_t color);

/**
 * @brief Draw circle with specified center, radius and color
 *
 * @param x horizontal coordinate of the center
 * @param y vertical coordinate of the center
 * @param radius radius length
 * @param color color of the circle to draw
 * @return 0 upon success, non-zero upon failure
 */
int vg_draw_circle(unsigned long x, unsigned long y, unsigned long radius, unsigned long color);

/**
 * @brief
 *
 * @param x
 * @param y
 * @param pixmap
 * @param width
 * @param height
 *
 * @return
 */
int vg_draw_pixmap(unsigned long x, unsigned long y, uint16_t *pixmap, unsigned short width, unsigned short height);

/**
 * @brief
 *
 * @param polygon
 * @param n
 * @param color
 *
 * @return
 */
int vg_draw_polygon(vector2D_t polygon[], unsigned n, unsigned long color);

/**
 * @brief
 *
 * @param x
 * @param y
 * @param bitmap
 */
void vg_draw_mouse(unsigned long x, unsigned long y, bitmap_t *bitmap);

/**
 * @brief
 *
 *
 * @return
 */
int vg_box_blur();

/**
 * @brief
 *
 * @return
 */
vbe_mode_info_t *vg_get_vbe_mode_info();

/**
 * @brief
 *
 */
void *vg_get_double_buffer();

/**
 * @brief
 *
 */
void vg_swap_buffer();

/**
 * @brief
 *
 */
void vg_swap_mouse_buffer();

/**
 * @brief
 *
 * @param r
 * @param g
 * @param b
 *
 * @return
 */
uint16_t rgb(unsigned char r, unsigned char g, unsigned char b);

 /**
 * @brief Returns to default Minix 3 text mode (0x03: 25 x 80, 16 colors)
 * 
 * @return 0 upon success, non-zero upon failure
 */
int vg_exit(void);

 /** @} end of video_gr */
 
#endif /* __VIDEO_GR_H */
