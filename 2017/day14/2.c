#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int *
knot_hash(char const * const, int const);
static void
reverse(int * const, int const, int const, int const);
static void
map_destroy(int * * const, int const);
static void
map_set(int * * const, int const, int * const);
static void
consume(int * * const, int const, int const);

int main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	FILE * const fh = stdin;

	char buf[41960] = {0};

	if (fgets(buf, (int) sizeof(buf), fh) == NULL) {
			fprintf(stderr, "fgets(): %s\n", strerror(errno));
			return 1;
	}

	char * ptr = buf;
	while (*ptr != '\0') {
		if (*ptr == '\n') {
			*ptr = '\0';
			break;
		}
		ptr++;
	}

	int * * const map = calloc(128, sizeof(int *));
	if (!map) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		return 1;
	}
	for (int i = 0; i < 128; i++) {
		map[i] = calloc(128, sizeof(int));
		if (!map[i]) {
			fprintf(stderr, "calloc(): %s\n", strerror(errno));
			map_destroy(map, 128);
			return 1;
		}
	}

	int const size = 256;

	char key[128] = {0};
	for (int i = 0; i < 128; i++) {
		memset(key, 0, 128);
		sprintf(key, "%s-%d", buf, i);

		int * const hash = knot_hash(key, size);
		if (!hash) {
			fprintf(stderr, "knot_hash()\n");
			map_destroy(map, 128);
			return 1;
		}

		if (0) {
			for (int j = 0; j < 16; j++) {
				printf("%02x", (unsigned int) hash[j]);
			}
			printf("\n");
		}

		map_set(map, i, hash);

		if (0) {
			for (int j = 0; j < 128; j++ ){
				if (map[i][j]) {
					printf("#");
				} else {
					printf(".");
				}
			}
			printf("\n");
		}

		free(hash);
	}

	int count = 0;
	for (int i = 0; i < 128; i++) {
		for (int j = 0; j < 128; j++) {
			if (map[i][j] != 1) {
				continue;
			}

			consume(map, i, j);
			count++;
		}
	}

	map_destroy(map, 128);

	printf("%d\n", count);
	return 0;
}

static int *
knot_hash(char const * const input, int const size)
{
	int * const hash_inputs = calloc(strlen(input)+5, sizeof(int));
	if (!hash_inputs) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		return NULL;
	}

	char const * ptr = input;
	int hash_input_size = 0;
	while (*ptr != '\0' && *ptr != '\n') {
		hash_inputs[hash_input_size] = (int) *ptr;
		hash_input_size++;
		ptr++;
	}

	hash_inputs[hash_input_size++] = 17;
	hash_inputs[hash_input_size++] = 31;
	hash_inputs[hash_input_size++] = 73;
	hash_inputs[hash_input_size++] = 47;
	hash_inputs[hash_input_size++] = 23;

	int * const list = calloc((size_t) size, sizeof(int));
	if (!list) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		free(hash_inputs);
		return NULL;
	}

	for (int i = 0; i < size; i++) {
		list[i] = i;
	}

	int idx = 0;
	int skip_size = 0;
	for (int round = 0; round < 64; round++) {
		for (int i = 0; i < hash_input_size; i++) {
			int const length = hash_inputs[i];

			reverse(list, size, idx, length);

			idx += length;
			if (idx >= size) {
				idx -= size;
			}

			idx += skip_size;
			if (idx >= size) {
				idx -= size;
			}

			skip_size++;
			if (skip_size == size) {
				skip_size = 0;
			}
		}
	}

	free(hash_inputs);

	int * const dense = calloc(16, sizeof(int));
	if (!dense) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		free(list);
		return NULL;
	}

	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			int const pos = i*16+j;
			dense[i] ^= list[pos];
		}
	}

	free(list);

	return dense;
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
map_destroy(int * * const map, int const size)
{
	if (!map) {
		return;
	}

	for (int i = 0; i < size; i++) {
		if (!map[i]) {
			break;
		}
		free(map[i]);
	}

	free(map);
}

static void
map_set(int * * const map, int const row, int * const hash)
{
	for (int i = 0; i < 16; i++) {
		int x = hash[i];

		for (int j = 7; j >= 0; j--) {
			int const idx = i*8+j;

			if (x & 0x01) {
				map[row][idx] = 1;
			}

			x >>= 1;
		}
	}
}

static void
consume(int * * const map, int const x, int const y)
{
	map[x][y] = 0;

	for (int i = x+1; i < 128; i++) {
		if (map[i][y] != 1) {
			break;
		}

		consume(map, i, y);
	}

	for (int i = x-1; i >= 0; i--) {
		if (map[i][y] != 1) {
			break;
		}

		consume(map, i, y);
	}

	for (int i = y+1; i < 128; i++) {
		if (map[x][i] != 1) {
			break;
		}

		consume(map, x, i);
	}

	for (int i = y-1; i >= 0; i--) {
		if (map[x][i] != 1) {
			break;
		}

		consume(map, x, i);
	}
}
