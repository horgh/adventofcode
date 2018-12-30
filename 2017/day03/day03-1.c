#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(const int argc, const char * const * const argv)
{
	(void)argc;
	(void)argv;

	FILE * const fh = stdin;

	char buf[10240] = {0};

	if (fgets(buf, (int)sizeof(buf), fh) == NULL) {
		fprintf(stderr, "fgets(): %s\n", strerror(errno));
		return 1;
	}

	// Number of squares is a perfect square. Must be odd so that 1 is in the
	// centre.
	//
	// Bottom right is the number of squares. Bottom left is that minus side
	// length, etc.

	int const square = atoi(buf);

	int side_length = 0;
	int num_squares = 0;
	while (1) {
		num_squares = side_length * side_length;
		if (num_squares >= square && num_squares % 2 != 0) {
			break;
		}

		side_length++;
	}

	int const bottom_right = num_squares;
	int const bottom_left = bottom_right - side_length + 1;
	int const top_left = bottom_left - side_length + 1;
	int const top_right = top_left - side_length + 1;

	int steps_horizontal = 0;
	int steps_vertical = 0;
	if (square >= bottom_left) {
		// Bottom side.
		int const mid = bottom_right - (side_length / 2);
		steps_horizontal = abs(mid - square);
		steps_vertical = side_length / 2;
	} else if (square >= top_left) {
		// Left side.
		int const mid = bottom_left - (side_length / 2);
		steps_horizontal = side_length / 2;
		steps_vertical = abs(mid - square);
	} else if (square >= top_right) {
		// Top side.
		int const mid = top_left - (side_length / 2);
		steps_horizontal = abs(mid - square);
		steps_vertical = side_length / 2;
	} else {
		// Right side.
		int const mid = top_right - (side_length / 2);
		steps_horizontal = side_length / 2;
		steps_vertical = abs(mid - square);
	}

	int const steps = steps_horizontal + steps_vertical;
	printf("%d\n", steps);

	return 0;
}
