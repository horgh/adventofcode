#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void
reverse(int * const, int const, int const, int const);
static void
print_list(int const * const, int const);

int main(int const argc, char const * const * const argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <list size>\n", argv[0]);
		return 1;
	}
	int const list_size = atoi(argv[1]);

	FILE * const fh = stdin;

	char buf[20480] = {0};

	if (fgets(buf, (int) sizeof(buf), fh) == NULL) {
		fprintf(stderr, "fgets(): %s\n", strerror(errno));
		return 1;
	}

	int instructions[256] = {0};
	int instruction_count = 0;
	char const *ptr = buf;
	while (*ptr != '\0' && *ptr != '\n') {
		instructions[instruction_count] = atoi(ptr);
		instruction_count++;

		while (isdigit(*ptr)) {
			ptr++;
		}

		if (*ptr == ',') {
			ptr++;
		}
	}

	int * const list = calloc((size_t) list_size, sizeof(int));
	if (!list) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		return 1;
	}

	for (int i = 0; i < list_size; i++) {
		list[i] = i;
	}

	int idx = 0;
	int skip_size = 0;
	if (0) {
		print_list(list, list_size);
	}
	for (int i = 0; i < instruction_count; i++) {
		int const length = instructions[i];

		reverse(list, list_size, idx, length);

		if (0) {
			print_list(list, list_size);
		}

		idx += length+skip_size;
		if (idx >= list_size) {
			idx = idx-list_size;
		}
		skip_size++;
	}

	printf("%d\n", list[0]*list[1]);
	free(list);
	return 0;
}

static void
reverse(int * const list, int const list_size, int const idx, int const length)
{
	for (int i = 0; i < length/2; i++) {
		int idx0 = idx+i;
		if (idx0 >= list_size) {
			idx0 = idx0-list_size;
		}
		int idx1 = idx+length-1-i;
		if (idx1 >= list_size) {
			idx1 = idx1-list_size;
		}

		int const tmp = list[idx0];
		list[idx0] = list[idx1];
		list[idx1] = tmp;
	}
}

static void
print_list(int const * const list, int const list_size)
{
	for (int i = 0; i < list_size; i++) {
		if (i == 0) {
			printf("%d", list[i]);
			continue;
		}
		printf(",%d", list[i]);
	}
	printf("\n");
}
