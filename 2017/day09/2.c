#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Score {
	char const * s;
	int score;
};

static struct Score *
get_score(char const * const);

int main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	FILE * const fh = stdin;

	char buf[20480] = {0};

	if (fgets(buf, (int) sizeof(buf), fh) == NULL) {
		fprintf(stderr, "fgets(): %s\n", strerror(errno));
		return 1;
	}

	struct Score * const score = get_score(buf);
	printf("%d\n", score->score);
	free(score);
	return 0;
}

static struct Score *
get_score(char const * const s)
{
	struct Score * const score = calloc(1, sizeof(struct Score));
	if (!score) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		return NULL;
	}
	score->s = s;

	while (1) {
		if (*score->s == '\0' || *score->s == '\n') {
			return score;
		}

		if (*score->s == '{') {
			score->s++;
			struct Score * const score2 = get_score(score->s);
			score->score += score2->score;
			score->s = score2->s;
			free(score2);
			continue;
		}

		if (*score->s == '}') {
			score->s++;
			return score;
		}

		if (*score->s == '<') {
			score->s++;
			while (1) {
				if (*score->s == '!') {
					score->s++;
					score->s++;
					continue;
				}
				if (*score->s == '>') {
					score->s++;
					break;
				}
				score->score++;
				score->s++;
			}
			continue;
		}

		if (*score->s == ',') {
			score->s++;
			continue;
		}

		fprintf(stderr, "unexpected character %c\n", *score->s);
		return score;
	}

	fprintf(stderr, "should not hit\n");
	return NULL;
}
