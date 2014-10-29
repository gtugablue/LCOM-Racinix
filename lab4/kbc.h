/** @defgroup kbc kbc
 * @{
 *
 * Functions for using the keyboard and mouse
 */

#include "i8042.h"

int kbc_subscribe_keyboard_int(unsigned char* hook_id);

int kbc_subscribe_mouse_int(unsigned char* hook_id);

int kbc_send_command(unsigned char command);

int kbc_read_response(unsigned char* response);

int kbc_unsubscribe_keyboard_int(unsigned char hook_id);

int kbc_unsubscribe_mouse_int(unsigned char hook_id);
