#include <ctype.h>
#include <errno.h>
#include <map.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char * get_word(char const * const, int * const);

int
main(const int argc, const char * const * const argv)
{
	(void)argc;
	(void)argv;

	FILE * const fh = stdin;

	char buf[10240] = {0};

	int count = 0;

	while (1) {
		if (fgets(buf, (int)sizeof(buf), fh) == NULL) {
			if (!feof(fh)) {
				fprintf(stderr, "fgets(): %s\n", strerror(errno));
				return 1;
			}
			break;
		}

		struct htable * const h = hash_init(1024);
		if (!h) {
			fprintf(stderr, "hash_init()\n");
			return 1;
		}

		int idx = 0;
		bool valid = true;
		while (1) {
			char * const word = get_word(buf, &idx);
			if (strlen(word) == 0) {
				free(word);
				break;
			}

			if (hash_has_key(h, word)) {
				valid = false;
				free(word);
				break;
			}

			if (!hash_set(h, word, NULL)) {
				fprintf(stderr, "hash_set()\n");
				hash_free(h, NULL);
				free(word);
				return 1;
			}
			free(word);
		}

		if (valid) {
			count++;
		}

		if (!hash_free(h, NULL)) {
			fprintf(stderr, "hash_free()\n");
			return 1;
		}
	}

	printf("%d\n", count);
	return 0;
}

static char *
get_word(char const * const s, int * const idx)
{
	char const * ptr = s + *idx;

	while (isspace(*ptr)) {
		ptr++;
		*idx += 1;
	}

	char * const word = calloc(strlen(s), sizeof(char));
	if (!word) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		return NULL;
	}

	char * wordptr = word;

	while (*ptr != '\0' && !isspace(*ptr)) {
		*wordptr = *ptr;
		wordptr++;
		ptr++;
		*idx += 1;
	}

	return word;
}
