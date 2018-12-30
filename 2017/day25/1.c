#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <map.h>
#include <queue.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 0

int
main(int const argc, char const * const * const argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <steps>\n", argv[0]);
		return 1;
	}
	uint64_t const steps = (uint64_t)atoll(argv[1]);

	size_t const sz = 100000;
	int64_t * const tape = calloc(sz, sizeof(int64_t));
	if (!tape) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		return 1;
	}

	int idx = sz / 2;

	char state = 'A';
	for (uint64_t i = 0; i < steps; i++) {
		if (DEBUG) {
			printf("state %c\n", state);
		}
		switch (state) {
		case 'A':
			if (tape[idx] == 0) {
				tape[idx] = 1;
				if (idx == sz - 1) {
					fprintf(stderr, "end of tape\n");
					free(tape);
					return 1;
				}
				idx++;
				state = 'B';
			} else {
				tape[idx] = 0;
				if (idx == 0) {
					fprintf(stderr, "end of tape (0)\n");
					free(tape);
					return 1;
				}
				idx--;
				state = 'C';
			}
			break;
		case 'B':
			if (tape[idx] == 0) {
				tape[idx] = 1;
				if (idx == 0) {
					fprintf(stderr, "end of tape (0)\n");
					free(tape);
					return 1;
				}
				idx--;
				state = 'A';
			} else {
				tape[idx] = 1;
				if (idx == sz - 1) {
					fprintf(stderr, "end of tape (right)\n");
					free(tape);
					return 1;
				}
				idx++;
				state = 'D';
			}
			break;
		case 'C':
			if (tape[idx] == 0) {
				tape[idx] = 0;
				if (idx == 0) {
					fprintf(stderr, "end of tape (0)\n");
					free(tape);
					return 1;
				}
				idx--;
				state = 'B';
			} else {
				tape[idx] = 0;
				if (idx == 0) {
					fprintf(stderr, "end of tape (left)\n");
					free(tape);
					return 1;
				}
				idx--;
				state = 'E';
			}
			break;
		case 'D':
			if (tape[idx] == 0) {
				tape[idx] = 1;
				if (idx == sz - 1) {
					fprintf(stderr, "end of tape (right)\n");
					free(tape);
					return 1;
				}
				idx++;
				state = 'A';
			} else {
				tape[idx] = 0;
				if (idx == sz - 1) {
					fprintf(stderr, "end of tape (right)\n");
					free(tape);
					return 1;
				}
				idx++;
				state = 'B';
			}
			break;
		case 'E':
			if (tape[idx] == 0) {
				tape[idx] = 1;
				if (idx == 0) {
					fprintf(stderr, "end of tape (left)\n");
					free(tape);
					return 1;
				}
				idx--;
				state = 'F';
			} else {
				tape[idx] = 1;
				if (idx == 0) {
					fprintf(stderr, "end of tape (left)\n");
					free(tape);
					return 1;
				}
				idx--;
				state = 'C';
			}
			break;
		case 'F':
			if (tape[idx] == 0) {
				tape[idx] = 1;
				if (idx == sz - 1) {
					fprintf(stderr, "end of tape (right)\n");
					free(tape);
					return 1;
				}
				idx++;
				state = 'D';
			} else {
				tape[idx] = 1;
				if (idx == sz - 1) {
					fprintf(stderr, "end of tape (right)\n");
					free(tape);
					return 1;
				}
				idx++;
				state = 'A';
			}
			break;
		default:
			fprintf(stderr, "unknown state: %c\n", state);
			free(tape);
			return 1;
		}
	}

	int checksum = 0;
	for (size_t i = 0; i < sz; i++) {
		if (tape[i]) {
			checksum++;
		}
	}

	printf("%d\n", checksum);
	free(tape);
	return 0;
}
