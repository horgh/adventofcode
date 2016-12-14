#ifndef __SIPHASH24_H
#define __SIPHASH24_H

#include <stdint.h>

int siphash(uint8_t *out, const uint8_t *in, uint64_t inlen, const uint8_t *k);

#endif
