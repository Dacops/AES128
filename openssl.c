#include "openssl.h"

void openssl_aes128_aes(uint8_t *state, uint8_t *key) {
    AES_KEY aes_key;
    AES_set_encrypt_key(key, 128, &aes_key);
    AES_ecb_encrypt(state, state, &aes_key, AES_ENCRYPT);
}

void openssl_aes128_evp(uint8_t *state, uint8_t *key) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    int len;
    EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, NULL);
    EVP_CIPHER_CTX_set_padding(ctx, 0);

    uint8_t out[16];
    EVP_EncryptUpdate(ctx, out, &len, state, 16);

    int len_final = 0;
    EVP_EncryptFinal_ex(ctx, out + len, &len_final);

    memcpy(state, out, 16);

    EVP_CIPHER_CTX_free(ctx);
}
