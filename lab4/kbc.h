/** @defgroup kbc kbc
 * @{
 *
 * Functions for using the keyboard and mouse
 */

#include "i8042.h"
//#include <minix/syslib.h>
//#include <minix/sysutil.h>
#include <minix/drivers.h>

int kbc_read_status(unsigned long* status);

int kbc_subscribe_keyboard_int(unsigned* hook_id);

int kbc_write(unsigned num_tries, unsigned char command);

int kbc_send_data(unsigned num_tries, unsigned char argument);

int kbc_write_to_mouse(unsigned num_tries);

int kbc_read(unsigned num_tries, unsigned long* output);

int kbc_clean_output_buffer(unsigned num_tries);

int kbc_unsubscribe_int(unsigned hook_id);
