#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <minix/drivers.h>
#include "i8042.h"
#include <stdbool.h>

#define IS_BREAK_CODE(scancode)			((scancode) & (1 << I8042_BREAK_CODE_BIT))
#define TOGGLE_LED(led, status)			((status) ^ BIT(led))

#define KBC_TRIES				10
#define KEYBOARD_HOOK_BIT		2
#define TWO_BYTE_SCANCODE		0xE0

/** @defgroup keyboard keyboard
 * @{
 *
 * Functions for using the keyboard
 */

/**
 * @brief Checks if a given interruption is a response to a command
 *
 * @params interruption data read from the output buffer after the
 *  interruption occured
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int keyboard_is_response(unsigned long interruption);

/**
 * @brief Subscribe the keyboard interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int keyboard_subscribe_int();

/**
 * @brief Toggle a keyboard led status (1 = on; 0 = off)
 *
 * @param led
 * Bit 2 - Caps Lock indicator
 * Bit 1 - Numeric Lock indicator
 * Bit 0 - Scroll Lock indicator
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int keyboard_toggle_leds(unsigned short leds);

/**
 * @brief Read the status of the KBC
 *
 * @param status memory address to write the 4-byte status at
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int keyboard_read_status(unsigned long* status);

/**
 * @brief Handle keyboard interrupts
 *
 * Reads the scancode from the OUT_BUF
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int keyboard_int_handler();

/**
 * @brief Unsubscribe the keyboard interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int keyboard_unsubscribe_int();

/**
 * @brief Returns the LEDs status
 *
 * @return Returns the LEDs status
 */
unsigned char keyboard_get_led_status();

// Keys
typedef struct
{
	unsigned long makecode;
	bool pressed;
} kbd_key_t;

kbd_key_t kbd_keys[102];

enum {
    KEY_NONE,
    KEY_ESC,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_0,
    KEY_APOSTROPHE,
    KEY_ANGLE_QUOTES,
    KEY_BKSP,
    KEY_TAB,
    KEY_Q,
    KEY_W,
    KEY_E,
    KEY_R,
    KEY_T,
    KEY_Y,
    KEY_U,
    KEY_I,
    KEY_O,
    KEY_P,
    KEY_PLUS,
    KEY_ACCENT,
    KEY_ENTER,
    KEY_L_CTRL,
    KEY_A,
    KEY_S,
    KEY_D,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_C_CEDILLA,
    KEY_ORDINAL,
    KEY_BACKSLASH,
    KEY_L_SHIFT,
    KEY_TILDE,
    KEY_Z,
    KEY_X,
    KEY_C,
    KEY_V,
    KEY_B,
    KEY_N,
    KEY_M,
    KEY_COMMA,
    KEY_DOT,
    KEY_MINUS,
    KEY_R_SHIFT,
    KEY_ALT,
    KEY_SPACE,
    KEY_CAPS,
    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_NUM,
    KEY_SCRLL,
    KEY_NUM_7,
    KEY_NUM_8,
    KEY_NUM_9,
    KEY_NUM_MINUS,
    KEY_NUM_4,
    KEY_NUM_5,
    KEY_NUM_6,
    KEY_NUM_PLUS,
    KEY_NUM_1,
    KEY_NUM_2,
    KEY_NUM_3,
    KEY_NUM_0,
    KEY_NUM_DEL,
    KEY_MINOR,
    KEY_F11,
    KEY_F12,
    KEY_NUM_ENTER,
    KEY_R_CTRL,
    KEY_NUM_SLASH,
    KEY_ALT_GR,
    KEY_HOME,
    KEY_ARR_UP,
    KEY_PGUP,
    KEY_ARR_LEFT,
    KEY_ARR_RIGHT,
    KEY_ARR_DOWN,
    KEY_PGDN,
    KEY_INS,
    KEY_DEL,
    KEY_WIN,
    KEY_CNTX,
    KEY_END
};

