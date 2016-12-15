#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DISCS 16

struct Disc {
	int number;
	int positions;
	int current_position;
};

static void
__destroy_discs(struct Disc * * const);
static void
__print_discs(struct Disc * * const);
static int
__solve(struct Disc * * const);
static struct Disc * *
__copy_discs(struct Disc * * const);
static bool
__drops_through(struct Disc * * const);
static void
__move_disc(struct Disc * const);

int
main(const int argc, const char * const * const argv)
{
	if (argc != 2) {
		printf("Usage: %s <input file>\n", argv[0]);
		return 1;
	}
	const char * const input_file = argv[1];

	FILE * const fh = fopen(input_file, "r");
	if (!fh) {
		printf("Unable to open file: %s: %s\n", input_file, strerror(errno));
		return 1;
	}

	struct Disc * * const discs = calloc(MAX_DISCS, sizeof(struct Disc *));
	if (!discs) {
		printf("%s\n", strerror(errno));
		fclose(fh);
		return 1;
	}

	size_t i = 0;

	while (feof(fh) == 0) {
		char buf[1024];
		memset(buf, 0, sizeof(buf));
		if (fgets(buf, sizeof(buf), fh) == NULL) {
			break;
		}

		struct Disc * const disc = calloc(1, sizeof(struct Disc));
		if (!disc) {
			fclose(fh);
			__destroy_discs(discs);
			return 1;
		}

		discs[i] = disc;
		i++;

		disc->number = (int) i-1;

		const char * ptr = buf;
		while (strncmp(ptr, "has ", 4) != 0) {
			ptr++;
		}
		ptr += 4;

		int matches = sscanf(ptr, "%d", &disc->positions);
		if (matches != 1) {
			printf("positions not found\n");
			fclose(fh);
			__destroy_discs(discs);
			return 1;
		}

		while (strncmp(ptr, "position ", strlen("position ")) != 0) {
			ptr++;
		}
		ptr += strlen("position ");

		matches = sscanf(ptr, "%d", &disc->current_position);
		if (matches != 1) {
			printf("current position not found\n");
			fclose(fh);
			__destroy_discs(discs);
			return 1;
		}
	}

	if (fclose(fh) != 0) {
		printf("fclose(): %s\n", strerror(errno));
		__destroy_discs(discs);
		return 1;
	}

	__print_discs(discs);

	const int t = __solve(discs);
	printf("%d\n", t);

	__destroy_discs(discs);

	return 0;
}

static void
__destroy_discs(struct Disc * * const discs)
{
	if (!discs) {
		return;
	}

	for (size_t i = 0; i < MAX_DISCS; i++) {
		if (!discs[i]) {
			break;
		}

		free(discs[i]);
	}

	free(discs);
}

static void
__print_discs(struct Disc * * const discs)
{
	for (size_t i = 0; i < MAX_DISCS; i++) {
		if (!discs[i]) {
			break;
		}

		const struct Disc * const disc = discs[i];

		printf("Disc %d: %d positions, current position %d\n", disc->number,
				disc->positions, disc->current_position);
	}
}

static int
__solve(struct Disc * * const discs)
{
	for (int t = 0; ; t++) {
		struct Disc * * const discs2 = __copy_discs(discs);
		if (!discs2) {
			printf("__copy_discs\n");
			return -1;
		}

		if (__drops_through(discs2)) {
			__destroy_discs(discs2);
			return t;
		}

		__destroy_discs(discs2);

		// Move all discs one position.
		for (size_t i = 0; i < MAX_DISCS; i++) {
			struct Disc * const disc = discs[i];
			if (!disc) {
				break;
			}
			__move_disc(disc);
		}
	}

	return -1;
}

static struct Disc * *
__copy_discs(struct Disc * * const discs)
{
	struct Disc * * const discs2 = calloc(MAX_DISCS, sizeof(struct Disc *));
	if (!discs2) {
		printf("%s\n", strerror(errno));
		return NULL;
	}

	for (size_t i = 0; i < MAX_DISCS; i++) {
		struct Disc * const disc = discs[i];
		if (!disc) {
			continue;
		}

		struct Disc * const disc2 = calloc(1, sizeof(struct Disc));
		if (!disc2) {
			printf("%s\n", strerror(errno));
			__destroy_discs(discs2);
			return NULL;
		}

		discs2[i] = disc2;

		disc2->number = disc->number;
		disc2->positions = disc->positions;
		disc2->current_position = disc->current_position;
	}

	return discs2;
}

static bool
__drops_through(struct Disc * * const discs)
{
	// We drop the ball now.

	// Drop for MAX_DISCS seconds.
	for (size_t i = 0; i < MAX_DISCS; i++) {
		// Advance all discs each second.
		for (size_t j = 0; j < MAX_DISCS; j++) {
			struct Disc * const disc = discs[j];
			if (!disc) {
				break;
			}
			__move_disc(disc);
		}

		// We reach one.
		struct Disc * const disc = discs[i];
		if (!disc) {
			break;
		}

		if (disc->current_position != 0) {
			return false;
		}
	}

	return true;
}

static void
__move_disc(struct Disc * const disc)
{
	if (disc->current_position == disc->positions-1) {
		disc->current_position = 0;
	} else {
		disc->current_position++;
	}
}
