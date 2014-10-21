#ifndef _I8042_H
#define _I8042_H

#define I8042_TIMEOUT_US	20000

#define BIT(n) (0x01<<(n))

#define I8042_IRQ					1		// Keyboard IRQ Line

#define I8042_KBD_TIMEOUT			20000	// Wait time in microseconds

// KBC registers
#define I8042_STAT_REG				0x64
#define I8042_IN_KBC_CMD_BUF		0x64
#define I8042_IN_KBD_CMD_BUF		0x60
#define I8042_IN_ARG_BUF			0x60
#define I8042_OUT_BUF				0x60

// C@KBD responses
#define I8042_ACK					0xFA
#define I8042_RESEND				0xFE	// The latest byte should be written again
#define I8042_ERROR					0xFC	// The entire input/output sequence should be restarted

// Keyboard Commands
#define I8042_RESET_KBD				0xFF
#define I8042_DEFAULT_ENABLE_KBD	0xF6
#define	I8042_DISABLE_KBD			0xF5
#define I8042_CLEAR_BUF_ENABLE_KBD	0xF4
#define I8042_CHANGE_KBD_REPETITION	0xF3
#define I8042_SWITCH_KBD_LEDS		0xED

// LEDs
#define I8042_LED_CAPSLOCK_BIT		2
#define I8042_LED_NUMLOCK_BIT		1
#define I8042_LED_SCROLL_BIT		0

// KBC Status Register
#define I8042_STATUS_PARITY_BIT		7
#define I8042_STATUS_TIMEOUT_BIT	6
#define I8042_STATUS_AUX_BIT		5
#define I8042_STATUS_INH_BIT		4
#define I8042_STATUS_A2_BIT			3		// Used to distinguish KBC commands from KBD commands
#define I8042_STATUS_SYS_BIT		2
#define I8042_STATUS_IBF_BIT		1
#define I8042_STATUS_OBF_BIT		0

// KBC Commands
#define I8042_READ_COMMAND_BYTE		0x20
#define I8042_WRITE_COMMAND_BYTE	0x60
#define I8042_CHECK_KBC				0xAA
#define I8042_CHECK_KEYBOARD_ITF	0xAB
#define I8042_DISABLE_KBD_ITF		0xAD
#define I8042_ENABLE_KBD_ITF		0xAE

// KBC Self-test result
#define I8042_CHECK_KBC_OK			0x55
#define I8042_CHECK_KBC_ERROR		0xFC

// KBC Command Byte
#define I8042_CMD_BYTE_TRANSLATE	6
#define I8042_CMD_BYTE_DIS_MOUSE	5
#define I8042_CMD_BYTE_DIS_KEYBOARD	4
#define I8042_CMD_BYTE_IGNORE_LOCK	3
#define I8042_CMD_BYTE_SYSTEM_FLAG	2
#define I8042_CMD_BYTE_MOUSE_INT	1
#define I8042_CMD_BYTE_KEYBOARD_INT	0

#define I8042_BREAK_CODE_BIT		7
#define I8042_SCANCODE_EXTRA_BYTE	0xE0

#define i8042_KEYCODE_T_MAX_NAME	30

/// Keys
typedef enum {
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
} KEY;

#endif
