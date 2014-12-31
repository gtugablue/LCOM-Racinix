#include "font.h"
#include <stdlib.h>
#include "string.h"
#include <minix/driver.h>

static int font_show_character(font_t *font, bitmap_t *character, unsigned height, int x, int y, uint16_t color, unsigned shade);
static bitmap_t *font_character_to_bitmap(font_t *font, char character);

font_t *font_load(const char* folder)
{
	font_t *font;
	if ((font = malloc(sizeof(font_t))) == NULL)
	{
		return NULL;
	}

	font->lower_case = NULL;
	font->upper_case = NULL;
	font->digit = NULL;
	font->dot = NULL;
	font->exclamation_mark = NULL;
	font->question_mark = NULL;
	font->comma = NULL;
	font->quotation_mark = NULL;
	font->brackets_open = NULL;
	font->brackets_close = NULL;
	font->hyphen = NULL;
	font->colon = NULL;
	font->space = NULL;
	font->right_slash = NULL;

	if ((font->lower_case = malloc(FONT_NUM_LETTERS * sizeof(bitmap_t *))) == NULL)
	{
		font_delete(font);
		return NULL;
	}
	memset(font->lower_case, (int)NULL, FONT_NUM_LETTERS * sizeof(bitmap_t *)); // Needed so that font_delete works properly
	if ((font->upper_case = malloc(FONT_NUM_LETTERS * sizeof(bitmap_t *))) == NULL)
	{
		font_delete(font);
		return NULL;
	}
	memset(font->upper_case, (int)NULL, FONT_NUM_LETTERS * sizeof(bitmap_t *)); // Needed so that font_delete works properly
	if ((font->digit = malloc(FONT_NUM_DIGITS * sizeof(bitmap_t *))) == NULL)
	{
		font_delete(font);
		return NULL;
	}
	memset(font->digit, (int)NULL, FONT_NUM_LETTERS * sizeof(bitmap_t *)); // Needed so that font_delete works properly

	if ((font->dot = malloc(sizeof(bitmap_t *))) == NULL)
	{
		font_delete(font);
		return NULL;
	}
	if ((font->exclamation_mark = malloc(sizeof(bitmap_t *))) == NULL)
	{
		font_delete(font);
		return NULL;
	}
	if ((font->question_mark = malloc(sizeof(bitmap_t *))) == NULL)
	{
		font_delete(font);
		return NULL;
	}
	if ((font->comma = malloc(sizeof(bitmap_t *))) == NULL)
	{
		font_delete(font);
		return NULL;
	}
	if ((font->quotation_mark = malloc(sizeof(bitmap_t *))) == NULL)
	{
		font_delete(font);
		return NULL;
	}
	if ((font->brackets_open = malloc(sizeof(bitmap_t *))) == NULL)
	{
		font_delete(font);
		return NULL;
	}
	if ((font->brackets_close = malloc(sizeof(bitmap_t *))) == NULL)
	{
		font_delete(font);
		return NULL;
	}
	if ((font->hyphen = malloc(sizeof(bitmap_t *))) == NULL)
	{
		font_delete(font);
		return NULL;
	}
	if ((font->colon = malloc(sizeof(bitmap_t *))) == NULL)
	{
		font_delete(font);
		return NULL;
	}
	if ((font->space = malloc(sizeof(bitmap_t *))) == NULL)
	{
		font_delete(font);
		return NULL;
	}
	if ((font->right_slash = malloc(sizeof(bitmap_t *))) == NULL)
	{
		font_delete(font);
		return NULL;
	}
	char *s;
	size_t i;
	for (i = 0; i < FONT_NUM_LETTERS; ++i)
	{
		// Lower case
		if (asprintf(&s, "%s/%c.bmp", folder, 'a' + i) == -1)
		{
			return NULL;
		}
		if ((font->lower_case[i] = bitmap_load(s)) == NULL)
		{
			free(s);
			return NULL;
		}
		free(s);

		// Upper case
		if (asprintf(&s, "%s/%c%c.bmp", folder, 'a' + i, 'a' + i) == -1)
		{
			return NULL;
		}
		if ((font->upper_case[i] = bitmap_load(s)) == NULL)
		{
			free(s);
			return NULL;
		}
		free(s);
	}

	for (i = 0; i < FONT_NUM_DIGITS; ++i)
	{
		// Digit
		if (asprintf(&s, "%s/%c.bmp", folder, '0' + i) == -1)
		{
			return NULL;
		}
		if ((font->digit[i] = bitmap_load(s)) == NULL)
		{
			free(s);
			return NULL;
		}
		free(s);
	}

	// Space
	if (asprintf(&s, "%s/space.bmp", folder) == -1)
	{
		return NULL;
	}
	if ((font->space = bitmap_load(s)) == NULL)
	{
		free(s);
		return NULL;
	}
	free(s);

	// Dot
	if (asprintf(&s, "%s/dot.bmp", folder) == -1)
	{
		return NULL;
	}
	if ((font->dot = bitmap_load(s)) == NULL)
	{
		free(s);
		return NULL;
	}
	free(s);

	// Exclamation mark
	if (asprintf(&s, "%s/exclamation.bmp", folder) == -1)
	{
		return NULL;
	}
	if ((font->exclamation_mark = bitmap_load(s)) == NULL)
	{
		free(s);
		return NULL;
	}
	free(s);

	// Question mark
	if (asprintf(&s, "%s/interrogation.bmp", folder) == -1)
	{
		return NULL;
	}
	if ((font->question_mark = bitmap_load(s)) == NULL)
	{
		free(s);
		return NULL;
	}
	free(s);

	// Comma
	if (asprintf(&s, "%s/comma.bmp", folder) == -1)
	{
		return NULL;
	}
	if ((font->comma = bitmap_load(s)) == NULL)
	{
		free(s);
		return NULL;
	}
	free(s);

	// Colon
	if (asprintf(&s, "%s/colon.bmp", folder) == -1)
	{
		return NULL;
	}
	if ((font->colon = bitmap_load(s)) == NULL)
	{
		free(s);
		return NULL;
	}
	free(s);

	// Right slash
	if (asprintf(&s, "%s/right_slash.bmp", folder) == -1)
	{
		return NULL;
	}
	if ((font->right_slash = bitmap_load(s)) == NULL)
	{
		free(s);
		return NULL;
	}
	free(s);

	return font;
}

