#include <ctype.h>
#include <errno.h>
#include <map.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int
get_bank(char const * const, int * const);
static char *
banks_to_string(int const * const, int const);
static bool
banks_redistribute(int * const, int const);

int main(const int argc, const char * const * const argv)
{
	(void) argc;
	(void) argv;

	FILE * const fh = stdin;

	char buf[10240] = {0};

	if (fgets(buf, (int) sizeof(buf), fh) == NULL) {
		fprintf(stderr, "fgets(): %s\n", strerror(errno));
		return 1;
	}

	int bank_count = 0;
	int idx = 0;
	while (1) {
		if (get_bank(buf, &idx) == -1) {
			break;
		}
		bank_count++;
	}

	int banks[bank_count];
	memset(banks, 0, (size_t) bank_count);

	idx = 0;
	for (int i = 0; i < bank_count; i++) {
		banks[i] = get_bank(buf, &idx);
		if (banks[i] == -1) {
			fprintf(stderr, "failed to get bank\n");
			return 1;
		}
	}

	struct htable * const h = hash_init(1024);
	if (!h) {
		fprintf(stderr, "hash_init()\n");
		return 1;
	}

	char * interesting_state = NULL;
	while (1) {
		char * const s = banks_to_string(banks, bank_count);
		if (!s) {
			fprintf(stderr, "banks_to_string()\n");
			hash_free(h, NULL);
			return 1;
		}

		if (hash_has_key(h, s)) {
			interesting_state = s;
			break;
		}

		if (!hash_set(h, s, NULL)) {
			fprintf(stderr, "hash_set()\n");
			hash_free(h, NULL);
			free(s);
			return 1;
		}

		free(s);

		if (!banks_redistribute(banks, bank_count)) {
			fprintf(stderr, "banks_redistribute()\n");
			hash_free(h, NULL);
			free(s);
			return 1;
		}
	}

	if (!hash_free(h, NULL)) {
		fprintf(stderr, "hash_free()\n");
		free(interesting_state);
		return 1;
	}

	int steps = 0;

	if (!banks_redistribute(banks, bank_count)) {
		fprintf(stderr, "banks_redistribute()\n");
		free(interesting_state);
		return 1;
	}

	steps++;

	while (1) {
		char * const s = banks_to_string(banks, bank_count);
		if (!s) {
			fprintf(stderr, "banks_to_string()\n");
			free(interesting_state);
			return 1;
		}

		if (strcmp(s, interesting_state) == 0) {
			free(s);
			break;
		}

		free(s);

		if (!banks_redistribute(banks, bank_count)) {
			fprintf(stderr, "banks_redistribute()\n");
			free(interesting_state);
			return 1;
		}

		steps++;
	}

	free(interesting_state);

	printf("%d\n", steps);
	return 0;
}

static int
get_bank(char const * const s, int * const idx)
{
	char const * ptr = s+*idx;

	while (isspace(*ptr)) {
		ptr++;
		*idx += 1;
	}

	if (*ptr == '\0') {
		return -1;
	}

	int const blocks = atoi(ptr);

	while (isdigit(*ptr)) {
		ptr++;
		*idx += 1;
	}

	return blocks;
}

static char *
banks_to_string(int const * const banks, int const count)
{
	char * const buf = calloc(1024, sizeof(char));
	if (!buf) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		return NULL;
	}

	for (int i = 0; i < count; i++) {
		char buf2[128] = {0};
		sprintf(buf2, "%d, ", banks[i]);
		strcat(buf, buf2);
	}

	return buf;
}

static bool
banks_redistribute(int * const banks, int const count)
{
	int largest_idx = 0;
	int largest = 0;
	for (int i = 0; i < count; i++) {
		if (banks[i] <= largest) {
			continue;
		}
		largest = banks[i];
		largest_idx = i;
	}

	int to_distribute = largest;
	banks[largest_idx] = 0;

	int idx = largest_idx+1;
	while (to_distribute > 0) {
		if (idx >= count) {
			idx = 0;
		}

		banks[idx]++;
		to_distribute--;
		idx++;
	}

	return true;
}
