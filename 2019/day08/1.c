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

		int min = -1;
		size_t min_layer = 0;
		for (size_t i = 0; i < n_layers; i++) {
			int count = 0;
			for (y = 0; y < height; y++) {
				for (x = 0; x < width; x++) {
					if (layers[i].pixels[x][y] == 0) {
						count++;
					}
				}
			}
			if (min == -1 || count < min) {
				min = count;
				min_layer = i;
			}
		}

		int nums[10] = {0};
		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				nums[layers[min_layer].pixels[x][y]]++;
			}
		}
		printf("%d\n", nums[1]*nums[2]);
	}

	return 0;
}
