#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

bool
__valid_triangle(const int, const int, const int);

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
		int a0 = 0, b0 = 0, c0 = 0;
		int matched = fscanf(fh, "%d %d %d\n", &a0, &b0, &c0);
		if (matched != 3) {
			break;
		}

		int a1 = 0, b1 = 0, c1 = 0;
		matched = fscanf(fh, "%d %d %d\n", &a1, &b1, &c1);
		if (matched != 3) {
			break;
		}

		int a2 = 0, b2 = 0, c2 = 0;
		matched = fscanf(fh, "%d %d %d\n", &a2, &b2, &c2);
		if (matched != 3) {
			break;
		}

		if (__valid_triangle(a0, a1, a2)) {
			possible++;
		}
		if (__valid_triangle(b0, b1, b2)) {
			possible++;
		}
		if (__valid_triangle(c0, c1, c2)) {
			possible++;
		}
	}

	if (fclose(fh) != 0) {
		printf("fclose(): %s\n", strerror(errno));
		return 1;
	}

	printf("%d\n", possible);

	return 0;
}

__attribute__((const))
bool
__valid_triangle(const int a, const int b, const int c)
{
	if (a + b <= c) {
		return false;
	}
	if (a + c <= b) {
		return false;
	}
	if (b + c <= a) {
		return false;;
	}
	return true;
}
