#ifndef AESNI_H
#define AESNI_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wmmintrin.h>
#include <arpa/inet.h>

void aesni_encrypt(uint8_t *state, uint8_t *key);
void matrix_to_linear(uint8_t *key);

#endif
