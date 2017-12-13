#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DOWN 0
#define UP   1

struct scanner {
	int range;
	int depth;
	int index;
	int direction;
};

static void
scanners_destroy(struct scanner * * const, size_t const);
static void
parse_line(char const * const,
		struct scanner * * const,
		size_t * const);
static void
scanners_print(struct scanner * * const,
		size_t const);
static struct scanner * *
scanners_copy(struct scanner * * const,
		size_t const);
static void
scanners_copy_shallow(struct scanner * * const,
		struct scanner * * const,
		size_t const);
static int
run(struct scanner * * const, size_t const);
static void
move_scanners(struct scanner * * const, size_t const);

int main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	FILE * const fh = stdin;

	char buf[41960] = {0};

	struct scanner * * const scanners = calloc(128, sizeof(struct scanner *));
	if (!scanners) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		return 1;
	}

	size_t num_scanners = 0;
	while (1) {
		if (fgets(buf, (int) sizeof(buf), fh) == NULL) {
			if (!feof(fh)) {
				fprintf(stderr, "fgets(): %s\n", strerror(errno));
				scanners_destroy(scanners, num_scanners);
				return 1;
			}
			break;
		}

		parse_line(buf, scanners, &num_scanners);
	}

	if (0) {
		scanners_print(scanners, num_scanners);
	}

	struct scanner * * const scanners2 = scanners_copy(scanners, num_scanners);
	if (!scanners2) {
		scanners_destroy(scanners, num_scanners);
		return 1;
	}

	int t = 0;
	while (1) {
		scanners_copy_shallow(scanners2, scanners, num_scanners);

		int const severity = run(scanners2, num_scanners);
		if (severity == 0) {
			break;
		}

		move_scanners(scanners, num_scanners);
		t++;
	}

	scanners_destroy(scanners, num_scanners);
	scanners_destroy(scanners2, num_scanners);

	printf("%d\n", t);
	return 0;
}

static void
scanners_destroy(struct scanner * * const scanners, size_t const num_scanners)
{
	for (size_t i = 0; i < num_scanners; i++) {
		struct scanner * const sc = scanners[i];
		if (!sc) {
			continue;
		}
		free(sc);
	}

	free(scanners);
}

static void
parse_line(char const * const line,
		struct scanner * * const scanners,
		size_t * const num_scanners)
{
	const char * ptr = line;

	struct scanner * const sc = calloc(1, sizeof(struct scanner));
	if (!sc) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		return;
	}

	sc->depth = atoi(ptr);

	while (isdigit(*ptr)) {
		ptr++;
	}

	while (*ptr == ':' || *ptr == ' ') {
		ptr++;
	}

	sc->range = atoi(ptr);

	while ((int) *num_scanners < sc->depth) {
		*num_scanners += 1;
	}

	scanners[*num_scanners] = sc;

	*num_scanners += 1;
}

static void
scanners_print(struct scanner * * const scanners,
		size_t const num_scanners)
{
	for (size_t i = 0; i < num_scanners; i++) {
		struct scanner const * const sc = scanners[i];
		printf("depth: %zu: ", i);
		if (!sc) {
			printf("no scanner\n");
			continue;
		}
		printf("scanner range %d, index %d\n", sc->range, sc->index);
	}
}

static struct scanner * *
scanners_copy(struct scanner * * const scanners,
		size_t const num_scanners)
{
	struct scanner * * const scanners_copy = calloc(num_scanners,
			sizeof(struct scanner *));
	if (!scanners_copy) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		return NULL;
	}

	for (size_t i = 0; i < num_scanners; i++) {
		struct scanner * const sc0 = scanners[i];
		if (!sc0) {
			continue;
		}

		struct scanner * const sc1 = calloc(1, sizeof(struct scanner));
		if (!sc1) {
			fprintf(stderr, "calloc(): %s\n", strerror(errno));
			scanners_destroy(scanners_copy, num_scanners);
			return NULL;
		}

		sc1->range     = sc0->range;
		sc1->depth     = sc0->depth;
		sc1->index     = sc0->index;
		sc1->direction = sc0->direction;

		scanners_copy[i] = sc1;
	}

	return scanners_copy;
}

static void
scanners_copy_shallow(struct scanner * * const scanners_target,
		struct scanner * * const scanners_source,
		size_t const num_scanners)
{
	for (size_t i = 0; i < num_scanners; i++) {
		struct scanner * const sc0 = scanners_source[i];
		if (!sc0) {
			continue;
		}

		struct scanner * const sc1 = scanners_target[i];
		sc1->range     = sc0->range;
		sc1->depth     = sc0->depth;
		sc1->index     = sc0->index;
		sc1->direction = sc0->direction;
	}
}

static int
run(struct scanner * * const scanners, size_t const num_scanners)
{
	for (size_t depth = 0; depth < num_scanners; depth++) {
		struct scanner * const sc = scanners[depth];
		if (sc && sc->index == 0) {
			return 1;
		}

		move_scanners(scanners, num_scanners);
	}

	return 0;
}

static void
move_scanners(struct scanner * * const scanners, size_t const num_scanners)
{
	for (size_t i = 0; i < num_scanners; i++) {
		struct scanner * const sc = scanners[i];
		if (!sc) {
			continue;
		}

		if (sc->direction == DOWN) {
			if (sc->index+1 == sc->range) {
				sc->index--;
				sc->direction = UP;
				continue;
			}
			sc->index++;
			continue;
		}

		if (sc->index-1 == -1) {
			sc->index++;
			sc->direction = DOWN;
			continue;
		}
		sc->index--;
	}
}
