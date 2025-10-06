#include "aesni.h"

//-------------------------------------- KeyExpansion() logic -------------------------------------

#define Nr 10

static inline __m128i AES_128_ASSIST (__m128i temp1, __m128i temp2) {
    __m128i temp3;
    temp2 = _mm_shuffle_epi32 (temp2 ,0xff);
    temp3 = _mm_slli_si128 (temp1, 0x4);
    temp1 = _mm_xor_si128 (temp1, temp3);
    temp3 = _mm_slli_si128 (temp3, 0x4);
    temp1 = _mm_xor_si128 (temp1, temp3);
    temp3 = _mm_slli_si128 (temp3, 0x4);
    temp1 = _mm_xor_si128 (temp1, temp3);
    temp1 = _mm_xor_si128 (temp1, temp2);
    return temp1;
}

void AES_128_Key_Expansion (const unsigned char *userkey, unsigned char *key) {
    __m128i temp1, temp2;
    __m128i *Key_Schedule = (__m128i*)key;

    temp1 = _mm_loadu_si128((__m128i*)userkey);
    Key_Schedule[0] = temp1;

    temp2 = _mm_aeskeygenassist_si128 (temp1 ,0x1);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[1] = temp1;

    temp2 = _mm_aeskeygenassist_si128 (temp1,0x2);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[2] = temp1;

    temp2 = _mm_aeskeygenassist_si128 (temp1,0x4);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[3] = temp1;

    temp2 = _mm_aeskeygenassist_si128 (temp1,0x8);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[4] = temp1;

    temp2 = _mm_aeskeygenassist_si128 (temp1,0x10);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[5] = temp1;

    temp2 = _mm_aeskeygenassist_si128 (temp1,0x20);
    temp1 = AES_128_ASSIST(temp1, temp2);  
    Key_Schedule[6] = temp1;

    temp2 = _mm_aeskeygenassist_si128 (temp1,0x40);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[7] = temp1;

    temp2 = _mm_aeskeygenassist_si128 (temp1,0x80);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[8] = temp1;

    temp2 = _mm_aeskeygenassist_si128 (temp1,0x1b);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[9] = temp1;

    temp2 = _mm_aeskeygenassist_si128 (temp1,0x36);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[10] = temp1;
 } 


//-------------------------------------- Main Encryption Logic -------------------------------------

void matrix_to_linear(uint8_t *key) {
    uint8_t tmp[16];

    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            tmp[r*4 + c] = key[c*4 + r];
        }
    }

    for (int i = 0; i < 16; i++) {
        key[i] = tmp[i];
    }
}

void aesni_encrypt(uint8_t *state, uint8_t *key) {
    uint8_t key_schedule[16 * (Nr + 1)]; // uint8_t is the same as unsigned char
    AES_128_Key_Expansion(key, key_schedule);

    __m128i block = _mm_loadu_si128((__m128i*)state);

    // 1st ADDROUNDKEY = xor 
    block = _mm_xor_si128(block, ((__m128i*)key_schedule)[0]);

    for (int i = 1; i < Nr; i++) {
        // AESNI for SUBBYTES + SHIFTROWS + MIXCOLUMNS + ADDROUNDKEY
        block = _mm_aesenc_si128(block, ((__m128i*)key_schedule)[i]);
    }

    // AESNI for SUBBYTES + SHIFTROWS + ADDROUNDKEY
    block = _mm_aesenclast_si128(block, ((__m128i*)key_schedule)[Nr]);

    _mm_storeu_si128((__m128i*)state, block);
}