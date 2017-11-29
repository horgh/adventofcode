#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Letter {
	int c;
	int count;
};

static int
__get_valid_sector_id(const char * const);
static bool
__valid_checksum(struct Letter *, const char * const);
#ifdef DEBUG
static void
__print_letters(const struct Letter * const);
#endif
static int
__letter_cmp(const void *, const void *);

int
main(void)
{
	const char * const input_file = "input4-1.txt";

	FILE * const fh = fopen(input_file, "r");
	if (!fh) {
		printf("Unable to open file: %s: %s\n", input_file, strerror(errno));
		return 1;
	}

	int sector_ids_sum = 0;

	while (feof(fh) == 0) {
		char buf[1024];
		memset(buf, 0, sizeof(buf));
		if (fgets(buf, sizeof(buf), fh) == NULL) {
			break;
		}

		int sector_id = __get_valid_sector_id(buf);
		if (sector_id == -1) {
			continue;
		}

		sector_ids_sum += sector_id;
	}

	if (fclose(fh) != 0) {
		printf("fclose(): %s\n", strerror(errno));
		return 1;
	}

	printf("%d\n", sector_ids_sum);

	return 0;
}

static int
__get_valid_sector_id(const char * const buf)
{
	struct Letter * map = calloc(26, sizeof(struct Letter));
	if (!map) {
		return -1;
	}
	for (int i = 0; i < 26; i++) {
		map[i].c = i+97;
	}

	char sector_str[1024];
	memset(sector_str, 0, sizeof(sector_str));
	char * sector_str_ptr = sector_str;

	const char * ptr = buf;

	while (*ptr) {
		if (*ptr == '\n') {
			break;
		}

		if (*ptr >= 97 && *ptr <= 122) {
			map[*ptr-97].count++;
			ptr++;
			continue;
		}

		if (*ptr == '-') {
			ptr++;
			continue;
		}

		if (*ptr >= 48 && *ptr <= 57) {
			*sector_str_ptr = *ptr;
			sector_str_ptr++;
			ptr++;
			continue;
		}

		if (*ptr == '[') {
			if (__valid_checksum(map, ptr+1)) {
				free(map);
				return atoi(sector_str);
			}

			free(map);
			return -1;
		}

		printf("Unrecognized character\n");
		free(map);
		return -1;
	}

	printf("Unexpected end of input\n");
	free(map);
	return -1;
}

static bool
__valid_checksum(struct Letter * map, const char * const checksum)
{
#ifdef DEBUG
	printf("Before sorting:\n");
	__print_letters(map);
#endif
	qsort(map, 26, sizeof(struct Letter), __letter_cmp);
#ifdef DEBUG
	printf("After sorting:\n");
	__print_letters(map);
#endif

	int i = 0;
	const char * ptr = checksum;
	while (*ptr) {
		if (*ptr == ']') {
			break;
		}

		if (map[i].c != *ptr) {
			return false;
		}

		i++;
		ptr++;
	}

	return true;
}

#ifdef DEBUG
static void
__print_letters(const struct Letter * const map)
{
	for (int i = 0; i < 26; i++) {
		printf("Letter: %c Count: %d\n", map[i].c, map[i].count);
	}
}
#endif

static int
__letter_cmp(const void * p1, const void * p2)
{
	const struct Letter * l1 = p1;
	const struct Letter * l2 = p2;

	if (l1->count > l2->count) {
		return -1;
	}

	if (l1->count < l2->count) {
		return 1;
	}

	if (l1->c < l2->c) {
		return -1;
	}

	if (l1->c > l2->c) {
		return 1;
	}

	return 0;
}
