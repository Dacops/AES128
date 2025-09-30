#ifndef AES128_H
#define AES128_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>

void aes128_encrypt(uint8_t *state, uint8_t *key);

#endif
