#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *
__swap_position(const char * const, const int, const int);
static char *
__swap_letter(const char * const, const char, const char);
static char *
__rotate_left(const char * const, const int);
static char *
__rotate_right(const char * const, const int);
static char *
__rotate_based_on_letter(const char * const, const char);
static char *
__reverse_positions(const char * const, const int, const int);
static char *
__move_position(const char * const, const int, const int);
static char *
__instruction(const char * const, const char * const);

int
main(const int argc, const char * const * const argv)
{
	if (argc != 3) {
		printf("Usage: %s <input file> <input>\n", argv[0]);
		return 1;
	}
	const char * const input_file = argv[1];
	const char * const input = argv[2];

	char * const s0 = __swap_position("abcde", 4, 0);
	assert(strcmp(s0, "ebcda") == 0);

	char * const s1 = __swap_letter("ebcda", 'd', 'b');
	assert(strcmp(s1, "edcba") == 0);

	char * const s2 = __rotate_left("edcba", 1);
	assert(strcmp(s2, "dcbae") == 0);

	char * const s3 = __rotate_right("abcd", 1);
	assert(strcmp(s3, "dabc") == 0);

	char * const s4 = __move_position("abcd", 1, 2);
	assert(strcmp(s4, "acbd") == 0);

	char * const s5 = __move_position("bcdea", 1, 4);
	assert(strcmp(s5, "bdeac") == 0);

	char * const s6 = __move_position("bdeac", 3, 0);
	assert(strcmp(s6, "abdec") == 0);

	char * const s7 = __rotate_based_on_letter("ecabd", 'd');
	assert(strcmp(s7, "decab") == 0);

	free(s0);
	free(s1);
	free(s2);
	free(s3);
	free(s4);
	free(s5);
	free(s6);
	free(s7);

	FILE * const fh = fopen(input_file, "r");
	if (!fh) {
		printf("Unable to open file: %s: %s\n", input_file, strerror(errno));
		return 1;
	}

	char * s = calloc(strlen(input)+1, sizeof(char));
	if (!s) {
		printf("%s\n", strerror(errno));
		fclose(fh);
		return 1;
	}

	memcpy(s, input, strlen(input));

	while (feof(fh) == 0) {
		char buf[1024];
		memset(buf, 0, sizeof(buf));
		if (fgets(buf, sizeof(buf), fh) == NULL) {
			break;
		}

		char * const t = __instruction(s, buf);
		if (!t) {
			printf("__instruction: %s", buf);
			printf("current string: %s\n", s);
			fclose(fh);
			free(s);
			return 1;
		}

		printf("%s", buf);
		printf("%s to %s\n", s, t);

		free(s);
		s = t;
	}

	free(s);

	if (fclose(fh) != 0) {
		printf("fclose(): %s\n", strerror(errno));
		return 1;
	}

	return 0;
}

static char *
__swap_position(const char * const s, const int x, const int y)
{
	char * const n = calloc(strlen(s)+1, sizeof(char));
	if (!n) {
		printf("%s\n", strerror(errno));
		return NULL;
	}

	memcpy(n, s, strlen(s));

	n[x] = s[y];
	n[y] = s[x];

	return n;
}

static char *
__swap_letter(const char * const s, const char x, const char y)
{
	char * const n = calloc(strlen(s)+1, sizeof(char));
	if (!n) {
		printf("%s\n", strerror(errno));
		return NULL;
	}

	for (size_t i = 0; i < strlen(s); i++) {
		if (s[i] == x) {
			n[i] = y;
			continue;
		}

		if (s[i] == y) {
			n[i] = x;
			continue;
		}

		n[i] = s[i];
	}

	return n;
}

static char *
__rotate_left(const char * const s, const int x)
{
	char * const n = calloc(strlen(s)+1, sizeof(char));
	if (!n) {
		printf("%s\n", strerror(errno));
		return NULL;
	}

	memcpy(n, s, strlen(s));

	for (int i = 0; i < x; i++) {
		for (int j = 0; j < (int) strlen(n)-1; j++) {
			char tmp = n[j+1];
			n[j+1] = n[j];
			n[j] = tmp;
		}
	}

	return n;
}

static char *
__rotate_right(const char * const s, const int x)
{
	char * const n = calloc(strlen(s)+1, sizeof(char));
	if (!n) {
		printf("%s\n", strerror(errno));
		return NULL;
	}

	memcpy(n, s, strlen(s));

	for (int i = 0; i < x; i++) {
		for (int j = (int) strlen(n)-1; j > 0; j--) {
			int index = j+1;
			if (index >= (int) strlen(n)) {
				index = 0;
			}
			char tmp = n[index];
			n[index] = n[j];
			n[j] = tmp;
		}
	}

	return n;
}

static char *
__rotate_based_on_letter(const char * const s, const char x)
{
	int index = -1;

	for (int i = 0; i < (int) strlen(s); i++) {
		if (s[i] == x) {
			index = i;
			break;
		}
	}

	if (index == -1) {
		printf("character %c not found\n", x);
		return NULL;
	}

	if (index < 4) {
		return __rotate_right(s, 1+index);
	}

	return __rotate_right(s, 1+index+1);
}

