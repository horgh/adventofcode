#define _POSIX_C_SOURCE 200809L

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

struct Note {
	char pattern[5];
	bool plant;
};

static void print_state(char const * const);

static void print_note(struct Note const * const);

static bool grows(
		char const * const, int const, struct Note const * const, size_t const);

#define SZ 2048
#define SZ2 SZ + 1
#define MIDPOINT SZ / 2

int
main(int const argc, char const * const * const argv)
{
	(void)argc;
	(void)argv;
	uint64_t const rounds = 50000000000;

	char state[SZ2] = {0};
	for (size_t i = 0; i < SZ; i++) {
		state[i] = '.';
	}
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
					state[n_states] = '#';
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
				notes[n_notes].pattern[i] = '#';
			} else {
				notes[n_notes].pattern[i] = '.';
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

	uint64_t interesting_round = 0;
	char last_pattern[SZ2] = {0};
	for (uint64_t i = 0; i < rounds; i++) {
		if (false) {
			printf("%2" PRIu64 ": ", i);
			print_state(state);
		}
		char next_state[SZ2] = {0};
		for (size_t j = 0; j < SZ; j++) {
			next_state[j] = '.';
		}
		for (int j = 0; j < SZ; j++) {
			if (j - 2 < 0) {
				continue;
			}
			if (j + 2 >= SZ) {
				continue;
			}
			if (!grows(state, j, notes, n_notes)) {
				continue;
			}
			next_state[j] = '#';
		}

		char pattern[SZ2] = {0};
		char const * ptr = strchr(next_state, '#');
		char const * eptr = strrchr(next_state, '#');
		int j = 0;
		while (ptr != eptr + 1) {
			pattern[j++] = *ptr;
			ptr++;
		}
		pattern[j++] = *ptr;
		if (memcmp(pattern, last_pattern, SZ) == 0) {
			interesting_round = i;
			break;
		}
		memcpy(last_pattern, pattern, SZ);
		memcpy(state, next_state, SZ);
	}

	uint64_t sum = 0;
	for (uint64_t j = 0; j < SZ; j++) {
		if (state[j] == '.') {
			continue;
		}
		sum += j - MIDPOINT + (rounds - interesting_round);
	}
	printf("%" PRIu64 "\n", sum);
	return 0;
}

static void
print_state(char const * const state)
{
	for (size_t i = 0; i < SZ; i++) {
		printf("%c", state[i]);
	}
	printf("\n");
}

static void
print_note(struct Note const * const note)
{
	for (size_t j = 0; j < 5; j++) {
		printf("%c", note->pattern[j]);
	}
	printf(" => ");
	if (note->plant) {
		printf("#\n");
	} else {
		printf(".\n");
	}
}

static bool
grows(char const * const state,
		int const pos,
		struct Note const * const notes,
		size_t const n_notes)
{
	for (size_t i = 0; i < n_notes; i++) {
		if (!notes[i].plant) {
			continue;
		}
		if (state[pos - 2] == notes[i].pattern[0] &&
				state[pos - 1] == notes[i].pattern[1] &&
				state[pos] == notes[i].pattern[2] &&
				state[pos + 1] == notes[i].pattern[3] &&
				state[pos + 2] == notes[i].pattern[4]) {
			return true;
		}
	}
	return false;
}
