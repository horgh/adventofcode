#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static uint64_t
__calc_decompressed_length(const char * const);

int
main(const int argc, const char * const * const argv)
{
	if (argc != 2) {
		printf("Usage: %s <input file>\n", argv[0]);
		return 1;
	}
	const char * const input_file = argv[1];

	FILE * const fh = fopen(input_file, "r");
	if (!fh) {
		printf("Unable to open file: %s: %s\n", input_file, strerror(errno));
		return 1;
	}

	uint64_t decompressed_len = 0;

	while (feof(fh) == 0) {
		char buf[102400];
		memset(buf, 0, sizeof(buf));
		if (fgets(buf, sizeof(buf), fh) == NULL) {
			break;
		}

		if (buf[strlen(buf)-1] == '\n') {
			buf[strlen(buf)-1] = '\0';
		}

		uint64_t line_len = __calc_decompressed_length(buf);
		if (line_len == UINT64_MAX) {
			fclose(fh);
			return 1;
		}

		//printf("=> %s: %" PRIu64 "\n", buf, line_len);

		if (decompressed_len > UINT64_MAX - line_len) {
			printf("overflow\n");
			fclose(fh);
			return 1;
		}

		decompressed_len += line_len;
	}

	if (fclose(fh) != 0) {
		printf("fclose(): %s\n", strerror(errno));
		return 1;
	}

	printf("%" PRIu64 "\n", decompressed_len);

	return 0;
}

static uint64_t
__calc_decompressed_length(const char * const buf)
{
	uint64_t decompressed_len = 0;

	char * s = calloc(strlen(buf)+1, sizeof(char));
	if (!s) {
		printf("%s\n", strerror(ENOMEM));
		return UINT64_MAX;
	}

	for (size_t i = 0; i < strlen(buf); ) {
		if (isspace(buf[i])) {
			i++;
			continue;
		}

		if (buf[i] != '(') {
			decompressed_len++;
			i++;
			continue;
		}

		// When we see a compressed piece, extract its compressed length and
		// calculate the decompressed length of just that piece. Then multiply by
		// how many times this level wants us to repeat it. Do this recursively
		// and we know the actual decompressed length.

		size_t piece_len = 0;
		size_t count = 0;
		int matches = sscanf(buf+i, "(%zux%zu)", &piece_len, &count);
		if (matches != 2) {
			printf("sscanf failed\n");
			free(s);
			return UINT64_MAX;
		}

		while (buf[i] != ')') {
			i++;
		}

		// Advance past the )
		i++;

		memset(s, 0, strlen(buf)+1);
		strncat(s, buf+i, piece_len);

		uint64_t piece_decompressed_len = __calc_decompressed_length(s);
		if (piece_decompressed_len == UINT64_MAX) {
			free(s);
			return UINT64_MAX;
		}

		if (piece_len > UINT64_MAX / count) {
			printf("overflow\n");
			free(s);
			return UINT64_MAX;
		}

		if (decompressed_len > UINT64_MAX - piece_decompressed_len*count) {
			printf("overflow\n");
			free(s);
			return UINT64_MAX;
		}

		decompressed_len += piece_decompressed_len*count;

		i += piece_len;
	}

	free(s);

	//printf("...%s: %" PRIu64 "\n", buf, decompressed_len);

	return decompressed_len;
}
