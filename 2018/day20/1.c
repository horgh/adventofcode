#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

static char ** expand(char const * const, size_t * const);

static char const * find_end_of_group(char const * const);

#define SZ 2048
#define STRING_SZ 4096

int
main(int const argc, char const * const * const argv)
{
	(void)argc;
	(void)argv;

	char buf[40960] = {0};
	assert(fgets(buf, 40960, stdin) != NULL);
	trim_right(buf);

	assert(buf[0] == '^');
	char const * ptr = buf + 1;
	char * ptr2 = buf;
	while (*ptr2 != '\0') {
		if (*ptr2 == '$') {
			*ptr2 = '\0';
			break;
		}
		ptr2++;
	}

	size_t n_pieces = 0;
	char ** const pieces = expand(ptr, &n_pieces);

	size_t max = 0;
	for (size_t i = 0; i < n_pieces; i++) {
		if (strlen(pieces[i]) <= max) {
			continue;
		}
		max = strlen(pieces[i]);
	}
	printf("%zu\n", max);

	for (size_t i = 0; i < n_pieces; i++) {
		free(pieces[i]);
	}
	free(pieces);
	return 0;
}

static char **
expand(char const * const s, size_t * const ret_n_pieces)
{
	char ** pieces = calloc(SZ, sizeof(char *));
	assert(pieces != NULL);
	size_t n_pieces = 0;
	size_t len = 0;

	pieces[n_pieces] = calloc(STRING_SZ, sizeof(char));
	assert(pieces[n_pieces] != NULL);
	n_pieces++;

	char ** const done_pieces = calloc(SZ, sizeof(char *));
	assert(done_pieces != NULL);
	size_t n_done_pieces = 0;

	char const * ptr = s;
	while (1) {
		if (*ptr == 'N' || *ptr == 'E' || *ptr == 'S' || *ptr == 'W') {
			for (size_t i = 0; i < n_pieces; i++) {
				pieces[i][len] = *ptr;
			}
			len++;
			assert(len + 1 != STRING_SZ);
			ptr++;
			continue;
		}

		if (*ptr == '(') {
			size_t n_pieces2 = 0;
			char ** const pieces2 = expand(ptr + 1, &n_pieces2);

			char ** const pieces3 = calloc(n_pieces * n_pieces2, sizeof(char *));
			assert(pieces3 != NULL);

			bool has_detour = false;
			for (size_t i = 0; i < n_pieces2; i++) {
				if (strlen(pieces2[i]) == 0) {
					has_detour = true;
				}
			}

			size_t n_pieces3 = 0;
			for (size_t i = 0; i < n_pieces; i++) {
				for (size_t j = 0; j < n_pieces2; j++) {
					char * const s2 = calloc(STRING_SZ, sizeof(char));
					assert(strlen(pieces[i]) + strlen(pieces2[j]) + 1 <= STRING_SZ);

					if (has_detour && strlen(pieces2[j]) > 0) {
						strcat(s2, pieces[i]);
						size_t k = strlen(s2);
						for (size_t l = 0; l < strlen(pieces2[j]) / 2; l++) {
							s2[k++] = pieces2[j][l];
						}
						done_pieces[n_done_pieces++] = s2;
						continue;
					}

					strcat(s2, pieces[i]);
					strcat(s2, pieces2[j]);
					pieces3[n_pieces3++] = s2;
				}
			}

			for (size_t i = 0; i < n_pieces; i++) {
				free(pieces[i]);
			}
			free(pieces);
			for (size_t i = 0; i < n_pieces2; i++) {
				free(pieces2[i]);
			}
			free(pieces2);

			pieces = pieces3;
			n_pieces = n_pieces3;

			if (n_pieces == 0) {
				pieces[0] = calloc(STRING_SZ, sizeof(char));
				assert(pieces[0] != NULL);
				n_pieces++;
				len = 0;
			} else {
				len = strlen(pieces[0]);
			}

			ptr = find_end_of_group(ptr + 1);
			continue;
		}

		if (*ptr == '|') {
			for (size_t i = 0; i < n_pieces; i++) {
				done_pieces[n_done_pieces++] = pieces[i];
				assert(n_done_pieces != SZ);
			}
			pieces[0] = calloc(STRING_SZ, sizeof(char));
			assert(pieces[0] != NULL);
			n_pieces = 1;
			len = 0;
			ptr++;
			continue;
		}

		if (*ptr == ')' || *ptr == '\0') {
			for (size_t i = 0; i < n_pieces; i++) {
				done_pieces[n_done_pieces++] = pieces[i];
				assert(n_done_pieces != SZ);
			}
			free(pieces);
			*ret_n_pieces = n_done_pieces;
			return done_pieces;
		}

		assert(1 == 0);
	}
}

__attribute__((pure)) static char const *
find_end_of_group(char const * const s)
{
	char const * ptr = s;
	int n_braces = 0;
	while (1) {
		assert(*ptr != '\0');

		if (*ptr == '(') {
			n_braces++;
			ptr++;
			continue;
		}

		if (*ptr == ')') {
			if (n_braces == 0) {
				return ptr + 1;
			}
			n_braces--;
			ptr++;
			continue;
		}

		ptr++;
	}

	return NULL;
}