/*enum {
    KEY_NONE = 0x0000,
    KEY_ESC = 0x0001,
    KEY_1 = 0x0002,
    KEY_2 = 0x0003,
    KEY_3 = 0x0004,
    KEY_4 = 0x0005,
    KEY_5 = 0x0006,
    KEY_6 = 0x0007,
    KEY_7 = 0x0008,
    KEY_8 = 0x0009,
    KEY_9 = 0x000A,
    KEY_0 = 0x000B,
    KEY_APOSTROPHE = 0x000C,
    KEY_ANGLE_QUOTES = 0x000D,
    KEY_BKSP = 0x000E,
    KEY_TAB = 0x000F,
    KEY_Q = 0x0010,
    KEY_W = 0x0011,
    KEY_E = 0x0012,
    KEY_R = 0x0013,
    KEY_T = 0x0014,
    KEY_Y = 0x0015,
    KEY_U = 0x0016,
    KEY_I = 0x0017,
    KEY_O = 0x0018,
    KEY_P = 0x0019,
    KEY_PLUS = 0x001A,
    KEY_ACCENT = 0x001B,
    KEY_ENTER = 0x001C,
    KEY_L_CTRL = 0x001D,
    KEY_A = 0x001E,
    KEY_S = 0x001F,
    KEY_D = 0x0020,
    KEY_F = 0x0021,
    KEY_G = 0x0022,
    KEY_H = 0x0023,
    KEY_J = 0x0024,
    KEY_K = 0x0025,
    KEY_L = 0x0026,
    KEY_C_CEDILLA = 0x0027,
    KEY_ORDINAL = 0x0028,
    KEY_BACKSLASH = 0x0029,
    KEY_L_SHIFT = 0x002A,
    KEY_TILDE = 0x002B,
    KEY_Z = 0x002C,
    KEY_X = 0x002D,
    KEY_C = 0x002E,
    KEY_V = 0x002F,
    KEY_B = 0x0030,
    KEY_N = 0x0031,
    KEY_M = 0x0032,
    KEY_COMMA = 0x0033,
    KEY_DOT = 0x0034,
    KEY_MINUS = 0x0035,
    KEY_R_SHIFT = 0x0036,
    KEY_ALT = 0x0038,
    KEY_SPACE = 0x0039,
    KEY_CAPS = 0x003A,
    KEY_F1 = 0x003B,
    KEY_F2 = 0x003C,
    KEY_F3 = 0x003D,
    KEY_F4 = 0x003E,
    KEY_F5 = 0x003F,
    KEY_F6 = 0x0040,
    KEY_F7 = 0x0041,
    KEY_F8 = 0x0042,
    KEY_F9 = 0x0043,
    KEY_F10 = 0x0044,
    KEY_NUM = 0x0045,
    KEY_SCRLL = 0x0046,
    KEY_NUM_7 = 0x0047,
    KEY_NUM_8 = 0x0048,
    KEY_NUM_9 = 0x0049,
    KEY_NUM_MINUS = 0x004A,
    KEY_NUM_4 = 0x004B,
    KEY_NUM_5 = 0x004C,
    KEY_NUM_6 = 0x004D,
    KEY_NUM_PLUS = 0x004E,
    KEY_NUM_1 = 0x004F,
    KEY_NUM_2 = 0x0050,
    KEY_NUM_3 = 0x0051,
    KEY_NUM_0 = 0x0052,
    KEY_NUM_DEL = 0x0053,
    KEY_MINOR = 0x0056,
    KEY_F11 = 0x0057,
    KEY_F12 = 0x0058,
    KEY_NUM_ENTER = 0xE01C,
    KEY_R_CTRL = 0xE01D,
    KEY_NUM_SLASH = 0xE035,
    KEY_ALT_GR = 0xE038,
    KEY_HOME = 0xE047,
    KEY_ARR_UP = 0xE048,
    KEY_PGUP = 0xE049,
    KEY_ARR_LEFT = 0xE04B,
    KEY_ARR_RIGHT = 0xE04D,
    KEY_ARR_DOWN = 0xE050,
    KEY_PGDN = 0xE051,
    KEY_INS = 0xE052,
    KEY_DEL = 0xE053,
    KEY_WIN = 0xE05B,
    KEY_CNTX = 0xE05D,
    KEY_END = 0xE04F
};*/

#endif
