#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map.h>
#include <unistd.h>

enum Direction { North, East, South, West, Unknown };

enum Direction
__decide_new_direction(const enum Direction, const char);
bool
__visit(const struct htable * const, const int, const int);
bool
__have_visited(const struct htable * const, const int, const int);

int
main(const int argc, const char * const * const argv)
{
	(void) argc;
	(void) argv;

	const char * const input_file = "input1.txt";

	int x = 0;
	int y = 0;

	enum Direction current_direction = North;

	FILE * const fh = fopen(input_file, "r");
	if (!fh) {
		printf("Unable to open file: %s: %s\n", input_file, strerror(errno));
		return 1;
	}

	struct htable * locations = hash_init(1024);
	if (!locations) {
		printf("Unable to allocate hashtable\n");
		fclose(fh);
		return 1;
	}

	int * visited = calloc(1, sizeof(int));
	if (!visited) {
		printf("%s\n", strerror(errno));
		fclose(fh);
		hash_free(locations, free);
		return 1;
	}

	// Record we visited 0,0
	if (!hash_set(locations, "0,0", visited)) {
		printf("Unable to visit 0,0\n");
		fclose(fh);
		return 1;
	}

	while (feof(fh) == 0) {
		char dir = 0;
		int steps = 0;

		int matched = fscanf(fh, "%c%d, ", &dir, &steps);
		printf("matched %d (%c %d)\n", matched, dir, steps);
		if (matched != 2) {
			break;
		}

		current_direction = __decide_new_direction(current_direction, dir);

		switch (current_direction) {
			case North:
				for (int i = 0; i < steps; i++) {
					x++;
					if (__have_visited(locations, x, y)) {
						goto DONE;
					}
					__visit(locations, x, y);
				}
				break;
			case East:
				for (int i = 0; i < steps; i++) {
					y++;
					if (__have_visited(locations, x, y)) {
						goto DONE;
					}
					__visit(locations, x, y);
				}
				break;
			case South:
				for (int i = 0; i < steps; i++) {
					x--;
					if (__have_visited(locations, x, y)) {
						goto DONE;
					}
					__visit(locations, x, y);
				}
				break;
			case West:
				for (int i = 0; i < steps; i++) {
					y--;
					if (__have_visited(locations, x, y)) {
						goto DONE;
					}
					__visit(locations, x, y);
				}
				break;
			case Unknown:
			default:
				printf("Unknown direction\n");
				fclose(fh);
				hash_free(locations, free);
				return 1;
		}
	}

DONE:

	hash_free(locations, free);

	if (fclose(fh) != 0) {
		printf("fclose(): %s\n", strerror(errno));
		return 1;
	}

	const int steps = abs(x) + abs(y);
	printf("Steps: %d\n", steps);

	return 0;
}

__attribute__((const))
enum Direction
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

bool
__have_visited(const struct htable * const locations, const int x, const int y)
{
	char key[1024];
	memset(key, 0, sizeof(key));
	snprintf(key, sizeof(key), "%d,%d", x, y);

	return hash_get(locations, key) != NULL;
}

bool
__visit(const struct htable * const locations, const int x, const int y)
{
	char key[1024];
	memset(key, 0, sizeof(key));
	snprintf(key, sizeof(key), "%d,%d", x, y);

	int * visited = calloc(1, sizeof(int));
	if (!visited) {
		printf("%s\n", strerror(errno));
		return false;
	}

	return hash_set(locations, key, visited);
}
