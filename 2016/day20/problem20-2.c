#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RANGES 1024

//#define LAST_IP 9
#ifndef LAST_IP
#define LAST_IP 4294967295
#endif

struct Range {
	uint64_t start;
	uint64_t end;
};

static void __destroy_ranges(struct Range **);
static uint64_t __find_allowed(struct Range **, size_t);
static int __sort_ranges(const void *, const void *);
static struct Range ** __combine_ranges(struct Range **, size_t *);
static void __remove_range(struct Range **, const size_t, const size_t);

int
main(const int argc, const char * const * const argv)
{
	if (argc != 2) {
		printf("Usage: %s <input file> <max IP>\n", argv[0]);
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

	const uint64_t allowed = __find_allowed(ranges, ranges_sz);
	printf("%" PRIu64 "\n", allowed);

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
__find_allowed(struct Range ** ranges, size_t ranges_sz)
{
	qsort(ranges, ranges_sz, sizeof(struct Range *), __sort_ranges);
	// printf("%zu ranges\n", ranges_sz);
	ranges = __combine_ranges(ranges, &ranges_sz);
	// printf("%zu ranges\n", ranges_sz);

	// for (size_t i = 0; i < ranges_sz; i++) {
	//	struct Range * const range = ranges[i];
	//	printf("have range %" PRIu64 " to %" PRIu64 "\n", range->start,
	// range->end);
	//}

	uint64_t allowed = 0;
	for (size_t i = 0; i < ranges_sz - 1; i++) {
		struct Range * const range0 = ranges[i];
		struct Range * const range1 = ranges[i + 1];

		if (range1->start > range0->end) {
			allowed += range1->start - range0->end - 1;
		}
		// printf("range0 %" PRIu64 " to %" PRIu64
		//		" range1 %" PRIu64 " to %" PRIu64 " (%" PRIu64 ")\n",
		//		range0->start, range0->end,
		//		range1->start, range1->end,
		//		allowed);
	}

	if (ranges[ranges_sz - 1]->end < LAST_IP) {
		allowed += LAST_IP - ranges[ranges_sz - 1]->end;
	}

	return allowed;
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

static struct Range **
__combine_ranges(struct Range ** ranges, size_t * ranges_sz)
{
	for (size_t i = 0; i < *ranges_sz - 1; i++) {
		struct Range * const range0 = ranges[i];

		while (i + 1 < *ranges_sz) {
			struct Range * const range1 = ranges[i + 1];

			if (range0->start == range1->start) {
				// printf("combining: %" PRIu64 " to %" PRIu64
				//		" and %" PRIu64 " to %" PRIu64 "\n", range0->start, range0->end,
				//		range1->start, range1->end);

				if (range1->end > range0->end) {
					range0->end = range1->end;
				}

				__remove_range(ranges, *ranges_sz, i + 1);
				*ranges_sz -= 1;
				continue;
			}

			if (range0->end > range1->start) {
				// printf("combining: %" PRIu64 " to %" PRIu64
				//		" and %" PRIu64 " to %" PRIu64 "\n", range0->start, range0->end,
				//		range1->start, range1->end);

				if (range1->end > range0->end) {
					range0->end = range1->end;
				}

				__remove_range(ranges, *ranges_sz, i + 1);
				*ranges_sz -= 1;
				continue;
			}

			break;
		}
	}

	return ranges;
}

static void
__remove_range(
		struct Range ** ranges, const size_t ranges_sz, const size_t index)
{
	free(ranges[index]);

	for (size_t i = index; i < ranges_sz - 1; i++) {
		ranges[i] = ranges[i + 1];
	}

	ranges[ranges_sz - 1] = NULL;
}
