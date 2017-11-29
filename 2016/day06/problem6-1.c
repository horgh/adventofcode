#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Letter {
	int c;
	int count;
};

static void
__destroy_index_to_letters(struct Letter * * const,
		const size_t);
static int
__cmp_letters(const void *, const void *);
static void
__print_letters(const struct Letter * const, const size_t);

int
main(const int argc, const char * const * const argv)
{
	if (argc != 2) {
		printf("Usage: %s <input file>\n", argv[0]);
		return 1;
	}
	const char * const input_file = argv[1];

	FILE * const fh = fopen(input_file, "r");
	if (!fh) {
		printf("Unable to open file: %s: %s\n", input_file, strerror(errno));
		return 1;
	}

	// Data structure: At each index we store a block of memory - 26 Letters. Ahead
	// of time I support at most max_password_len indices. If the password is
	// longer than that we do not decode it.

	size_t max_password_len = 100;

	struct Letter * * index_to_letters = calloc(max_password_len,
			sizeof(struct Letter *));
	if (!index_to_letters) {
		printf("%s\n", strerror(ENOMEM));
		fclose(fh);
		return 1;
	}

	size_t alphabet_len = 26;

	// Allocate a block of memory where there is one struct Letter for each letter
	// in the alphabet. Initialize this struct too.
	for (size_t i = 0; i < max_password_len; i++) {
		index_to_letters[i] = calloc(alphabet_len, sizeof(struct Letter));
		if (!index_to_letters[i]) {
			printf("%s\n", strerror(ENOMEM));
			fclose(fh);
			__destroy_index_to_letters(index_to_letters, max_password_len);
			return 1;
		}

		struct Letter * letters = index_to_letters[i];

		for (size_t j = 0; j < alphabet_len; j++) {
			letters[j].c = (int) j+97;
			letters[j].count = 0;
		}
	}

	size_t actual_password_len = 0;

	while (feof(fh) == 0) {
		char buf[1024];
		memset(buf, 0, sizeof(buf));
		if (fgets(buf, sizeof(buf), fh) == NULL) {
			break;
		}

		if (buf[strlen(buf)-1] == '\n') {
			buf[strlen(buf)-1] = '\0';
		}

		actual_password_len = strlen(buf);

		if (actual_password_len > max_password_len) {
			printf("password too long\n");
			fclose(fh);
			__destroy_index_to_letters(index_to_letters, max_password_len);
			return 1;
		}

		const char * ptr = buf;
		while (*ptr != '\0') {
			// Index in the password.
			long int i = ptr-buf;

			struct Letter * letters = index_to_letters[i];

			// Letter index.
			int j = *ptr-97;
			if (j < 0 || j > 25) {
				printf("character out of bounds [%c]\n", *ptr);
				break;
			}

			letters[j].count++;
			ptr++;
		}
	}

	if (fclose(fh) != 0) {
		printf("fclose(): %s\n", strerror(errno));
		__destroy_index_to_letters(index_to_letters, max_password_len);
		return 1;
	}

	int * password = calloc(actual_password_len+1, sizeof(int));
	if (!password) {
		__destroy_index_to_letters(index_to_letters, max_password_len);
		return 1;
	}

	for (size_t i = 0; i < actual_password_len; i++) {
		struct Letter * letters = index_to_letters[i];
		__print_letters(letters, alphabet_len);
		qsort(letters, alphabet_len, sizeof(struct Letter), __cmp_letters);
		password[i] = letters[0].c;
		if (false) {
			printf("Position %zu\n", i);
			__print_letters(letters, alphabet_len);
		}
	}

	__destroy_index_to_letters(index_to_letters, max_password_len);

	printf("Password (length %zu): ", actual_password_len);
	for (size_t i = 0; i < actual_password_len; i++) {
		printf("%c", password[i]);
	}
	printf("\n");

	free(password);

	return 0;
}

static void
__destroy_index_to_letters(struct Letter * * const index_to_letters,
		const size_t sz)
{
	if (!index_to_letters) {
		return;
	}

	for (size_t i = 0; i < sz; i++) {
		free(index_to_letters[i]);
	}

	free(index_to_letters);
}

static int
__cmp_letters(const void * p1, const void * p2)
{
	const struct Letter * l1 = p1;
	const struct Letter * l2 = p2;

	if (l1->count > l2->count) {
		return -1;
	}

	if (l1->count < l2->count) {
		return 1;
	}

	return 0;
}

static void
__print_letters(const struct Letter * const letters, const size_t sz)
{
	for (size_t i = 0; i < sz; i++) {
		printf("Letter: %c Count: %d\n", letters[i].c, letters[i].count);
	}
}
