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

/** @defgroup context_menu Context Menu
 * @{
 *
 * Module that makes the background darker and shows a context menu with clickable options.
 *
 */

/** @name Click location info */
/** @{ */
enum context_menu_click_location
{
	CONTEXT_MENU_CLICK_BACKGROUND = -2, //!< click outside the context menu
	CONTEXT_MENU_CLICK_NO_BUTTON = -1 //!< click inside the context menu but on no option
};
/** @} end of click location info */

/** @name Context menu structure */
/** @{
 *
 * Context menu structure
 */
typedef struct
{
	const unsigned char **items;
	unsigned num_items;
	uint16_t *background; //!< For efficiency purposes
	font_t *font;
} context_menu_t;
/** @} end of context_menu_t */

/**
 * @brief Context menu constructor
 *
 * @param items array of strings with the text to show in each menu item
 * @param num_items number of items to be shown in the menu
 * @param vbe_mode_info pointer to VBE block
 * @param font pointer to the font to be used in the items text
 * @return A pointer to the new context menu on success, NULL otherwise
 */
context_menu_t *context_menu_create(const unsigned char **items, unsigned num_items, vbe_mode_info_t *vbe_mode_info, font_t *font);

/**
 * @brief Click handler
 *
 * Checks where the user is clicking (in the background, in the context menu or in an option)
 *
 * @param context_menu pointer to the context menu
 * @param x x coordinate of the mouse
 * @param y y coordinate of the mouse
 * @param vbe_mode_info pointer to VBE block
 * @return The ID of the button clicked or the ::context_menu_click_location if no button was clicked
 */
int context_menu_click(context_menu_t *context_menu, unsigned x, unsigned y, vbe_mode_info_t *vbe_mode_info);

/**
 * @brief Draws the context menu
 *
 * @param context_menu pointer to the context menu
 * @param mouse_position location of the mouse
 * @param vbe_mode_info pointer to VBE block
 */
void context_menu_draw(context_menu_t *context_menu, vector2D_t mouse_position, vbe_mode_info_t *vbe_mode_info);

/**
 * @brief Click handler
 *
 * Frees the memory allocated by the context menu constructor
 *
 * @param context_menu pointer to the context menu
 * @param mouse_position location of the mouse
 * @param vbe_mode_info pointer to VBE block
 */
void context_menu_delete(context_menu_t *context_menu);

/** @} end of Context Menu */

#endif
