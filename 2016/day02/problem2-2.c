#include <errno.h>
#include <stdio.h>
#include <string.h>

int __next_digit(const int, const char);

int
main(void)
{
	const char * const input_file = "input2-1.txt";

	FILE * const fh = fopen(input_file, "r");
	if (!fh) {
		printf("Unable to open file: %s: %s\n", input_file, strerror(errno));
		return 1;
	}

	int digit = 5;

	while (feof(fh) == 0) {
		char c = 0;
		int matched = fscanf(fh, "%c", &c);
		if (matched != 1) {
			break;
		}

		if (c == '\n') {
			if (digit < 10) {
				printf("%d", digit);
			} else {
				printf("%c", digit);
			}
			digit = 5;
			continue;
		}

		digit = __next_digit(digit, c);
		if (digit == -1) {
			printf("Unknown direction: %c\n", c);
			fclose(fh);
			return 1;
		}
	}

	if (fclose(fh) != 0) {
		printf("fclose(): %s\n", strerror(errno));
		return 1;
	}

	printf("\n");
	return 0;
}

__attribute__((const)) int
__next_digit(const int current_digit, const char direction)
{
	if (direction == 'U') {
		switch (current_digit) {
		case 1:
			return 1;
		case 2:
			return 2;
		case 3:
			return 1;
		case 4:
			return 4;
		case 5:
			return 5;
		case 6:
			return 2;
		case 7:
			return 3;
		case 8:
			return 4;
		case 9:
			return 9;
		case 'A':
			return 6;
		case 'B':
			return 7;
		case 'C':
			return 8;
		case 'D':
			return 'B';
		default:
			return -1;
		}
	}

	if (direction == 'D') {
		switch (current_digit) {
		case 1:
			return 3;
		case 2:
			return 6;
		case 3:
			return 7;
		case 4:
			return 8;
		case 5:
			return 5;
		case 6:
			return 'A';
		case 7:
			return 'B';
		case 8:
			return 'C';
		case 9:
			return 9;
		case 'A':
			return 'A';
		case 'B':
			return 'D';
		case 'C':
			return 'C';
		case 'D':
			return 'D';
		default:
			return -1;
		}
	}

	if (direction == 'R') {
		switch (current_digit) {
		case 1:
			return 1;
		case 2:
			return 3;
		case 3:
			return 4;
		case 4:
			return 4;
		case 5:
			return 6;
		case 6:
			return 7;
		case 7:
			return 8;
		case 8:
			return 9;
		case 9:
			return 9;
		case 'A':
			return 'B';
		case 'B':
			return 'C';
		case 'C':
			return 'C';
		case 'D':
			return 'D';
		default:
			return -1;
		}
	}

	if (direction == 'L') {
		switch (current_digit) {
		case 1:
			return 1;
		case 2:
			return 2;
		case 3:
			return 2;
		case 4:
			return 3;
		case 5:
			return 5;
		case 6:
			return 5;
		case 7:
			return 6;
		case 8:
			return 7;
		case 9:
			return 8;
		case 'A':
			return 'A';
		case 'B':
			return 'A';
		case 'C':
			return 'B';
		case 'D':
			return 'D';
		default:
			return -1;
		}
	}

	return -1;
}
