#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define SZ 61000000

static uint32_t
step(uint32_t const,
		uint32_t * const,
		uint32_t const,
		uint32_t const);
static void
print_buf(uint32_t const * const, uint32_t const);

int
main(int const argc, char const * const * const argv)
{
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <steps> <times>\n", argv[0]);
		return 1;
	}
	uint32_t const steps = (uint32_t) atoi(argv[1]);
	uint32_t const times = (uint32_t) atoll(argv[2]);

	uint32_t * const buf = calloc(SZ, sizeof(uint32_t));
	if (!buf) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		return 1;
	}

	uint32_t bufsz = 1;
	uint32_t pos = 0;

	for (uint32_t i = 0; i < times; i++) {
		if (0) {
			printf("i = %" PRIu32 ", pos = %" PRIu32 ", bufsz = %" PRIu32 ": ", i,
					pos, bufsz);
			print_buf(buf, bufsz);
		}

		pos = step(steps, buf, bufsz, pos);
		bufsz++;
	}

	printf("%" PRIu32 "\n", buf[1]);
	free(buf);
	return 0;
}

static uint32_t
step(uint32_t const steps,
		uint32_t * const buf,
		uint32_t const bufsz,
		uint32_t const pos)
{
	uint32_t newpos = pos+steps;
	newpos %= bufsz;

	newpos++;
	buf[newpos] = bufsz;
	return newpos;
}

static void
print_buf(uint32_t const * const buf, uint32_t const bufsz)
{
	for (uint32_t i = 0; i < bufsz; i++) {
		if (i == 0) {
			printf("%" PRIu32, buf[i]);
			continue;
		}
		printf(" %" PRIu32, buf[i]);
	}
	printf("\n");
}
