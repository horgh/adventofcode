#include <errno.h>
#include <stdio.h>
#include <string.h>

int
main(void)
{
	const char * const input_file = "input3-1.txt";

	FILE * const fh = fopen(input_file, "r");
	if (!fh) {
		printf("Unable to open file: %s: %s\n", input_file, strerror(errno));
		return 1;
	}

	int possible = 0;

	while (feof(fh) == 0) {
		int a = 0, b = 0, c = 0;
		int matched = fscanf(fh, "%d %d %d\n", &a, &b, &c);
		if (matched != 3) {
			break;
		}

		if (a + b <= c) {
			continue;
		}
		if (a + c <= b) {
			continue;
		}
		if (b + c <= a) {
			continue;
		}

		possible++;
	}

	if (fclose(fh) != 0) {
		printf("fclose(): %s\n", strerror(errno));
		return 1;
	}

	printf("%d\n", possible);

	return 0;
}
