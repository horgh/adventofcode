#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *
__decompress(char *);

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

	int len = 0;

	while (feof(fh) == 0) {
		char buf[102400];
		memset(buf, 0, sizeof(buf));
		if (fgets(buf, sizeof(buf), fh) == NULL) {
			break;
		}

		if (buf[strlen(buf)-1] == '\n') {
			buf[strlen(buf)-1] = '\0';
		}

		char * uncompressed = __decompress(buf);
		if (!uncompressed) {
			fclose(fh);
			return 1;
		}

		printf("%s %zu\n", uncompressed, strlen(uncompressed));

		for (size_t i = 0; i < strlen(uncompressed); i++) {
			if (isspace(uncompressed[i])) {
				continue;
			}
			len++;
		}

		free(uncompressed);
	}

	if (fclose(fh) != 0) {
		printf("fclose(): %s\n", strerror(errno));
		return 1;
	}

	printf("%d\n", len);

	return 0;
}

static char *
__decompress(char * buf)
{
	size_t sz = 102400;
	char * uncompressed = calloc(sz+1, sizeof(char));
	if (!uncompressed) {
		printf("%s\n", strerror(ENOMEM));
		return NULL;
	}

	for (size_t i = 0; i < strlen(buf); ) {
		size_t len = 0;
		size_t count = 0;
		int matches = sscanf(buf+i, "(%zux%zu)", &len, &count);
		if (matches != 2) {
			strncat(uncompressed, buf+i, 1);
			i++;
			continue;
		}

		while (buf[i] != ')') {
			i++;
		}

		// Move past the )
		i++;

		char * s = calloc(sz+1, sizeof(char));
		if (!s) {
			printf("%s\n", strerror(ENOMEM));
			free(uncompressed);
			return NULL;
		}

		strncat(s, buf+i, len);

		for (size_t j = 0; j < count; j++) {
			strcat(uncompressed, s);
		}

		free(s);

		i += len;
	}

	return uncompressed;
}
