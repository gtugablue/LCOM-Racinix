#ifndef _CONTEXT_MENU_H
#define _CONTEXT_MENU_H

#include "video_gr.h"
#include "vbe.h"
#include "font.h"
#include "vector2D.h"

#define CONTEXT_MENU_WIDTH							400
#define CONTEXT_MENU_COLOR							rgb(125, 125, 125)
#define CONTEXT_MENU_BACKGROUND_COLOR_PCT			0.20
#define CONTEXT_MENU_CHAR_HEIGHT					30

enum
{
	CONTEXT_MENU_CLICK_BACKGROUND = -2,
	CONTEXT_MENU_CLICK_NO_BUTTON = -1
};

typedef struct
{
	char **items;
	unsigned num_items;
	uint16_t *background; // For efficiency purposes
	font_t *font;
} context_menu_t;

context_menu_t *context_menu_create(char **items, unsigned num_items, vbe_mode_info_t *vbe_mode_info, font_t *font);

int context_menu_click(context_menu_t *context_menu, unsigned x, unsigned y, vbe_mode_info_t *vbe_mode_info);

void context_menu_draw(context_menu_t *context_menu, vbe_mode_info_t *vbe_mode_info);

void context_menu_delete(context_menu_t *context_menu);

#endif
