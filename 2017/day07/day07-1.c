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
print_program(struct program const * const);
static char const *
get_lowest(struct htable const * const, char const * const);

int main(const int argc, char const * const * const argv)
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
				// XXX cleanup
				return 1;
			}
			break;
		}

		struct program * const program = parse_line(buf);
		if (!program) {
			fprintf(stderr, "parse_line()\n");
			// XXX cleanup
			return 1;
		}

		if (0) {
			print_program(program);
		}

		if (!hash_set(h, program->name, program)) {
			fprintf(stderr, "hash_set()\n");
			return 1;
		}

		last_name = program->name;
	}

	char const * const lowest_name = get_lowest(h, last_name);
	printf("got %s\n", lowest_name);

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
		return NULL;
	}

	ptr++;
	if (*ptr != '(') {
		fprintf(stderr, "no (\n");
		return NULL;
	}
	ptr++;

	int const weight = atoi(ptr);

	while (*ptr != '\0' && *ptr != ')') {
		ptr++;
	}

	if (*ptr != ')') {
		fprintf(stderr, ") not found\n");
		return NULL;
	}
	ptr++;

	while (isspace(*ptr)) {
		ptr++;
	}

	struct program * const program = calloc(1, sizeof(struct program));
	if (!program) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
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
		return NULL;
	}
	ptr += 3;

	program->h = hash_init(1024);
	if (!program->h) {
		fprintf(stderr, "hash_init()\n");
		return NULL;
	}

	while (1) {
		char * const p2 = calloc(strlen(ptr)+1, sizeof(char));
		if (!p2) {
			fprintf(stderr, "calloc(): %s\n", strerror(errno));
			return NULL;
		}

		int j = 0;
		while (isalpha(*ptr)) {
			p2[j++] = *ptr;
			ptr++;
		}

		if (!hash_set(program->h, p2, NULL)) {
			fprintf(stderr, "hash_set()\n");
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
	return NULL;
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

	//hash_free_keys(keys);
}

static char const *
get_lowest(struct htable const * const h, char const * const name)
{
	void * * const keys = hash_get_keys(h);
	if (!keys) {
		fprintf(stderr, "hash_get_keys()\n");
		return NULL;
	}

	printf("-> %s\n", name);

	for (size_t i = 0; keys[i]; i++) {
		char const * const key = keys[i];

		printf("  -> %s\n", key);

		struct program const * const p = hash_get(h, key);
		if (!p) {
			fprintf(stderr, "hash_get()\n");
			return NULL;
		}

		if (hash_has_key(p->h, name)) {
			printf("  ->found %s\n", key);
			//hash_free_keys(keys);
			return get_lowest(h, key);
		}
	}

	//hash_free_keys(keys);
	return name;
}
