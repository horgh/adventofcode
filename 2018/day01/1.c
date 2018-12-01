#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <map.h>
#include <queue.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	int freq = 0;
	while (1) {
		char buf[4096] = {0};
		if (fgets(buf, 4096, stdin) == NULL) {
			break;
		}

		char const * ptr = buf;
		while (*ptr == '+') {
			ptr++;
		}

		int const n = atoi(buf);
		freq += n;
	}

	printf("%d\n", freq);
	return 0;
}
