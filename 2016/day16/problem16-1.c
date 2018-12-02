#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *
__fill_disk(const char * const, const int);
static char *
__make_output(const char * const, const int);
static char *
__reverse(char * const);
static char *
__make_checksum(const char * const, const int);

int
main(const int argc, const char * const * const argv)
{
	if (argc != 3) {
		printf("Usage: %s <input> <wanted length>\n", argv[0]);
		return 1;
	}
	const char * const input = argv[1];
	const int wanted_length = atoi(argv[2]);

	char * const checksum = __fill_disk(input, wanted_length);
	assert(checksum != NULL);
	printf("%s\n", checksum);
	free(checksum);

	return 0;
}

static char *
__fill_disk(const char * const input, const int wanted_length)
{
	//printf("making output...\n");
	char * const output = __make_output(input, wanted_length);
	assert(output != NULL);
	output[wanted_length] = 0;
	//printf("output: %s length %zu\n", output, strlen(output));

	//printf("making checksum...\n");
	char * const checksum = __make_checksum(output, wanted_length);
	free(output);

	return checksum;
}

static char *
__make_output(const char * const input, const int wanted_length)
{
	char * output = calloc(strlen(input)+1, sizeof(char));
	assert(output != NULL);
	memcpy(output, input, strlen(input));

	// Avoid as much use of strlen as we can. Slow. Weird!
	size_t len = strlen(output);

	while (len < (size_t) wanted_length) {
		//printf("output %zu...\n", len);

		char * const b = __reverse(output);
		assert(b != NULL);

		for (size_t i = 0; i < len; i++) {
			if (b[i] == '0') {
				b[i] = '1';
			} else {
				b[i] = '0';
			}
		}

		char * const new_output = calloc(len+len+1+1,
				sizeof(char));
		assert(new_output != NULL);

		strcat(new_output, output);
		strcat(new_output, "0");
		strcat(new_output, b);

		free(b);
		free(output);

		output = new_output;
		len += len+1;
	}

	return output;
}

static char *
__reverse(char * const s)
{
	char * const r = calloc(strlen(s)+1, sizeof(char));
	assert(r != NULL);

	int j = 0;
	for (int i = (int) strlen(s)-1; i >= 0; i--) {
		r[j] = s[i];
		j++;
	}

	return r;
}

static char *
__make_checksum(const char * const s, const int wanted_length)
{
	const char * ptr = s;

	// Avoid use of strlen. Slow. Weird!
	size_t sz = strlen(s);

	char * const checksum = calloc(sz+1, sizeof(char));
	assert(checksum != NULL);

	size_t j = 0;
	for (size_t i = 0; i < sz-1; i += 2) {
		if (ptr[i] == '0' && ptr[i+1] == '0') {
			checksum[j] = '1';
			j++;
			continue;
		}

		if (ptr[i] == '1' && ptr[i+1] == '1') {
			checksum[j] = '1';
			j++;
			continue;
		}

		checksum[j] = '0';
		j++;
	}

	//printf("checksum %s (length %zu)\n", checksum, strlen(checksum));
	if (strlen(checksum) % 2 == 0) {
		char * const new_checksum = __make_checksum(checksum, wanted_length);
		free(checksum);
		return new_checksum;
	}

	return checksum;
}
