#ifndef _FONT_H
#define _FONT_H

#include "video_gr.h"
#include "bitmap.h"

#define FONT_NUM_LETTERS			26
#define FONT_NUM_DIGITS				10
#define FONT_LETTER_SPACING_FACTOR	0.2
#define FONT_TRANSPARENT_COLOR		VIDEO_GR_WHITE
#define FONT_BITMAP_HEIGHT			80

typedef enum
{
	FONT_ALIGNMENT_LEFT,
	FONT_ALIGNMENT_MIDDLE,
	FONT_ALIGNMENT_RIGHT
} font_alignment_t;

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
} font_t;

font_t *font_load(const char* folder);

void font_show_string(font_t *font, char *string, unsigned height, unsigned x, unsigned y, font_alignment_t font_alignment, uint16_t color, unsigned shade);

unsigned font_calculate_string_width(font_t *font, char *string, unsigned height);

void font_delete(font_t *font);

#endif
