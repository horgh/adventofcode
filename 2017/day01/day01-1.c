#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(const int argc, const char * const * const argv)
{
	(void)argc;
	(void)argv;

	FILE * const fh = stdin;

	int first = -1;
	int prev = -1;
	int sum = 0;
	int i = 0;
	while (!feof(fh)) {
		char buf[2] = {0};
		if (fread(buf, sizeof(char), 1, fh) != 1) {
			if (feof(fh)) {
				break;
			}
			fprintf(stderr, "fread(): %s\n", strerror(errno));
			return 1;
		}

		if (buf[0] < '0' || buf[0] > '9') {
			break;
		}

		int const x = atoi(buf);

		if (i == 0) {
			first = x;
			prev = x;
			i++;
			continue;
		}

		if (x == prev) {
			sum += x;
		}

		prev = x;
		i++;
	}

	if (prev == first) {
		sum += prev;
	}

	printf("%d\n", sum);

	return 0;
}