static int font_show_character(font_t *font, bitmap_t *character, unsigned height, int x, int y, uint16_t color, unsigned shade)
{
	double ratio = (double)character->bitmap_information_header.width / character->bitmap_information_header.height;
	bitmap_t *scaled_bitmap;
	unsigned width = ratio * height;
	if (height == FONT_BITMAP_HEIGHT)
	{
		scaled_bitmap = character;
	}
	else
	{
		if ((scaled_bitmap = bitmap_scale(character, ratio * height, height * ((double)character->bitmap_information_header.height / FONT_BITMAP_HEIGHT))) == NULL)
		{
			return -1;
		}
	}

	vbe_mode_info_t *vbe_mode_info = vg_get_vbe_mode_info();
	uint16_t *double_buffer = vg_get_double_buffer();
	int i, j;
	// Shade
	if (shade > 0)
	{
		x += shade;
		y += shade;
		// Shade
		int x_min = MAX(x, 0);
		int y_min = MAX(y, 0);
		int x_max = MIN(x + (int)scaled_bitmap->bitmap_information_header.width, vbe_mode_info->XResolution);
		int y_max = MIN(y + (int)scaled_bitmap->bitmap_information_header.height, vbe_mode_info->YResolution);
		for (j = y_min; j < y_max; ++j)
		{
			uint16_t *line = (uint16_t *)scaled_bitmap->pixel_array + (x_min - x) + (y_min + scaled_bitmap->bitmap_information_header.height - 1 - j) * scaled_bitmap->bitmap_information_header.width;
			for (i = x_min; i < x_max; ++i)
			{
				if (*line == VIDEO_GR_BLACK)
				{
					vg_set_pixel(i, j, VIDEO_GR_BLACK);
				}
				++line;
			}
		}
		x -= shade;
		y -= shade;
	}
	// Character
	int x_min = MAX(x, 0);
	int y_min = MAX(y, 0);
	int x_max = MIN(x + (int)scaled_bitmap->bitmap_information_header.width, vbe_mode_info->XResolution);
	int y_max = MIN(y + (int)scaled_bitmap->bitmap_information_header.height, vbe_mode_info->YResolution);
	for (j = y_min; j < y_max; ++j)
	{
		uint16_t *line = (uint16_t *)scaled_bitmap->pixel_array + (x_min - x) + (y_min + scaled_bitmap->bitmap_information_header.height - 1 - j) * scaled_bitmap->bitmap_information_header.width;
		for (i = x_min; i < x_max; ++i)
		{
			if (*line == VIDEO_GR_BLACK)
			{
				vg_set_pixel(i, j, color);
			}
			++line;
		}
	}
	if (height != FONT_BITMAP_HEIGHT)
	{
		bitmap_delete(scaled_bitmap);
	}
	return width;
}

