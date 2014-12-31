#ifndef _FONT_H
#define _FONT_H

#include "video_gr.h"
#include "bitmap.h"

#define FONT_NUM_LETTERS			26
#define FONT_NUM_DIGITS				10
#define FONT_LETTER_SPACING_FACTOR	0.15
#define FONT_TRANSPARENT_COLOR		VIDEO_GR_WHITE
#define FONT_BITMAP_HEIGHT			80

/** @name Font Alignment  */
/** @{ */
typedef enum
{
	FONT_ALIGNMENT_LEFT,
	FONT_ALIGNMENT_MIDDLE,
	FONT_ALIGNMENT_RIGHT
} font_alignment_t;
/** @} end of Font Alignment*/

/** @name Font structure */
/** @{
 *
 * Font structure
 */
typedef struct
{
	bitmap_t **lower_case;
	bitmap_t **upper_case;
	bitmap_t **digit;
	bitmap_t *dot;
	bitmap_t *exclamation_mark;
	bitmap_t *question_mark;
	bitmap_t *comma;
	bitmap_t *quotation_mark;
	bitmap_t *brackets_open;
	bitmap_t *brackets_close;
	bitmap_t *hyphen;
	bitmap_t *colon;
	bitmap_t *space;
	bitmap_t *right_slash;
} font_t;
/** @} end of font_t */

/**
 * @brief Font loader
 *
 * @param folder the folder where the function is going to select the letter
 *
 * @return A pointer to the letter on success, NULL otherwise
 */
font_t *font_load(const char* folder);


/**
 * @brief Shows the string of the font
 *
 * @param font struct of the font
 * @param string string that is written
 * @param height height of the font
 * @param x x axis coordinate of the position of the top left corner of the font
 * @param y y axis coordinate of the position of the top left corner of the font
 * @param font_alignment_t :: font_alignment_t
 * @param color color of the font
 * @param shade number of pixels to the right and down where it will put the shade
 */
void font_show_string(font_t *font, const unsigned char *string, unsigned height, int x, int y, font_alignment_t font_alignment, uint16_t color, unsigned shade);

/**
 * @brief Shows the string of the font
 *
 * @param font struct of the font
 * @param string string that is written
 * @param height height of the font
 *
 * @return The width of the string
 */
unsigned font_calculate_string_width(font_t *font, const unsigned char *string, unsigned height);

/**
 * @brief Delete the font
 *
 * @param font struct of the font
 */
void font_delete(font_t *font);

/** @} end of Font */

#endif