static char *
__reverse_positions(const char * const s, const int x, const int y)
{
	char * const n = calloc(strlen(s)+1, sizeof(char));
	if (!n) {
		printf("%s\n", strerror(errno));
		return NULL;
	}

	memcpy(n, s, strlen(s));

	for (int i = y, j = x; i >= x; i--, j++) {
		n[j] = s[i];
	}

	return n;
}

static char *
__move_position(const char * const s, const int x, const int y)
{
	char * const n = calloc(strlen(s)+1, sizeof(char));
	if (!n) {
		printf("%s\n", strerror(errno));
		return NULL;
	}

	int j = 0;

	for (int i = 0; i < (int) strlen(s); i++) {
		if (i == x) {
			continue;
		}

		if (i == y) {
			if (y > x) {
				n[j] = s[i];
				j++;

				n[j] = s[x];
				j++;
			} else {
				n[j] = s[x];
				j++;

				n[j] = s[i];
				j++;
			}

			continue;
		}

		n[j] = s[i];
		j++;
	}

	if (x == (int) strlen(s)) {
		n[j] = s[x];
	}

	return n;
}

static char *
__instruction(const char * const s, const char * const instruction)
{
	const char * ptr = instruction;

	if (strncmp(ptr, "swap position ", strlen("swap position ")) == 0) {
		ptr += strlen("swap position ");

		int x = -1;
		int matches = sscanf(ptr, "%d", &x);
		if (matches != 1) {
			printf("swap position: missing x\n");
			return NULL;
		}

		while (isdigit(*ptr)) {
			ptr++;
		}

		ptr += strlen(" with position ");

		int y = -1;
		matches = sscanf(ptr, "%d", &y);
		if (matches != 1) {
			printf("swap position: missing y\n");
			return NULL;
		}

		return __swap_position(s, x, y);
	}

	if (strncmp(ptr, "swap letter ", strlen("swap letter ")) == 0) {
		ptr += strlen("swap letter ");

		char x = -1;
		int matches = sscanf(ptr, "%c", &x);
		if (matches != 1) {
			printf("swap letter: missing x\n");
			return NULL;
		}

		ptr++;

		ptr += strlen(" with letter ");

		char y = -1;
		matches = sscanf(ptr, "%c", &y);
		if (matches != 1) {
			printf("swap letter: missing y\n");
			return NULL;
		}

		return __swap_letter(s, x, y);
	}

	if (strncmp(ptr, "rotate left ", strlen("rotate left ")) == 0) {
		ptr += strlen("rotate left ");

		int x = -1;
		int matches = sscanf(ptr, "%d", &x);
		if (matches != 1) {
			printf("rotate left: missing x\n");
			return NULL;
		}

		return __rotate_left(s, x);
	}

	if (strncmp(ptr, "rotate right ", strlen("rotate right ")) == 0) {
		ptr += strlen("rotate right ");

		int x = -1;
		int matches = sscanf(ptr, "%d", &x);
		if (matches != 1) {
			printf("rotate right: missing x\n");
			return NULL;
		}

		return __rotate_right(s, x);
	}

	if (strncmp(ptr, "rotate based on position of letter ",
				strlen("rotate based on position of letter ")) == 0) {
		ptr += strlen("rotate based on position of letter ");

		char x = -1;
		int matches = sscanf(ptr, "%c", &x);
		if (matches != 1) {
			printf("rotate based on position: missing x\n");
			return NULL;
		}

		return __rotate_based_on_letter(s, x);
	}

	if (strncmp(ptr, "reverse positions ", strlen("reverse positions ")) == 0) {
		ptr += strlen("reverse positions ");

		int x = -1;
		int matches = sscanf(ptr, "%d", &x);
		if (matches != 1) {
			printf("reverse positions: missing x: %s", ptr);
			return NULL;
		}

		while (isdigit(*ptr)) {
			ptr++;
		}

		ptr += strlen(" through ");

		int y = -1;
		matches = sscanf(ptr, "%d", &y);
		if (matches != 1) {
			printf("reverse positions: missing y: %s", ptr);
			return NULL;
		}

		return __reverse_positions(s, x, y);
	}

	if (strncmp(ptr, "move position ", strlen("move position ")) == 0) {
		ptr += strlen("move position ");

		int x = -1;
		int matches = sscanf(ptr, "%d", &x);
		if (matches != 1) {
			printf("move position: missing x\n");
			return NULL;
		}

		while (isdigit(*ptr)) {
			ptr++;
		}

		ptr += strlen(" to position ");

		int y = -1;
		matches = sscanf(ptr, "%d", &y);
		if (matches != 1) {
			printf("move position: missing y\n");
			return NULL;
		}

		return __move_position(s, x, y);
	}

	printf("unrecognized instruction\n");
	return NULL;
}
