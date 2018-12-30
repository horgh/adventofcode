#include <errno.h>
#include <openssl/evp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char * __md5(const char * const);

int
main(const int argc, const char * const * const argv)
{
	if (argc != 2) {
		printf("Usage: %s <input string>\n", argv[0]);
		return 1;
	}
	const char * const input = argv[1];
	// printf("Input: %s\n", input);

	// unsigned char * const hashtest = __md5(input);
	// printf("md5 of input: ");
	// for (int i = 0; i < 16; i++) {
	//	printf("%02x", hashtest[i]);
	//}
	// printf("\n");

	char * password = calloc(strlen(input) + 1, sizeof(char));
	if (!password) {
		printf("%s\n", strerror(ENOMEM));
		return 1;
	}

	int hash_index = 0;
	for (size_t i = 0; i < strlen(input); i++) {
		while (1) {
			char raw[1024];
			memset(raw, 0, sizeof(raw));
			snprintf(raw, sizeof(raw), "%s%d", input, hash_index);

			unsigned char * const hash = __md5(raw);
			if (!hash) {
				free(password);
				return 1;
			}

			if ((~hash[0] & 0xff) == 0xff && (~hash[1] & 0xff) == 0xff &&
					(~hash[2] & 0xf0) == 0xf0) {
				memset(raw, 0, sizeof(raw));
				snprintf(raw, sizeof(raw), "%02x", hash[2]);
				password[i] = raw[1];
				// printf("Character found. String index: %zu Interesting byte: %s Hash
				// index: %d Password so far: %s\n",
				//		i, raw, hash_index, password);
				free(hash);
				hash_index++;
				break;
			}

			free(hash);
			hash_index++;

			// if (hash_index % 100000 == 0) {
			//	printf("%d... (%zu)\n", hash_index, i);
			//}
		}
	}

	printf("%s\n", password);
	free(password);
	return 0;
}

static unsigned char *
__md5(const char * const s)
{
	if (!s) {
		printf("__md5: %s\n", strerror(EINVAL));
		return NULL;
	}

	EVP_MD_CTX * const evp = EVP_MD_CTX_create();
	if (!evp) {
		printf("Unable to create evp ctx\n");
		return NULL;
	}

	EVP_DigestInit(evp, EVP_md5());

	EVP_DigestUpdate(evp, s, strlen(s));

	unsigned char * const buf = calloc(16, sizeof(char));
	if (!buf) {
		printf("%s\n", strerror(ENOMEM));
		EVP_MD_CTX_destroy(evp);
		return NULL;
	}

	EVP_DigestFinal_ex(evp, buf, NULL);

	// EVP_MD_CTX_cleanup();
	EVP_MD_CTX_destroy(evp);

	return buf;
}
