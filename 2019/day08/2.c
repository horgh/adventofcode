#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Layer {
	int * * pixels;
};

int
main(int const argc, char const * const * const argv)
{
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <width> <height>\n", argv[0]);
		return 1;
	}
	int const width = atoi(argv[1]);
	int const height = atoi(argv[2]);

	while (1) {
		char buf[16384] = {0};
		if (fgets(buf, sizeof(buf), stdin) == NULL) {
			break;
		}

		size_t const max_layers = 1024;
		struct Layer * const layers = calloc(max_layers, sizeof(struct Layer));
		assert(layers != NULL);
		for (size_t i = 0; i < max_layers; i++) {
			layers[i].pixels = calloc((size_t) width, sizeof(int *));
			assert(layers[i].pixels != NULL);
			for (int x = 0; x < width; x++) {
				layers[i].pixels[x] = calloc((size_t) height, sizeof(int));
				assert(layers[i].pixels[x] != NULL);
			}
		}

		size_t n_layers = 0;
		char const * ptr = buf;
		int x = 0, y = 0;
		while (*ptr != '\n') {
			int const i = *ptr - '0';
			ptr++;

			layers[n_layers].pixels[x][y] = i;
			x++;
			if (x == width) {
				x = 0;
				y++;
			}
			if (y == height) {
				x = 0;
				y = 0;
				n_layers++;
				assert(n_layers != max_layers);
			}
		}

		int * * const image = calloc((size_t) width, sizeof(int *));
		assert(image != NULL);
		for (x = 0; x < width; x++) {
			image[x] = calloc((size_t) height, sizeof(int));
			assert(image[x] != NULL);
		}
		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				image[x][y] = -1;
			}
		}

		for (size_t i = 0; i < n_layers; i++) {
			for (y = 0; y < height; y++) {
				for (x = 0; x < width; x++) {
					if (image[x][y] == -1) {
						image[x][y] = layers[i].pixels[x][y];
						continue;
					}
					if (image[x][y] == 0 || image[x][y] == 1) {
						continue;
					}
					if (image[x][y] == 2) {
						image[x][y] = layers[i].pixels[x][y];
						continue;
					}
				}
			}
		}

		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				if (image[x][y] == 2) {
					printf(" ");
					continue;
				}
				if (image[x][y] == 0) {
					//printf(" ");
					printf("\033[30;40m \033[0m");
					continue;
				}
				if (image[x][y] == 1) {
					printf("\033[37;47m \033[0m");
					//printf("X");
					continue;
				}
			}
			printf("\n");
		}
	}

	return 0;
}
