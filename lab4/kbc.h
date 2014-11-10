/** @defgroup kbc kbc
 * @{
 *
 * Functions for using the keyboard and mouse
 */

#include "i8042.h"
#include <minix/drivers.h>

#define KBC_NUM_TRIES					10

int kbc_read_status(unsigned long* status);

int kbc_subscribe_keyboard_int(unsigned* hook_id);

int kbc_write(unsigned char command);

int kbc_send_data(unsigned char argument);

int kbc_write_to_mouse();

int kbc_read(unsigned long* output);

int kbc_clean_output_buffer();

int kbc_unsubscribe_int();