unsigned font_calculate_string_width(font_t *font, const unsigned char *string, unsigned height)
{
	bitmap_t *character;
	size_t i;
	unsigned string_width = 0;
	double ratio = (double)height / FONT_BITMAP_HEIGHT;
	unsigned length = -1;
	for (i = 0; i < strlen(string); ++i)
	{
		if ((character = font_character_to_bitmap(font, string[i])) != NULL)
		{
			string_width += ratio * character->bitmap_information_header.width; // char width
			++length;
		}
	}
	string_width += FONT_LETTER_SPACING_FACTOR * height * length; // spacing width
	return string_width;
}

void font_show_string(font_t *font, const unsigned char *string, unsigned height, int x, int y, font_alignment_t font_alignment, uint16_t color, unsigned shade)
{
	unsigned string_width = font_calculate_string_width(font, string, height);
	switch (font_alignment)
	{
	case FONT_ALIGNMENT_LEFT:
	{
		break;
	}
	case FONT_ALIGNMENT_MIDDLE:
	{
		x -= string_width / 2;
		break;
	}
	case FONT_ALIGNMENT_RIGHT:
	{
		x -= string_width;
		break;
	}
	}
	bitmap_t *character;
	size_t i;
	for (i = 0; i < strlen(string); ++i)
	{
		if ((character = font_character_to_bitmap(font, string[i])) != NULL)
		{
			x += FONT_LETTER_SPACING_FACTOR * height + font_show_character(font, character, height, x, y, color, shade);
		}
	}
}

void font_delete(font_t *font)
{
	size_t i;
	if (font->lower_case != NULL)
	{
		for (i = 0; i < FONT_NUM_LETTERS; ++i)
		{
			free(font->lower_case[i]);
		}
		free(font->lower_case);
		if (font->upper_case != NULL)
		{
			for (i = 0; i < FONT_NUM_LETTERS; ++i)
			{
				free(font->upper_case[i]);
			}
			free(font->upper_case);
			if (font->digit != NULL)
			{
				for (i = 0; i < FONT_NUM_DIGITS; ++i)
				{
					free(font->digit[i]);
				}
				free(font->digit);
				free(font->dot);
				free(font->exclamation_mark);
				free(font->question_mark);
				free(font->comma);
				free(font->quotation_mark);
				free(font->brackets_open);
				free(font->brackets_close);
				free(font->hyphen);
				free(font->colon);
				free(font->space);
			}
		}
	}
	free(font);
	font = NULL;
}

static bitmap_t *font_character_to_bitmap(font_t *font, char character)
{
	if (character >= 'a' && character <= 'z')
	{
		return font->lower_case[character - 'a'];
	}
	else if (character >= 'A' && character <= 'z')
	{
		return font->upper_case[character - 'A'];
	}
	else if (character >= '0' && character <= '9')
	{
		return font->digit[character - '0'];
	}
	switch (character)
	{
	case ' ': return font->space;
	case '.': return font->dot;
	case '!': return font->exclamation_mark;
	case '?': return font->question_mark;
	case ',': return font->comma;
	case ':': return font->colon;
	case '/': return font->right_slash;
	}
	return NULL;
}
