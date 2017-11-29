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
static bool
__decrypt_room(const char * const, const int);

int
main(void)
{
	const char * const input_file = "input4-1.txt";

	FILE * const fh = fopen(input_file, "r");
	if (!fh) {
		printf("Unable to open file: %s: %s\n", input_file, strerror(errno));
		return 1;
	}

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

		if (!__decrypt_room(buf, sector_id)) {
			fclose(fh);
			return 1;
		}
	}

	if (fclose(fh) != 0) {
		printf("fclose(): %s\n", strerror(errno));
		return 1;
	}

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

static bool
__decrypt_room(const char * const buf, const int sector_id)
{
	char * decrypted = calloc(strlen(buf)+1, sizeof(char));
	if (!decrypted) {
		printf("%s\n", strerror(ENOMEM));
		return false;
	}
	int decrypted_index = 0;

	const char * ptr = buf;

	while (*ptr) {
		if (*ptr >= 48 && *ptr <= 57) {
			break;
		}

		char c = *ptr;
		for (int i = 0; i < sector_id; i++) {
			if (c == '-') {
				c = ' ';
				break;
			}

			if (c == 122) {
				c = 97;
				continue;
			}

			c++;
		}

		decrypted[decrypted_index] = c;
		decrypted_index++;
		ptr++;
	}

	printf("%s (%d)\n", decrypted, sector_id);
	free(decrypted);
	return true;
}
