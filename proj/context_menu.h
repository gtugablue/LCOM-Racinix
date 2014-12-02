#ifndef _CONTEXT_MENU_H
#define _CONTEXT_MENU_H

#include "video_gr.h"
#include "vbe.h"

#define CONTEXT_MENU_WIDTH							400
#define CONTEXT_MENU_COLOR							rgb(125, 125, 125)
#define CONTEXT_MENU_BACKGROUND_COLOR_PCT			0.20

enum
{
	CONTEXT_MENU_CLICK_BACKGROUND,
	CONTEXT_MENU_CLICK_NO_BUTTON
};

typedef struct
{
	char **items;
	unsigned num_items;
	uint16_t *background; // For efficiency purposes
} context_menu_t;

context_menu_t *context_menu_create(char **items, unsigned num_items, vbe_mode_info_t *vbe_mode_info);

int context_menu_click(context_menu_t *context_menu, unsigned x, unsigned y);

void context_menu_draw(context_menu_t *context_menu, vbe_mode_info_t *vbe_mode_info);

void context_menu_delete(context_menu_t *context_menu);

#endif
