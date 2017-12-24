#include <ctype.h>
#include <errno.h>
#include <map.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 0

struct component;

struct port {
	bool used;
	int strength;
};

struct component {
	int id;
	struct port ports[2];
};

static void
destroy_components(
		struct component * * const,
		size_t const);
static struct component *
parse_line(char const * const, size_t const);
static void
print_component(struct component const * const);
static int
link_components(
		struct component * * const,
		size_t const,
		struct htable * const,
		struct component * const);

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	FILE * const fh = stdin;
	char buf[4096] = {0};

	struct component * components[64] = {0};
	size_t num_components = 0;

	while (1) {
		if (fgets(buf, (int) sizeof(buf), fh) == NULL) {
			if (!feof(fh)) {
				fprintf(stderr, "fgets(): %s\n", strerror(errno));
				destroy_components(components, num_components);
				return 1;
			}
			break;
		}

		char * ptr = buf;
		while (*ptr != '\0') {
			if (*ptr == '\n') {
				*ptr = '\0';
				break;
			}
			ptr++;
		}

		components[num_components] = parse_line(buf, num_components);
		if (!components[num_components]) {
			fprintf(stderr, "parse_line()\n");
			destroy_components(components, num_components);
			return 1;
		}

		num_components++;
	}

	if (DEBUG) {
		for (size_t i = 0; i < num_components; i++) {
			print_component(components[i]);
		}
	}

	struct htable * const h = hash_init(1024);
	if (!h) {
		fprintf(stderr, "hash_init()\n");
		destroy_components(components, num_components);
		return 1;
	}

	int max = 0;
	for (size_t i = 0; i < num_components; i++) {
		struct component * const c = components[i];

		for (size_t j = 0; j < 2; j++) {
			if (c->ports[j].strength != 0) {
				continue;
			}

			c->ports[j].used = true;

			if (!hash_set_i(h, c->id, NULL)) {
				fprintf(stderr, "hash_set_i()\n");
				destroy_components(components, num_components);
				hash_free(h, NULL);
				return 1;
			}

			int const strength = link_components(components, num_components, h, c);
			if (strength == -1) {
				fprintf(stderr, "link_components()\n");
				destroy_components(components, num_components);
				hash_free(h, NULL);
				return 1;
			}

			if (DEBUG) {
				printf("starting with component %d yields strength %d\n", c->id,
						strength);
			}

			if (strength > max) {
				max = strength;
			}

			if (!hash_delete_i(h, c->id, NULL)) {
				fprintf(stderr, "hash_delete_i()\n");
				destroy_components(components, num_components);
				hash_free(h, NULL);
				return 1;
			}

			c->ports[j].used = false;
		}
	}

	printf("%d\n", max);

	destroy_components(components, num_components);
	if (!hash_free(h, NULL)) {
		fprintf(stderr, "hash_free()\n");
		return 1;
	}

	return 0;
}

static void
destroy_components(
		struct component * * const components,
		size_t const num_components)
{
	if (!components) {
		return;
	}

	for (size_t i = 0; i < num_components; i++) {
		free(components[i]);
	}
}

static struct component *
parse_line(char const * const s, size_t const id)
{
	struct component * const c = calloc(1, sizeof(struct component));
	if (!c) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		return NULL;
	}

	c->id = (int) id;

	char const * ptr = s;

	c->ports[0].strength = atoi(ptr);

	while (isdigit(*ptr)) {
		ptr++;
	}

	if (*ptr != '/') {
		fprintf(stderr, "expecting '/': %s\n", ptr);
		free(c);
		return NULL;
	}
	ptr++;

	c->ports[1].strength = atoi(ptr);

	while (isdigit(*ptr)) {
		ptr++;
	}

	if (*ptr != '\0') {
		fprintf(stderr, "expecting end of string: %s\n", ptr);
		free(c);
		return NULL;
	}

	return c;
}

static void
print_component(struct component const * const c)
{
	printf("component %d: %d (%s) / %d (%s)\n",
			c->id,
			c->ports[0].strength,
			c->ports[0].used ? "used" : "unused",
			c->ports[1].strength,
			c->ports[1].used ? "used" : "unused");
}

static int
link_components(
		struct component * * const components,
		size_t const num_components,
		struct htable * const h,
		struct component * const cur)
{
	struct port * unused_port = &cur->ports[0];
	if (unused_port->used) {
		unused_port = &cur->ports[1];
	}

	unused_port->used = true;

	int max = 0;

	for (size_t i = 0; i < num_components; i++) {
		struct component * const c = components[i];

		if (hash_has_key_i(h, c->id)) {
			continue;
		}

		if (!hash_set_i(h, c->id, NULL)) {
			fprintf(stderr, "hash_set_i()\n");
			return -1;
		}

		for (size_t j = 0; j < 2; j++) {
			if (c->ports[j].used) {
				continue;
			}
			if (c->ports[j].strength != unused_port->strength) {
				continue;
			}
			c->ports[j].used = true;

			if (DEBUG) {
				printf("linking component %d (port %d) with component %d (port %d)\n",
						cur->id, unused_port->strength,
						c->id, c->ports[j].strength);
			}

			int const strength = link_components(components, num_components, h, c);
			if (strength == -1) {
				fprintf(stderr, "link_components()\n");
				return -1;
			}

			if (strength > max) {
				max = strength;
			}

			c->ports[j].used = false;
		}

		if (!hash_delete_i(h, c->id, NULL)) {
			fprintf(stderr, "hash_delete_i()\n");
			return -1;
		}
	}

	int strength = 0;
	for (size_t i = 0; i < num_components; i++) {
		struct component * const c = components[i];
		if (c->ports[0].used || c->ports[1].used) {
			strength += c->ports[0].strength;
			strength += c->ports[1].strength;
		}
	}

	if (strength > max) {
		max = strength;
	}

	unused_port->used = false;
	return max;
}
