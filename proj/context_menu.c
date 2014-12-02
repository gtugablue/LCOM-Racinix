#include "context_menu.h"
#include <stdlib.h>

static int context_menu_background_lose_focus(context_menu_t *context_menu, vbe_mode_info_t *vbe_mode_info);

context_menu_t *context_menu_create(char **items, unsigned num_items, vbe_mode_info_t *vbe_mode_info)
{
	// TODO
	context_menu_t *context_menu;
	if ((context_menu = malloc(sizeof(context_menu_t))) == NULL)
	{
		return NULL;
	}
	if ((context_menu->background = malloc(vbe_mode_info->XResolution * vbe_mode_info->YResolution * vbe_mode_info->BitsPerPixel / 8)) == NULL)
	{
		context_menu_delete(context_menu);
		return NULL;
	}
	if (context_menu_background_lose_focus(context_menu, vbe_mode_info))
	{
		return NULL;
	}
	context_menu->items = items;
	context_menu->num_items = num_items;
	return context_menu;
}

int context_menu_click(context_menu_t *context_menu, unsigned x, unsigned y)
{
	// TODO
	return CONTEXT_MENU_CLICK_BACKGROUND;
}

void context_menu_draw(context_menu_t *context_menu, vbe_mode_info_t *vbe_mode_info)
{
	// TODO
	uint16_t *double_buffer = vg_get_double_buffer();
	memcpy(double_buffer, context_menu->background, vbe_mode_info->XResolution * vbe_mode_info->YResolution * vbe_mode_info->BitsPerPixel / 8);
	vg_draw_rectangle((vbe_mode_info->XResolution - CONTEXT_MENU_WIDTH) / 2, 0, CONTEXT_MENU_WIDTH, vbe_mode_info->YResolution, CONTEXT_MENU_COLOR);
}

static int context_menu_background_lose_focus(context_menu_t *context_menu, vbe_mode_info_t *vbe_mode_info)
{
	uint16_t *backup_buffer;
	if ((backup_buffer = malloc(vbe_mode_info->XResolution * vbe_mode_info->YResolution * vbe_mode_info->BitsPerPixel)) == NULL)
	{
		return 1;
	}
	uint16_t *double_buffer = vg_get_double_buffer();
	memcpy(backup_buffer, double_buffer, vbe_mode_info->XResolution * vbe_mode_info->YResolution * vbe_mode_info->BitsPerPixel / 8);

	size_t x, y;
	int i, j;
	unsigned r, g, b;
	uint16_t color;
	for (x = 0; x < vbe_mode_info->XResolution; ++x)
	{
		for (y = 0; y < vbe_mode_info->YResolution; ++y)
		{
			r = g = b = 0;
			/*for (i = 0; i < 1; ++i) // Change start and end values to blur the background but don't forget to paint the borders first (removed because the result is not very good in 16-bit mode)
			{
				for (j = 0; j < 1; ++j)
				{
					color = *(backup_buffer + (x + i) + (y + j) * vbe_mode_info->XResolution);
					r += (color >> vbe_mode_info->RedFieldPosition) & ((1 << vbe_mode_info->RedMaskSize) - 1);
					g += (color >> vbe_mode_info->GreenFieldPosition) & ((1 << vbe_mode_info->GreenMaskSize) - 1);
					b += (color >> vbe_mode_info->BlueFieldPosition) & ((1 << vbe_mode_info->BlueMaskSize) - 1);
				}
			}*/
			color = *(backup_buffer + x + y * vbe_mode_info->XResolution);
			r += (color >> vbe_mode_info->RedFieldPosition) & ((1 << vbe_mode_info->RedMaskSize) - 1);
			g += (color >> vbe_mode_info->GreenFieldPosition) & ((1 << vbe_mode_info->GreenMaskSize) - 1);
			b += (color >> vbe_mode_info->BlueFieldPosition) & ((1 << vbe_mode_info->BlueMaskSize) - 1);
			*(context_menu->background + x + y * vbe_mode_info->XResolution) = ((unsigned)(CONTEXT_MENU_BACKGROUND_COLOR_PCT * r) << vbe_mode_info->RedFieldPosition) | ((unsigned)(CONTEXT_MENU_BACKGROUND_COLOR_PCT * g) << vbe_mode_info->GreenFieldPosition) | ((unsigned)(CONTEXT_MENU_BACKGROUND_COLOR_PCT * b) << vbe_mode_info->BlueFieldPosition);
		}
	}
	free(backup_buffer);
	return 0;
}

void context_menu_delete(context_menu_t *context_menu)
{
	free(context_menu->items);
	free(context_menu->background);
	free(context_menu);
}
