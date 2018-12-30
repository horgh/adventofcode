#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RANGES 1024

struct Range {
	uint64_t start;
	uint64_t end;
};

static void __destroy_ranges(struct Range **);
static uint64_t __find_lowest(struct Range **, const size_t);
static int __sort_ranges(const void *, const void *);

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

	struct Range ** const ranges = calloc(MAX_RANGES, sizeof(struct Range *));
	if (!ranges) {
		printf("%s\n", strerror(errno));
		fclose(fh);
		return 1;
	}

	size_t ranges_sz = 0;

	while (feof(fh) == 0) {
		char buf[1024];
		memset(buf, 0, sizeof(buf));
		if (fgets(buf, sizeof(buf), fh) == NULL) {
			break;
		}

		uint64_t start = 0;
		int matches = sscanf(buf, "%" PRIu64, &start);
		if (matches != 1) {
			printf("unexpected input: %s", buf);
			fclose(fh);
			__destroy_ranges(ranges);
			return 1;
		}

		char * ptr = buf;
		while (*ptr != '-' && *ptr != '\0') {
			ptr++;
		}

		if (*ptr != '-') {
			printf("unexpected input: %s", buf);
			fclose(fh);
			__destroy_ranges(ranges);
			return 1;
		}

		ptr++;

		uint64_t end = 0;
		matches = sscanf(ptr, "%" PRIu64, &end);
		if (matches != 1) {
			printf("unexpected input: %s", buf);
			fclose(fh);
			__destroy_ranges(ranges);
			return 1;
		}

		struct Range * const range = calloc(1, sizeof(struct Range));
		if (!range) {
			printf("%s\n", strerror(errno));
			fclose(fh);
			__destroy_ranges(ranges);
			return 1;
		}

		ranges[ranges_sz] = range;
		ranges_sz++;

		range->start = start;
		range->end = end;

		// printf("read range %" PRIu64 " to %" PRIu64 "\n", range->start,
		// range->end);
	}

	if (fclose(fh) != 0) {
		printf("fclose(): %s\n", strerror(errno));
		__destroy_ranges(ranges);
		return 1;
	}

	const uint64_t lowest = __find_lowest(ranges, ranges_sz);
	printf("%" PRIu64 "\n", lowest);

	__destroy_ranges(ranges);
	return 0;
}

static void
__destroy_ranges(struct Range ** ranges)
{
	if (!ranges) {
		return;
	}

	for (size_t i = 0; i < MAX_RANGES; i++) {
		if (!ranges[i]) {
			break;
		}

		free(ranges[i]);
	}

	free(ranges);
}

static uint64_t
__find_lowest(struct Range ** ranges, const size_t ranges_sz)
{
	qsort(ranges, ranges_sz, sizeof(struct Range *), __sort_ranges);

	uint64_t lowest = 0;
	for (size_t i = 0; i < ranges_sz; i++) {
		struct Range * const range = ranges[i];
		if (lowest >= range->start && lowest <= range->end) {
			lowest = range->end + 1;
		}
	}

	return lowest;
}

static int
__sort_ranges(const void * p1, const void * p2)
{
	const struct Range * const * rs1 = p1;
	const struct Range * const * rs2 = p2;
	const struct Range * r1 = rs1[0];
	const struct Range * r2 = rs2[0];

	if (r1->start < r2->start) {
		return -1;
	}

	if (r1->start > r2->start) {
		return 1;
	}

	if (r1->end < r2->end) {
		return -1;
	}

	if (r1->end > r2->end) {
		return 1;
	}

	return 0;
}
