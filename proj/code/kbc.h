/** @defgroup kbc kbc
 * @{
 *
 * Functions for using the keyboard and mouse
 */

#include "i8042.h"
//#include <minix/syslib.h>
//#include <minix/sysutil.h>
#include <minix/drivers.h>

/**
 * @brief Read the kbc status
 *
 * @param status status of the kbc
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int kbc_read_status(unsigned long* status);

/**
 * @brief Subscribes the keyboard interrupts
 *
 * @param hook_id the memory address where the hook_id will be stored
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int kbc_subscribe_keyboard_int(unsigned* hook_id);

/**
 * @brief Write to the kbc
 *
 * @param num_tries number of tries whenever something fails
 * @param command command that will be sent to kbc
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int kbc_write(unsigned num_tries, unsigned char command);

/**
 * @brief Sends the data from kbc
 *
 * @param num_tries number of tries whenever something fails
 * @param command command that will be sent to kbc
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int kbc_send_data(unsigned num_tries, unsigned char argument);

/**
 * @brief Writes to the mouse
 *
 * @param num_tries number of tries whenever something fails
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int kbc_write_to_mouse(unsigned num_tries);

/**
 * @brief Read from kbc
 *
 * @param num_tries number of tries whenever something fails
 * @param output address where will be writing what is read
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int kbc_read(unsigned num_tries, unsigned char* output);

/**
 * @brief Cleans the outpur buffer
 *
 * @param num_tries number of tries whenever something fails
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int kbc_clean_output_buffer(unsigned num_tries);

/**
 * @brief Funtion that waits until the in_buffer is clean
 *
 * @param num_tries number of tries whenever something fails
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int kbc_wait_for_in_buf(unsigned num_tries);

/**
 * @brief Funtion that waits until the out_buffer has content to be read
 *
 * @param num_tries number of tries whenever something fails
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int kbc_wait_for_out_buf(unsigned num_tries);

/**
 * @brief Unsubscribe the kbc interrupts
 *
 * @param hook_id the memory address where the hook_id will be stored
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int kbc_unsubscribe_int(unsigned hook_id);
