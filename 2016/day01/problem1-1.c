#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum Direction { North, East, South, West, Unknown };

enum Direction __decide_new_direction(const enum Direction, const char);

int
main(const int argc, const char * const * const argv)
{
	(void)argc;
	(void)argv;

	const char * const input_file = "input1.txt";

	int x = 0;
	int y = 0;

	enum Direction current_direction = North;

	FILE * const fh = fopen(input_file, "r");
	if (!fh) {
		printf("Unable to open file: %s: %s\n", input_file, strerror(errno));
		return 1;
	}

	while (feof(fh) == 0) {
		char dir = 0;
		int steps = 0;

		int matched = fscanf(fh, "%c%d, ", &dir, &steps);
		// printf("matched %d (%c %d)\n", matched, dir, steps);
		if (matched != 2) {
			break;
		}

		current_direction = __decide_new_direction(current_direction, dir);

		switch (current_direction) {
		case North:
			x += steps;
			break;
		case East:
			y += steps;
			break;
		case South:
			x -= steps;
			break;
		case West:
			y -= steps;
			break;
		case Unknown:
		default:
			printf("Unknown direction\n");
			fclose(fh);
			return 1;
		}
	}

	if (fclose(fh) != 0) {
		printf("fclose(): %s\n", strerror(errno));
		return 1;
	}

	const int steps = abs(x) + abs(y);
	printf("%d\n", steps);

	return 0;
}

__attribute__((const)) enum Direction
__decide_new_direction(const enum Direction current_direction, const char dir)
{
	if (dir == 'R') {
		switch (current_direction) {
		case North:
			return East;
		case East:
			return South;
		case South:
			return West;
		case West:
			return North;
		case Unknown:
		default:
			return Unknown;
		}

		return Unknown;
	}

	switch (current_direction) {
	case North:
		return West;
	case East:
		return North;
	case South:
		return East;
	case West:
		return South;
	case Unknown:
	default:
		return Unknown;
	}

	return Unknown;
}
