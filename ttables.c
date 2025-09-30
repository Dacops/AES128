#include "ttables.h"

//-------------------------------------- KeyExpansion() logic -------------------------------------

#define Nk 4
#define Nr 10

// mapping is 1-indexed instead of 0-indexed, 0x00 just blocks the 0-index
static const uint8_t rcon[11] = {
    0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

static const uint8_t sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

// [a0, a1, a2, a3] -> [a1, a2, a3, a0]
#define ROTWORD(word) (((word) << 8) | ((word) >> 24))

// [a0, a1, a2, a3] -> [sbox(a0), sbox(a1), sbox(a2), sbox(a3)]
#define SUBWORD(word) ( \
    ((uint32_t)sbox[((word) >> 24) & 0xFF] << 24) | \
    ((uint32_t)sbox[((word) >> 16) & 0xFF] << 16) | \
    ((uint32_t)sbox[((word) >>  8) & 0xFF] <<  8) | \
    ((uint32_t)sbox[(word) & 0xFF]) \
)

//-------------------------------------- Main Encryption Logic -------------------------------------

#define GF_MUL2(x) (((x) << 1) ^ (((x) >> 7) * 0x1b))
#define GF_MUL3(x) (GF_MUL2(x) ^ (x))

uint32_t T0[256], T1[256], T2[256], T3[256];
uint32_t U0[256], U1[256], U2[256], U3[256];
uint32_t s0, s1, s2, s3;

void init_tables() {
    for (int x = 0; x < 256; x++) {
        uint8_t s1 = sbox[x];
        uint8_t s2 = GF_MUL2(s1), s3 = GF_MUL3(s1);

        T0[x] = ((uint32_t)s2 << 24) | ((uint32_t)s1 << 16) | ((uint32_t)s1 << 8) | (uint32_t)s3;
        T1[x] = ((uint32_t)s3 << 24) | ((uint32_t)s2 << 16) | ((uint32_t)s1 << 8) | (uint32_t)s1;
        T2[x] = ((uint32_t)s1 << 24) | ((uint32_t)s3 << 16) | ((uint32_t)s2 << 8) | (uint32_t)s1;
        T3[x] = ((uint32_t)s1 << 24) | ((uint32_t)s1 << 16) | ((uint32_t)s3 << 8) | (uint32_t)s2;

        U0[x] = (uint32_t)s1 << 24;
        U1[x] = (uint32_t)s1 << 16;
        U2[x] = (uint32_t)s1 << 8;
        U3[x] = (uint32_t)s1;
    }
}

void state_to_words(uint8_t *state) {
    s0 = ((uint32_t)state[ 0] << 24) | ((uint32_t)state[ 4] << 16) | ((uint32_t)state[ 8] << 8) | ((uint32_t)state[12]);
    s1 = ((uint32_t)state[ 1] << 24) | ((uint32_t)state[ 5] << 16) | ((uint32_t)state[ 9] << 8) | ((uint32_t)state[13]);
    s2 = ((uint32_t)state[ 2] << 24) | ((uint32_t)state[ 6] << 16) | ((uint32_t)state[10] << 8) | ((uint32_t)state[14]);
    s3 = ((uint32_t)state[ 3] << 24) | ((uint32_t)state[ 7] << 16) | ((uint32_t)state[11] << 8) | ((uint32_t)state[15]);
}

void words_to_state(uint8_t *state) {
    state[ 0] = (s0 >> 24) & 0xFF; state[ 4] = (s0 >> 16) & 0xFF; state[ 8] = (s0 >> 8) & 0xFF; state[12] = s0 & 0xFF;
    state[ 1] = (s1 >> 24) & 0xFF; state[ 5] = (s1 >> 16) & 0xFF; state[ 9] = (s1 >> 8) & 0xFF; state[13] = s1 & 0xFF;
    state[ 2] = (s2 >> 24) & 0xFF; state[ 6] = (s2 >> 16) & 0xFF; state[10] = (s2 >> 8) & 0xFF; state[14] = s2 & 0xFF;
    state[ 3] = (s3 >> 24) & 0xFF; state[ 7] = (s3 >> 16) & 0xFF; state[11] = (s3 >> 8) & 0xFF; state[15] = s3 & 0xFF;
}

#define ADDROUNDKEYS(roundkey)      \
    do {                            \
        s0 ^= roundkey[0];          \
        s1 ^= roundkey[1];          \
        s2 ^= roundkey[2];          \
        s3 ^= roundkey[3];          \
    } while(0)

uint32_t* aes128_key_expansion32(uint8_t *key) {
    uint32_t *w = malloc(4 * (Nr+1) * sizeof(uint32_t));
    uint32_t temp;

    for (int i = 0; i < Nk; i++) {
        w[i] = ((uint32_t)key[ 0+i] << 24) |
               ((uint32_t)key[ 4+i] << 16) |
               ((uint32_t)key[ 8+i] <<  8) |
               ((uint32_t)key[12+i]);
    }

    for (int i = Nk; i < 4*(Nr+1); i++) {
        temp = w[i-1];
        if (i % Nk == 0) {
            temp = SUBWORD(ROTWORD(temp)) ^ ((uint32_t)rcon[i/Nk] << 24);
        }
        w[i] = w[i-Nk] ^ temp;
    }

    return w;
}

// state saved in s0, s1, s2, s3
void ttables_encrypt(uint8_t *key) {
    uint32_t *w = aes128_key_expansion32(key);
    ADDROUNDKEYS(w);

    uint32_t t0, t1, t2, t3;
    for (int round = 1; round < Nr; round++) {
        t0 = T0[s0 >> 24] ^ T1[(s1 >> 16) & 0xFF] ^ T2[(s2 >> 8) & 0xFF] ^ T3[s3 & 0xFF] ^ w[4*round + 0];
        t1 = T0[s1 >> 24] ^ T1[(s2 >> 16) & 0xFF] ^ T2[(s3 >> 8) & 0xFF] ^ T3[s0 & 0xFF] ^ w[4*round + 1];
        t2 = T0[s2 >> 24] ^ T1[(s3 >> 16) & 0xFF] ^ T2[(s0 >> 8) & 0xFF] ^ T3[s1 & 0xFF] ^ w[4*round + 2];
        t3 = T0[s3 >> 24] ^ T1[(s0 >> 16) & 0xFF] ^ T2[(s1 >> 8) & 0xFF] ^ T3[s2 & 0xFF] ^ w[4*round + 3];

        s0 = t0; s1 = t1; s2 = t2; s3 = t3;
    }
    
    t0 = U0[s0 >> 24] ^ U1[(s1 >> 16) & 0xFF] ^ U2[(s2 >> 8) & 0xFF] ^ U3[s3 & 0xFF] ^ w[4*Nr + 0];
    t1 = U0[s1 >> 24] ^ U1[(s2 >> 16) & 0xFF] ^ U2[(s3 >> 8) & 0xFF] ^ U3[s0 & 0xFF] ^ w[4*Nr + 1];
    t2 = U0[s2 >> 24] ^ U1[(s3 >> 16) & 0xFF] ^ U2[(s0 >> 8) & 0xFF] ^ U3[s1 & 0xFF] ^ w[4*Nr + 2];
    t3 = U0[s3 >> 24] ^ U1[(s0 >> 16) & 0xFF] ^ U2[(s1 >> 8) & 0xFF] ^ U3[s2 & 0xFF] ^ w[4*Nr + 3];

    s0 = t0; s1 = t1; s2 = t2; s3 = t3;
}