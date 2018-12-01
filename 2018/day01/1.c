#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>

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

		freq += atoi(buf);
	}

	printf("%d\n", freq);
	return 0;
}
