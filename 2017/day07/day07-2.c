#include <ctype.h>
#include <errno.h>
#include <map.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct program {
	char * name;
	int weight;
	struct htable * h;
};

static struct program *
parse_line(char const * const);
static void
program_destroy(void * const);
static void
print_program(struct program const * const);
static char *
get_lowest(struct htable const * const, char const * const);
static int
get_weight(struct htable const * const,
		struct program const * const,
		int * const);

int main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	FILE * const fh = stdin;

	char buf[10240] = {0};

	struct htable * const h = hash_init(1024);
	if (!h) {
		fprintf(stderr, "hash_init()\n");
		return 1;
	}

	char const * last_name = NULL;

	while (1) {
		if (fgets(buf, (int) sizeof(buf), fh) == NULL) {
			if (!feof(fh)) {
				fprintf(stderr, "fgets(): %s\n", strerror(errno));
				hash_free(h, program_destroy);
				return 1;
			}
			break;
		}

		struct program * const program = parse_line(buf);
		if (!program) {
			fprintf(stderr, "parse_line()\n");
			hash_free(h, program_destroy);
			return 1;
		}

		if (0) {
			print_program(program);
		}

		if (!hash_set(h, program->name, program)) {
			fprintf(stderr, "hash_set()\n");
			hash_free(h, program_destroy);
			program_destroy(program);
			return 1;
		}

		last_name = program->name;
	}

	char * const lowest_name = get_lowest(h, last_name);
	if (!lowest_name) {
		fprintf(stderr, "get_lowest()\n");
		hash_free(h, program_destroy);
		return 1;
	}

	struct program const * const lowest_program = hash_get(h, lowest_name);
	if (!lowest_program) {
		fprintf(stderr, "hash_get()\n");
		hash_free(h, program_destroy);
		free(lowest_name);
		return 1;
	}

	free(lowest_name);

	int mismatch_weight = 0;
	if (get_weight(h, lowest_program, &mismatch_weight) == -1) {
		fprintf(stderr, "get_weight()\n");
		hash_free(h, program_destroy);
		return 1;
	}

	if (!hash_free(h, program_destroy)) {
		fprintf(stderr, "hash_free()\n");
		return 1;
	}

	printf("%d\n", mismatch_weight);

	return 0;
}

static struct program *
parse_line(char const * const s)
{
	char * const name = calloc(strlen(s)+1, sizeof(char));
	if (!name) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		return NULL;
	}

	char const * ptr = s;
	int i = 0;
	while (*ptr != '\0' && *ptr != ' ') {
		name[i++] = *ptr;
		ptr++;
	}

	if (*ptr != ' ') {
		fprintf(stderr, "no space after first name\n");
		free(name);
		return NULL;
	}

	ptr++;
	if (*ptr != '(') {
		fprintf(stderr, "no (\n");
		free(name);
		return NULL;
	}
	ptr++;

	int const weight = atoi(ptr);

	while (*ptr != '\0' && *ptr != ')') {
		ptr++;
	}

	if (*ptr != ')') {
		fprintf(stderr, ") not found\n");
		free(name);
		return NULL;
	}
	ptr++;

	while (isspace(*ptr)) {
		ptr++;
	}

	struct program * const program = calloc(1, sizeof(struct program));
	if (!program) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		free(name);
		return NULL;
	}
	program->name = name;
	program->weight = weight;

	if (*ptr == '\0') {
		return program;
	}

	if (*ptr != '-' ||
			*(ptr+1) != '>' ||
			*(ptr+2) != ' ') {
		fprintf(stderr, "missing '-> '\n");
		program_destroy(program);
		return NULL;
	}
	ptr += 3;

	program->h = hash_init(1024);
	if (!program->h) {
		fprintf(stderr, "hash_init()\n");
		program_destroy(program);
		return NULL;
	}

	while (1) {
		char * const p2 = calloc(strlen(ptr)+1, sizeof(char));
		if (!p2) {
			fprintf(stderr, "calloc(): %s\n", strerror(errno));
			program_destroy(program);
			return NULL;
		}

		int j = 0;
		while (isalpha(*ptr)) {
			p2[j++] = *ptr;
			ptr++;
		}

		if (!hash_set(program->h, p2, NULL)) {
			fprintf(stderr, "hash_set()\n");
			program_destroy(program);
			free(p2);
			return NULL;
		}
		free(p2);

		while (*ptr == ',' || *ptr == ' ' || *ptr == '\n') {
			ptr++;
		}
		if (*ptr == '\0') {
			return program;
		}
	}

	fprintf(stderr, "should not hit\n");
	program_destroy(program);
	return NULL;
}

static void
program_destroy(void * const v)
{
	if (!v) {
		return;
	}

	struct program * const p = v;

	if (p->name) {
		free(p->name);
	}

	if (p->h) {
		hash_free(p->h, NULL);
	}

	free(p);
}

static void
print_program(struct program const * const p)
{
	printf("program: %s weight %d\n", p->name, p->weight);

	if (!p->h) {
		return;
	}

	void * * const keys = hash_get_keys(p->h);
	if (!keys) {
		fprintf(stderr, "hash_get_keys()\n");
		return;
	}

	for (size_t i = 0; keys[i]; i++) {
		char const * const key = keys[i];
		printf("    key: %s\n", key);
	}

	hash_free_keys(keys);
}

static char *
get_lowest(struct htable const * const h, char const * const name)
{
	void * * const keys = hash_get_keys(h);
	if (!keys) {
		fprintf(stderr, "hash_get_keys()\n");
		return NULL;
	}

	for (size_t i = 0; keys[i]; i++) {
		char const * const key = keys[i];

		struct program const * const p = hash_get(h, key);
		if (!p) {
			fprintf(stderr, "hash_get()\n");
			hash_free_keys(keys);
			return NULL;
		}

		if (hash_has_key(p->h, name)) {
			char * const lowest = get_lowest(h, key);
			hash_free_keys(keys);
			return lowest;
		}
	}

	hash_free_keys(keys);

	// Because we free the keys
	char * const name_copy = calloc(strlen(name)+1, sizeof(char));
	if (!name_copy) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		return NULL;
	}
	strcpy(name_copy, name);

	return name_copy;
}

static int
get_weight(struct htable const * const h,
		struct program const * const program,
		int * const mismatch_weight)
{
	int weight = program->weight;

	if (!program->h) {
		return weight;
	}

	void * * const keys = hash_get_keys(program->h);
	if (!keys) {
		fprintf(stderr, "hash_get_keys(program=%s)\n", program->name);
		return -1;
	}

	int last_weight = -1;
	for (size_t i = 0; keys[i]; i++) {
		char const * const sub_program_name = keys[i];

		struct program const * const sub_program = hash_get(h, sub_program_name);
		if (!sub_program) {
			fprintf(stderr, "hash_get()\n");
			hash_free_keys(keys);
			return -1;
		}

		int const sub_weight = get_weight(h, sub_program, mismatch_weight);
		if (sub_weight == -1) {
			fprintf(stderr, "get_weight()\n");
			hash_free_keys(keys);
			return -1;
		}

		if (last_weight != -1) {
			if (last_weight != sub_weight) {
				if (*mismatch_weight == 0) {
					*mismatch_weight = sub_program->weight - abs(sub_weight-last_weight);
				}
			}
		} else {
			last_weight = sub_weight;
		}

		weight += sub_weight;
	}

	hash_free_keys(keys);
	return weight;
}
