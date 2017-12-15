#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static uint64_t
calc_next(uint64_t const,
		uint64_t const,
		uint64_t const);

int
main(int const argc, char const * const * const argv)
{
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <A start> <B start>\n", argv[0]);
		return 1;
	}

	uint64_t const a_start = (uint64_t) atoi(argv[1]);
	uint64_t const b_start = (uint64_t) atoi(argv[2]);

	uint64_t const a_factor = 16807;
	uint64_t const b_factor = 48271;

	uint64_t const divisor = 2147483647;

	uint64_t a_previous = a_start;
	uint64_t b_previous = b_start;

	int const n = 40000000;
	int count = 0;
	for (int i = 0; i < n; i++) {
		uint64_t const a_next = calc_next(a_previous, a_factor, divisor);
		uint64_t const b_next = calc_next(b_previous, b_factor, divisor);

		if (0) {
			printf("a: %" PRIu64 " b: %" PRIu64 "\n", a_next, b_next);
		}

		if ((a_next & 0xffff) == (b_next & 0xffff)) {
			count++;
		}

		a_previous = a_next;
		b_previous = b_next;
	}

	printf("%d\n", count);
	return 0;
}

static uint64_t
calc_next(uint64_t const previous_value,
		uint64_t const factor,
		uint64_t const divisor)
{
	return (previous_value*factor)%divisor;
}
