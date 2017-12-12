#include <ctype.h>
#include <errno.h>
#include <map.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct program {
	char * id;
	struct htable * connected_ids;
};

static bool
parse_line(char const * const, struct htable * const);
static void
program_destroy(void * const);
static void
print_programs(struct htable const * const);
static void
maybe_set(struct htable * const, char const * const);
static void
set_connected(struct htable * const,
		struct htable * const,
		char const * const);

int main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	FILE * const fh = stdin;

	char buf[41960] = {0};

	struct htable * const programs = hash_init(1024);
	if (!programs) {
		fprintf(stderr, "hash_init()\n");
		return 1;
	}

	while (1) {
		if (fgets(buf, (int) sizeof(buf), fh) == NULL) {
			if (!feof(fh)) {
				fprintf(stderr, "fgets(): %s\n", strerror(errno));
				hash_free(programs, program_destroy);
				return 1;
			}
			break;
		}

		if (!parse_line(buf, programs)) {
			fprintf(stderr, "parse_line()\n");
			hash_free(programs, program_destroy);
			return 1;
		}
	}

	if (0) {
		print_programs(programs);
	}

	struct htable * const connected = hash_init(1024);
	if (!connected) {
		fprintf(stderr, "hash_init()\n");
		hash_free(programs, program_destroy);
		return 1;
	}

	set_connected(programs, connected, "0");

	if (!hash_free(programs, program_destroy)) {
		fprintf(stderr, "hash_free(programs)\n");
		hash_free(connected, NULL);
		return 1;
	}

	int const count = hash_count_elements(connected);
	if (count == -1) {
		fprintf(stderr, "hash_count_elements()\n");
		hash_free(connected, NULL);
		return 1;
	}

	if (!hash_free(connected, NULL)) {
		fprintf(stderr, "hash_free(connected)\n");
		return 1;
	}

	printf("%d\n", count);
	return 0;
}

static bool
parse_line(char const * const line, struct htable * const programs)
{
	struct program * const p = calloc(1, sizeof(struct program));
	if (!p) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		return false;
	}

	p->connected_ids = hash_init(1024);
	if (!p->connected_ids) {
		fprintf(stderr, "hash_init()\n");
		program_destroy(p);
		return false;
	}

	char const * ptr = line;

	int const id = atoi(ptr);
	p->id = calloc(128, sizeof(char));
	if (!p->id) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		program_destroy(p);
		return false;
	}
	sprintf(p->id, "%d", id);

	while (isdigit(*ptr) || isspace(*ptr)) {
		ptr++;
	}

	if (strncmp(ptr, "<-> ", 4) != 0) {
		fprintf(stderr, "missing <->: %s\n", ptr);
		program_destroy(p);
		return false;
	}
	ptr += 4;

	while (*ptr != '\0' && *ptr != '\n') {
		int const id2 = atoi(ptr);
		char buf[128] = {0};
		sprintf(buf, "%d", id2);
		if (!hash_set(p->connected_ids, buf, NULL)) {
			fprintf(stderr, "hash_set()\n");
			program_destroy(p);
			return false;
		}

		while (isdigit(*ptr)) {
			ptr++;
		}
		while (*ptr == ',' || isspace(*ptr)) {
			ptr++;
		}
	}

	if (!hash_set(programs, p->id, p)) {
		fprintf(stderr, "hash_set()\n");
		program_destroy(p);
		return false;
	}

	return true;
}

static void
program_destroy(void * const v)
{
	if (!v) {
		return;
	}

	struct program * const p = v;

	if (p->id) {
		free(p->id);
	}

	if (p->connected_ids) {
		hash_free(p->connected_ids, NULL);
	}

	free(p);
}

static void
print_programs(struct htable const * const h)
{
	void * * const keys = hash_get_keys(h);
	if (!keys) {
		fprintf(stderr, "hash_get_keys()\n");
		return;
	}

	for (size_t i = 0; keys[i]; i++) {
		char const * const key = keys[i];
		printf("program %s:\n", key);

		struct program const * const p = hash_get(h, key);
		if (!p) {
			fprintf(stderr, "print_programs(): hash_get(%s)\n", key);
			hash_free_keys(keys);
			return;
		}

		if (!p->connected_ids) {
			continue;
		}

		void * * const keys2 = hash_get_keys(p->connected_ids);
		for (size_t j = 0; keys2[j]; j++) {
			char const * const key2 = keys2[j];
			if (j == 0) {
				printf("%s", key2);
				continue;
			}
			printf(",%s", key2);
		}
		printf("\n");
		hash_free_keys(keys2);
	}

	hash_free_keys(keys);
}

static void
maybe_set(struct htable * const h, char const * const key)
{
	if (hash_has_key(h, key)) {
		return;
	}

	if (!hash_set(h, key, NULL)) {
		fprintf(stderr, "maybe_set(%s)\n", key);
		return;
	}
}

static void
set_connected(struct htable * const programs,
		struct htable * const connected,
		char const * const id)
{
	struct program const * const p = hash_get(programs, id);
	if (!p) {
		fprintf(stderr, "hash_get()\n");
		return;
	}

	if (hash_has_key(connected, id)) {
		return;
	}
	maybe_set(connected, id);

	if (!p->connected_ids) {
		return;
	}

	void * * const ids = hash_get_keys(p->connected_ids);
	for (size_t i = 0; ids[i]; i++) {
		char const * const id2 = ids[i];
		set_connected(programs, connected, id2);
	}
	hash_free_keys(ids);
}
