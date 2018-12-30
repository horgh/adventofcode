#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>

int
main(int const argc, char const * const * const argv)
{
	(void)argc;
	(void)argv;

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

		int const face_0 = 2 * l + 2 * w;
		int const face_1 = 2 * w + 2 * h;
		int const face_2 = 2 * h + 2 * l;
		int const volume = l * w * h;

		if (face_0 <= face_1 && face_0 <= face_2) {
			feet += face_0;
		} else if (face_1 <= face_0 && face_1 <= face_2) {
			feet += face_1;
		} else {
			feet += face_2;
		}

		feet += volume;
	}

	printf("%d\n", feet);
	return 0;
}
