#include "kbc.h"

int mouse_subscribe_int(unsigned *hook_id);

int mouse_write(unsigned num_tries, unsigned char command);

int mouse_send_argument(unsigned num_tries, unsigned char argument);

int mouse_unsubscribe_int(unsigned hook_id);
