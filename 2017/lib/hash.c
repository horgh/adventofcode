#include <errno.h>
#include "hash.h"
#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char *
md5(char const * const s)
{
	if (!s) {
		fprintf(stderr, "md5: %s\n", strerror(EINVAL));
		return NULL;
	}

	EVP_MD_CTX * const evp = EVP_MD_CTX_create();
	if (!evp) {
		fprintf(stderr, "unable to create evp ctx\n");
		return NULL;
	}

	EVP_DigestInit(evp, EVP_md5());

	EVP_DigestUpdate(evp, s, strlen(s));

	unsigned char * const buf = calloc(16, sizeof(char));
	if (!buf) {
		fprintf(stderr, "calloc(): %s\n", strerror(errno));
		EVP_MD_CTX_destroy(evp);
		return NULL;
	}

	EVP_DigestFinal_ex(evp, buf, NULL);

	EVP_MD_CTX_destroy(evp);

	return buf;
}

#ifdef TEST_HASH
#include <assert.h>

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	unsigned char * const h = md5("hi");
	char hh[256] = {0};
	size_t hhptr = 0;
	for (size_t i = 0; i < 16; i++) {
		sprintf(hh+hhptr, "%02x", *(h+i));
		hhptr += 2;
	}

	char const expected[] = "49f68a5c8493ec2c0bf489821c21fc3b";
	assert(memcmp(hh, expected, strlen(expected)+1) == 0);

	free(h);

	return 0;
}

#endif
