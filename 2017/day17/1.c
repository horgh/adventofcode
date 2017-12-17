#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SZ 4096

static int
step(int const,
		int * const,
		int const,
		int * const,
		int const);
static void
print_buf(int const * const, int const);

int
main(int const argc, char const * const * const argv)
{
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <steps> <times>\n", argv[0]);
		return 1;
	}
	int const steps = atoi(argv[1]);
	int const times = atoi(argv[2]);

	int buf[SZ] = {0};
	int tmpbuf[SZ] = {0};

	int bufsz = 1;
	int pos = 0;

	for (int i = 0; i < times; i++) {
		if (0) {
			printf("i = %d, pos = %d, bufsz = %d: ", i, pos, bufsz);
			print_buf(buf, bufsz);
		}

		pos = step(steps, buf, bufsz, tmpbuf, pos);
		bufsz++;
	}

	printf("%d\n", buf[pos+1]);
	return 0;
}

static int
step(int const steps,
		int * const buf,
		int const bufsz,
		int * const tmpbuf,
		int const pos)
{
	int newpos = pos;
	for (int i = 0; i < steps; i++) {
		newpos++;
		if (newpos == bufsz) {
			newpos = 0;
		}
	}

	newpos++;

	memcpy(tmpbuf, buf, sizeof(int)*(size_t) bufsz);

	buf[newpos] = bufsz;

	if (newpos == bufsz) {
		// Nothing to copy
		return newpos;
	}

	int const to_copy = bufsz-newpos;
	memcpy(buf+newpos+1, tmpbuf+newpos, sizeof(int)*(size_t)to_copy);
	return newpos;
}

static void
print_buf(int const * const buf, int const bufsz)
{
	for (size_t i = 0; i < (size_t) bufsz; i++) {
		if (i == 0) {
			printf("%d", buf[i]);
			continue;
		}
		printf(" %d", buf[i]);
	}
	printf("\n");
}
