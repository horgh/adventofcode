#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	int total_mass = 0;
	while (1) {
		char buf[256] = {0};
		if (fgets(buf, sizeof(buf), stdin) == NULL) {
			break;
		}
		int const mass = atoi(buf);
		total_mass += mass/3-2;
	}

	printf("%d\n", total_mass);
	return 0;
}
