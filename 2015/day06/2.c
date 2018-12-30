#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum Directive { On, Toggle, Off };

struct Instruction {
	enum Directive directive;
	int start_x;
	int start_y;
	int end_x;
	int end_y;
};

int
main(int const argc, char const * const * const argv)
{
	(void)argc;
	(void)argv;

	struct Instruction instructions[512] = {0};
	size_t n = 0;
	while (1) {
		char buf[8192] = {0};
		if (fgets(buf, 8192, stdin) == NULL) {
			break;
		}

		if (strncmp(buf, "turn on", strlen("turn on")) == 0) {
			instructions[n].directive = On;
		} else if (strncmp(buf, "toggle", strlen("toggle")) == 0) {
			instructions[n].directive = Toggle;
		} else if (strncmp(buf, "turn off", strlen("turn off")) == 0) {
			instructions[n].directive = Off;
		} else {
			assert(1 == 0);
		}

		char const * ptr = buf;

		while (!isdigit(*ptr)) {
			ptr++;
		}
		instructions[n].start_x = atoi(ptr);
		while (*ptr != ',') {
			ptr++;
		}
		ptr++;
		instructions[n].start_y = atoi(ptr);

		while (isdigit(*ptr)) {
			ptr++;
		}

		while (!isdigit(*ptr)) {
			ptr++;
		}
		instructions[n].end_x = atoi(ptr);
		while (*ptr != ',') {
			ptr++;
		}
		ptr++;
		instructions[n].end_y = atoi(ptr);

		n++;
	}

#ifdef DEBUG
	for (size_t i = 0; i < n; i++) {
		printf("Instruction: ");
		switch (instructions[i].directive) {
		case On:
			printf("On ");
			break;
		case Toggle:
			printf("Toggle ");
			break;
		case Off:
			printf("Off ");
			break;
		default:
			assert(1 == 0);
		}
		printf("%d,%d to %d,%d\n",
				instructions[i].start_x,
				instructions[i].start_y,
				instructions[i].end_x,
				instructions[i].end_y);
	}
#endif

	int map[1000][1000] = {0};
	for (size_t i = 0; i < n; i++) {
		struct Instruction instr = instructions[i];
		for (int x = instr.start_x; x <= instr.end_x; x++) {
			for (int y = instr.start_y; y <= instr.end_y; y++) {
				switch (instr.directive) {
				case On:
					map[x][y]++;
					break;
				case Toggle:
					map[x][y] += 2;
					break;
				case Off:
					if (map[x][y] > 0) {
						map[x][y]--;
					}
					break;
				default:
					assert(1 == 0);
				}
			}
		}
	}

	int count = 0;
	for (size_t x = 0; x < 1000; x++) {
		for (size_t y = 0; y < 1000; y++) {
			count += map[x][y];
		}
	}

	printf("%d\n", count);
	return 0;
}
