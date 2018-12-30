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

	char buf[10240] = {0};

	int instr[2048] = {0};

	int i = 0;

	while (1) {
		if (fgets(buf, (int)sizeof(buf), fh) == NULL) {
			if (!feof(fh)) {
				fprintf(stderr, "fgets(): %s\n", strerror(errno));
				return 1;
			}
			break;
		}

		instr[i++] = atoi(buf);
	}

	int const count = i;

	int jumps = 0;
	i = 0;
	while (1) {
		int const next = instr[i];
		if (i + next >= count) {
			jumps++;
			break;
		}

		instr[i]++;
		jumps++;
		i += next;
	}

	printf("%d\n", jumps);
	return 0;
}
