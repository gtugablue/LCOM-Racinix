int kbd_test_scan(unsigned short ass); {
	/* To be completed */
}
int kbd_test_leds(unsigned short n, unsigned short *leds) {
    /* To be completed */
}
int kbd_test_timed_scan(unsigned short n) {
    /* To be completed */
}

int kbd_subscribe_int()
{
	int sys_inb(port_t port, unsigned long *byte);
}
