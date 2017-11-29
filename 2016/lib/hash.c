#include <errno.h>
#include "hash.h"
#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char *
md5(const char * const s)
{
	if (!s) {
		printf("md5: %s\n", strerror(EINVAL));
		return NULL;
	}

	EVP_MD_CTX * const evp = EVP_MD_CTX_create();
	if (!evp) {
		printf("unable to create evp ctx\n");
		return NULL;
	}

	EVP_DigestInit(evp, EVP_md5());

	EVP_DigestUpdate(evp, s, strlen(s));

	unsigned char * const buf = calloc(16, sizeof(char));
	if (!buf) {
		printf("%s\n", strerror(errno));
		EVP_MD_CTX_destroy(evp);
		return NULL;
	}

	EVP_DigestFinal_ex(evp, buf, NULL);

	EVP_MD_CTX_destroy(evp);

	return buf;
}
