#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aes128.h"
#include "ttables.h"
#include "tests.h"

int main(int argc, char **argv) {
    int repeats = (argc > 1) ? atoi(argv[1]) : 1;
    double total_time = 0.0;
    uint8_t tmp[16];

    // Naive AES-128
    for (int _ = 0; _ < repeats; _++) {
        for (int i = 0; i < 5; i++) {
            memcpy(tmp, input[i], 16);

            // only time the actual encryption
            clock_t start = clock();
            aes128_encrypt(tmp, keys[i]);
            clock_t end = clock();

            total_time += (double)(end - start) / CLOCKS_PER_SEC;

            // sanity check
            if (memcmp(tmp, output[i], 16) != 0) exit(EXIT_FAILURE);
        }
    }
    printf("Total time for %d naive AES-128 encryptions: %.6f seconds\n", repeats*5, total_time);
    printf("Average per 16B block encryption: %.2f microseconds\n", total_time / (5 * repeats) * 1e6);


    // AES-128 with T-Tables
    // init function should not count towards time, can be hardcoded
    total_time = 0.0;
    init_tables();
    for (int _ = 0; _ < repeats; _++) {
        for (int i = 0; i < 5; i++) {
            memcpy(tmp, input[i], 16);

            // helper function should not count towards time, they are just used to repurpose inputs of original AES-128
            state_to_words(tmp);

            // only time the actual encryption
            clock_t start = clock();
            ttables_encrypt(keys[i]);
            clock_t end = clock();

            // repack state back into tmp
            words_to_state(tmp);

            total_time += (double)(end - start) / CLOCKS_PER_SEC;

            // sanity check
            if (memcmp(tmp, output[i], 16) != 0) exit(EXIT_FAILURE);
        }
    }

    printf("Total time for %d T-Tables AES-128 encryptions: %.6f seconds\n", repeats*5, total_time);
    printf("Average per 16B block encryption: %.2f microseconds\n", total_time / (5 * repeats) * 1e6);

    return 0;
}
