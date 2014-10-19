#ifndef _I8042_H
#define _I8042_H

#define I8042_TIMEOUT_US	20000

#define BIT(n) (0x01<<(n))

#define I8042_IRQ					1		// Keyboard IRQ Line
#define I8042_HOOK_BIT				2

#define I8042_KBD_TIMEOUT			20000	// Wait time in microseconds

// KBC registers
#define I8042_STAT_REG				0x64
#define I8042_IN_BUF				0x64
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

#define i8042_KEYCODE_T_MAX_NAME	30

struct i8042_keycode_t
{
	unsigned position;
	unsigned scancode;
	char* name;
} i8042_keycodes[] = {
		1,	0x29,	"`~",
		2,	0x02,	"1!",
		3,	0x03,	"2@",
		4,	0x04,	"3#",
		5,	0x05,	"4$",
		6,	0x06,	"5%E",
		7,	0x07,	"6^",
		8,	0x08,	"7&",
		9,	0x09,	"8*",
		10,	0x0a,	"9(",
		11,	0x0b,	"0)",
		12,	0x0c,	"-_",
		13,	0x0d,	"=+",
		15,	0x0e,	"Backspace",

		16,	0x0f,	"Tab",
		17,	0x10,	"Q",
		18,	0x11,	"W",
		19,	0x12,	"E",
		20,	0x13,	"R",
		21,	0x14,	"T",
		22,	0x15,	"Y",
		23,	0x16,	"U",
		24,	0x17,	"I",
		25,	0x18,	"O",
		26,	0x19,	"P",
		27,	0x1a,	"[{",
		28,	0x1b,	"]}",
		29,	0x2b,	"\\|",

		30,	0x3a,	"CapsLock",
		31,	0x1e,	"A",
		32,	0x1f,	"S",
		33,	0x20,	"D",
		34,	0x21,	"F",
		35,	0x22,	"G",
		36,	0x23,	"H",
		37,	0x24,	"J",
		38,	0x25,	"K",
		39,	0x26,	"L",
		40,	0x27,	";:",
		41,	0x28,	"'\"",
		42,	0,	"non-US-1",
		43,	0x1c,	"Enter",

		44,	0x2a,	"LShift",
		46,	0x2c,	"Z",
		47,	0x2d,	"X",
		48,	0x2e,	"C",
		49,	0x2f,	"V",
		50,	0x30,	"B",
		51,	0x31,	"N",
		52,	0x32,	"M",
		53,	0x33,	",<",
		54,	0x34,	".>",
		55,	0x35,	"/?",
		57,	0x36,	"RShift",

		58,	0x1d,	"LCtrl",
		60,	0x38,	"LAlt",
		61,	0x39,		"space",
		62,	0xe038,	"RAlt",
		64,	0xe01d,	"RCtrl",

		75,	0xe052,	"Insert",
		76,	0xe053,	"Delete",
		80,	0xe047,	"Home",
		81,	0xe04f,	"End",
		85,	0xe049,	"PgUp",
		86,	0xe051,	"PgDn",

		79,	0xe04b,	"Left",
		83,	0xe048,	"Up",
		84,	0xe050,	"Down",
		89,	0xe04d,	"Right",

		90,	0x45,	"NumLock",
		91,	0x47,	"KP-7 / Home",
		92,	0x4b,	"KP-4 / Left",
		93,	0x4f,	"KP-1 / End",
		95,	0xe035,	"KP-/",
		96,	0x48,	"KP-8 / Up",
		97,	0x4c,	"KP-5",
		98,	0x50,	"KP-2",
		99,	0x52,	"KP-0 / Ins",
		100, 	0x37,	"KP-*",
		101, 	0x49,	"KP-9",
		102,	0x4d,	"KP-6 / Right",
		103,	0x51,	"KP-3 / PgDn",
		104,	0x53,	"KP-. / Del",
		105,	0x4a,	"KP--",
		106,	0x4e,	"KP-+",
		108,	0xe01c,	"KP-Enter",

		110,	0x01,	"Esc",
		112,	0x3b,	"F1",
		113,	0x3c,	"F2",
		114,	0x3d,	"F3",
		115,	0x3e,	"F4",
		116,	0x3f,	"F5",
		117,	0x40,	"F6",
		118,	0x41,	"F7", /* Vojtech has 0x02 in set2 */
		119,	0x42,	"F8",
		120,	0x43,	"F9",
		121,	0x44,	"F10",
		122,	0x57,	"F11",
		123,	0x58,	"F12",

		124,	0xe037,	"PrtScr",
		0,		0x54,	"Alt+SysRq",
		125,	0x46,	"ScrollLock",
		126,	0xe11d45,	"Pause",
		0,	0xe046,"Ctrl+Break",

		/* Microsoft Windows and Internet keys and Power keys */
		0,	0xe05b,	"LWin (USB: LGUI)",
		0,	0xe05c,	"RWin (USB: RGUI)",
		0,	0xe05d,		"Menu",

		0,	0xe06a,	"Back",
		0,	0xe069,	"Forward",
		0,	0xe068,	"Stop",
		0,	0xe06c,	"Mail",
		0,	0xe065,	"Search",
		0,	0xe066,	"Favorites",
		0,	0xe032,	"Web / Home",

		0,	0xe06b,	"My Computer",
		0,	0xe021,	"Calculator",
		0,	0xe05f,	"Sleep",
		0,	0xe05e, "Power",
		0,	0xe063,	"Wake",
};

#endif
