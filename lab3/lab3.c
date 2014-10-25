#include <minix/drivers.h>

#include "test3.h"

static int proc_args(int argc, char *argv[]);
static unsigned long parse_ulong(char *str, int base);
static long parse_long(char *str, int base);
static void print_usage(char *argv[]);

int main(int argc, char **argv) {

	sef_startup();

	/* Enable IO-sensitive operations for ourselves */
	//sys_iopenable(SELF);

	if ( argc == 1 ) {
		print_usage(argv);
		return 0;
	} else {
		proc_args(argc, argv);
	}

	return 0;

}

static void print_usage(char *argv[]) {
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"scan <ass>\" \n"
			"\t service run %s -args \"leds <leds>\" \n"
			"\t service run %s -args \"timedscan <n>\" \n" ,
			argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[]) {

	unsigned long ass, n;
	unsigned char* leds;

	/* check the function to test: if the first characters match, accept it */
	if (strncmp(argv[1], "scan", strlen("scan")) == 0) {
		if( argc != 3 ) {
			printf("kbd: wrong no of arguments for test of kbd_test_scan() \n");
			return 1;
		}
		if( (ass = parse_ulong(argv[2], 10)) == ULONG_MAX )
			return 1;
		printf("kbd:: kbd_test_scan(%lu)\n",
				ass);
		return kbd_test_scan(ass);
	} else if (strncmp(argv[1], "leds", strlen("leds")) == 0) {
		if( argc <= 2 ) {
			printf("kbd: wrong no of arguments for test of kbd_test_leds() \n");
			return 1;
		}

		n = argc - 2;
		if ((leds = malloc(n * sizeof(char))) == NULL)
		{
			return 1;
		}
		size_t i;
		for (i = 0; i < n; ++i)
		{
			if( (leds[i] = parse_ulong(argv[i + 2], 10)) == ULONG_MAX )
				return 1;
		}
		printf("kbd:: kbd_test_leds(%lu, leds)\n",
				n);
		return kbd_test_leds(n, leds);
	} else if (strncmp(argv[1], "timedscan", strlen("timedscan")) == 0) {
		if( argc != 3 ) {
			printf("kbd: wrong no of arguments for test of kbd_test_timed_scan() \n");
			return 1;
		}
		if( (n = parse_ulong(argv[2], 10)) == ULONG_MAX )
			return 1;
		printf("kbd:: kbd_test_timed_scan(%lu)\n",
				n);
		return kbd_test_timed_scan(n);
	} else {
		printf("kbd: non valid function \"%s\" to test\n", argv[1]);
		return 1;
	}
}

static unsigned long parse_ulong(char *str, int base) {
	char *endptr;
	unsigned long val;

	val = strtoul(str, &endptr, base);

	if ((errno == ERANGE && val == ULONG_MAX )
			|| (errno != 0 && val == 0)) {
		perror("strtol");
		return ULONG_MAX;
	}

	if (endptr == str) {
		printf("kbd: parse_ulong: no digits were found in %s \n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}

static long parse_long(char *str, int base) {
	char *endptr;
	unsigned long val;

	val = strtol(str, &endptr, base);

	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
			|| (errno != 0 && val == 0)) {
		perror("strtol");
		return LONG_MAX;
	}

	if (endptr == str) {
		printf("kbd: parse_long: no digits were found in %s \n", str);
		return LONG_MAX;
	}

	/* Successful conversion */
	return val;
}
