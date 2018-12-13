#define _POSIX_C_SOURCE 200809L

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

struct Note {
	bool pattern[5];
	bool plant;
};

static void
print_state(bool const * const);

static void
print_note(struct Note const * const);

static bool
grows(bool const * const,
		int const,
		struct Note const * const,
		size_t const);

#define SZ 300
#define MIDPOINT 150

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	bool state[SZ] = {0};
	size_t n_states = MIDPOINT;
	struct Note notes[64] = {0};
	int line = 0;
	size_t n_notes = 0;
	while (1) {
		char buf[4096] = {0};
		if (fgets(buf, 4096, stdin) == NULL) {
			break;
		}
		trim_right(buf);

		if (line == 1) {
			line++;
			continue;
		}

		char const * ptr = buf;

		if (line == 0) {
			ptr += strlen("initial state: ");
			while (*ptr != '\0') {
				if (*ptr == '#') {
					state[n_states] = true;
				}
				n_states++;
				ptr++;
			}
			line++;
			continue;
		}

		size_t i = 0;
		while (*ptr != ' ') {
			if (*ptr == '#') {
				notes[n_notes].pattern[i] = true;
			}
			i++;
			ptr++;
		}
		ptr += strlen(" => ");
		if (*ptr == '#') {
			notes[n_notes].plant = true;
		}
		n_notes++;
		line++;
	}

	if (0) {
		printf("initial state: ");
		print_state(state);
		printf("\n");

		for (size_t i = 0; i < n_notes; i++) {
			print_note(&notes[i]);
		}
	}

	for (int i = 0; i < 20; i++) {
		if (false) {
			printf("%2d: ", i);
			print_state(state);
		}
		bool next_state[SZ] = {0};
		for (int j = 0; j < SZ; j++) {
			if (j-2 < 0) {
				continue;
			}
			if (j+2 >= SZ) {
				continue;
			}
			if (!grows(state, j, notes, n_notes)) {
				continue;
			}
			next_state[j] = true;
		}
		memcpy(state, next_state, SZ);
	}

	int sum = 0;
	for (size_t j = 0; j < SZ; j++) {
		if (!state[j]) {
			continue;
		}
		sum += (int) j-MIDPOINT;
	}
	printf("%d\n", sum);
	return 0;
}

static void
print_state(bool const * const state)
{
	for (size_t i = 0; i < SZ; i++) {
		if (state[i]) {
			printf("#");
		} else {
			printf(".");
		}
	}
	printf("\n");
}

static void
print_note(struct Note const * const note)
{
	for (size_t j = 0; j < 5; j++) {
		if (note->pattern[j]) {
			printf("#");
		} else {
			printf(".");
		}
	}
	printf(" => ");
	if (note->plant) {
		printf("#\n");
	} else {
		printf(".\n");
	}
}

static bool
grows(bool const * const state,
		int const pos,
		struct Note const * const notes,
		size_t const n_notes)
{
	for (size_t i = 0; i < n_notes; i++) {
		if (!notes[i].plant) {
			continue;
		}
		if (state[pos-2] == notes[i].pattern[0] &&
				state[pos-1] == notes[i].pattern[1] &&
				state[pos]   == notes[i].pattern[2] &&
				state[pos+1] == notes[i].pattern[3] &&
				state[pos+2] == notes[i].pattern[4]) {
			return true;
		}
	}
	return false;
}
