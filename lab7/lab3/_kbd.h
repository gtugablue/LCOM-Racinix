#include	"i8042.h"

#define IS_BREAK_CODE(scancode)			((scancode) & (1 << I8042_BREAK_CODE_BIT))
#define TOGGLE_LED(led, status)			((status) ^ BIT(led))

#define KBC_TRIES			10
#define KBD_HOOK_BIT		2
#define TWO_BYTE_SCANCODE	0xE0

#define KEY_ESC				0x1
