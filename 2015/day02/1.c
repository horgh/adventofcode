#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	int feet = 0;
	while (1) {
		char buf[8192] = {0};
		if (fgets(buf, 8192, stdin) == NULL) {
			break;
		}

		char const * ptr = buf;

		int const l = atoi(ptr);
		while (*ptr != 'x') {
			ptr++;
		}
		ptr++;

		int const w = atoi(ptr);
		while (*ptr != 'x') {
			ptr++;
		}
		ptr++;

		int const h = atoi(ptr);

		int const top_bottom = l*w;
		int const side_0 = w*h;
		int const side_1 = h*l;

		feet += 2*top_bottom+2*side_0+2*side_1;

		if (top_bottom <= side_0 && top_bottom <= side_1) {
			feet += top_bottom;
		} else if (side_0 <= top_bottom && side_0 <= side_1) {
			feet += side_0;
		} else {
			feet += side_1;
		}
	}

	printf("%d\n", feet);
	return 0;
}
