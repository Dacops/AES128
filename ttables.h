#ifndef TTABLES_H
#define TTABLES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void init_tables();
void state_to_words(uint8_t *state);
void words_to_state(uint8_t *state);
void ttables_encrypt(uint8_t *key);

#endif
