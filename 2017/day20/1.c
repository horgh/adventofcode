#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PARTICLES 1024

#define DEBUG 0

struct particle {
	int id;

	int64_t x;
	int64_t y;
	int64_t z;

	int64_t velx;
	int64_t vely;
	int64_t velz;

	int64_t aclx;
	int64_t acly;
	int64_t aclz;
};

static struct particle * parse_line(char const * const);
static void destroy_particles(struct particle ** const);
static void print_particle(struct particle const * const);
static struct particle * find_closest_particle(
		struct particle ** const, size_t const);
static uint64_t manhattan_distance(struct particle const * const);
static void move_particle(struct particle * const);

int
main(int const argc, char const * const * const argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <steps>\n", argv[0]);
		return 1;
	}
	uint64_t const steps = (uint64_t)atoll(argv[1]);

	FILE * const fh = stdin;
	char buf[4096] = {0};

	size_t num_particles = 0;
	struct particle * particles[MAX_PARTICLES] = {0};

	while (1) {
		if (fgets(buf, (int)sizeof(buf), fh) == NULL) {
			if (!feof(fh)) {
				fprintf(stderr, "fgets(): %s\n", strerror(errno));
				destroy_particles(particles);
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

		particles[num_particles] = parse_line(buf);
		if (!particles[num_particles]) {
			fprintf(stderr, "parse_line()\n");
			destroy_particles(particles);
			return 1;
		}

		particles[num_particles]->id = (int)num_particles;

		num_particles++;
	}

	if (DEBUG) {
		for (size_t i = 0; i < num_particles; i++) {
			print_particle(particles[i]);
		}
	}

	struct particle const * closest = NULL;
	for (uint64_t i = 0; i < steps; i++) {
		closest = find_closest_particle(particles, num_particles);
		if (!closest) {
			fprintf(stderr, "find_closest_particle()\n");
			destroy_particles(particles);
			return 1;
		}

		if (DEBUG) {
			printf("step %" PRIu64 ":\n", i);

			for (size_t j = 0; j < num_particles; j++) {
				print_particle(particles[j]);
			}
		}

		for (size_t j = 0; j < num_particles; j++) {
			move_particle(particles[j]);
		}
	}

	printf("%d\n", closest->id);

	destroy_particles(particles);
	return 0;
}

static struct particle *
parse_line(char const * const s)
{
	struct particle * const p = calloc(1, sizeof(struct particle));
	if (!p) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		return NULL;
	}

	char const * ptr = s;

	if (strncmp(ptr, "p=<", 3) != 0) {
		fprintf(stderr, "expecting 'p=<': %s\n", ptr);
		free(p);
		return NULL;
	}
	ptr += 3;

	p->x = atoi(ptr);
	while (isdigit(*ptr) || *ptr == '-') {
		ptr++;
	}

	if (*ptr != ',') {
		fprintf(stderr, "expecting ',': %s\n", ptr);
		free(p);
		return NULL;
	}
	ptr++;

	p->y = atoi(ptr);
	while (isdigit(*ptr) || *ptr == '-') {
		ptr++;
	}

	if (*ptr != ',') {
		fprintf(stderr, "expecting ',': %s\n", ptr);
		free(p);
		return NULL;
	}
	ptr++;

	p->z = atoi(ptr);
	while (isdigit(*ptr) || *ptr == '-') {
		ptr++;
	}

	if (strncmp(ptr, ">, v=<", 6) != 0) {
		fprintf(stderr, "expecting '>, v=<': %s\n", ptr);
		free(p);
		return NULL;
	}
	ptr += 6;

	p->velx = atoi(ptr);
	while (isdigit(*ptr) || *ptr == '-') {
		ptr++;
	}

	if (*ptr != ',') {
		fprintf(stderr, "expecting ',': %s\n", ptr);
		free(p);
		return NULL;
	}
	ptr++;

	p->vely = atoi(ptr);
	while (isdigit(*ptr) || *ptr == '-') {
		ptr++;
	}

	if (*ptr != ',') {
		fprintf(stderr, "expecting ',': %s\n", ptr);
		free(p);
		return NULL;
	}
	ptr++;

	p->velz = atoi(ptr);
	while (isdigit(*ptr) || *ptr == '-') {
		ptr++;
	}

	if (strncmp(ptr, ">, a=<", 6) != 0) {
		fprintf(stderr, "expecting '>, a=<': %s\n", ptr);
		free(p);
		return NULL;
	}
	ptr += 6;

	p->aclx = atoi(ptr);
	while (isdigit(*ptr) || *ptr == '-') {
		ptr++;
	}

	if (*ptr != ',') {
		fprintf(stderr, "expecting ',': %s\n", ptr);
		free(p);
		return NULL;
	}
	ptr++;

	p->acly = atoi(ptr);
	while (isdigit(*ptr) || *ptr == '-') {
		ptr++;
	}

	if (*ptr != ',') {
		fprintf(stderr, "expecting ',': %s\n", ptr);
		free(p);
		return NULL;
	}
	ptr++;

	p->aclz = atoi(ptr);
	while (isdigit(*ptr) || *ptr == '-') {
		ptr++;
	}

	if (*ptr != '>') {
		fprintf(stderr, "expecting '>': %s\n", ptr);
		free(p);
		return NULL;
	}

	return p;
}

static void
destroy_particles(struct particle ** const particles)
{
	if (!particles) {
		return;
	}

	for (size_t i = 0;; i++) {
		struct particle * const p = particles[i];
		if (!p) {
			break;
		}

		free(p);
	}
}

static void
print_particle(struct particle const * const p)
{
	printf("p=<%" PRId64 ",%" PRId64 ",%" PRId64 ">,"
				 " v=<%" PRId64 ",%" PRId64 ",%" PRId64 ">,"
				 " a=<%" PRId64 ",%" PRId64 ",%" PRId64 ">\n",
			p->x,
			p->y,
			p->z,
			p->velx,
			p->vely,
			p->velz,
			p->aclx,
			p->acly,
			p->aclz);
}

static struct particle *
find_closest_particle(
		struct particle ** const particles, size_t const num_particles)
{
	struct particle * closest = NULL;
	uint64_t closest_distance = 0;
	for (size_t i = 0; i < num_particles; i++) {
		uint64_t const distance = manhattan_distance(particles[i]);

		if (i == 0) {
			closest = particles[i];
			closest_distance = distance;
			continue;
		}

		if (distance >= closest_distance) {
			continue;
		}

		closest = particles[i];
		closest_distance = distance;
	}

	return closest;
}

static uint64_t
manhattan_distance(struct particle const * const p)
{
	uint64_t x = 0;
	if (p->x >= 0) {
		x = (uint64_t)p->x;
	} else {
		x = (uint64_t)(p->x * -1);
	}

	uint64_t y = 0;
	if (p->y >= 0) {
		y = (uint64_t)p->y;
	} else {
		y = (uint64_t)(p->y * -1);
	}

	uint64_t z = 0;
	if (p->z >= 0) {
		z = (uint64_t)p->z;
	} else {
		z = (uint64_t)(p->z * -1);
	}

	return x + y + z;
}

static void
move_particle(struct particle * const p)
{
	p->velx += p->aclx;
	p->vely += p->acly;
	p->velz += p->aclz;

	p->x += p->velx;
	p->y += p->vely;
	p->z += p->velz;
}
