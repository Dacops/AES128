#ifndef OPENSSL_H
#define OPENSSL_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <openssl/aes.h>
#include <openssl/evp.h>

void openssl_aes128_aes(uint8_t *state, uint8_t *key);
void openssl_aes128_evp(uint8_t *state, uint8_t *key);

#endif
