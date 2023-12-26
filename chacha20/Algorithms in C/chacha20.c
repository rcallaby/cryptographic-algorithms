#include <stdio.h>
#include <stdint.h>
#include <string.h>

// ChaCha20 quarter round function
#define QR(a, b, c, d) \
    a += b; d ^= a; d = (d << 16) | (d >> 16); \
    c += d; b ^= c; b = (b << 12) | (b >> 20); \
    a += b; d ^= a; d = (d << 8) | (d >> 24); \
    c += d; b ^= c; b = (b << 7) | (b >> 25);

// ChaCha20 block function
void chacha20_block(uint32_t state[16]) {
    int i;
    for (i = 0; i < 10; ++i) {
        QR(state[0], state[4], state[8], state[12]);
        QR(state[1], state[5], state[9], state[13]);
        QR(state[2], state[6], state[10], state[14]);
        QR(state[3], state[7], state[11], state[15]);
        QR(state[0], state[5], state[10], state[15]);
        QR(state[1], state[6], state[11], state[12]);
        QR(state[2], state[7], state[8], state[13]);
        QR(state[3], state[4], state[9], state[14]);
    }
}

// ChaCha20 encryption function
void chacha20_encrypt(const uint8_t key[32], const uint8_t nonce[8], const uint32_t counter, uint8_t output[64]) {
    // Constants
    const char sigma[16] = "expand 32-byte k";
    uint32_t state[16];

    // Initialize state
    state[0] = ((uint32_t *)sigma)[0];
    state[1] = ((uint32_t *)sigma)[1];
    state[2] = ((uint32_t *)sigma)[2];
    state[3] = ((uint32_t *)sigma)[3];
    state[4] = ((uint32_t *)key)[0];
    state[5] = ((uint32_t *)key)[1];
    state[6] = ((uint32_t *)key)[2];
    state[7] = ((uint32_t *)key)[3];
    state[8] = ((uint32_t *)key)[4];
    state[9] = ((uint32_t *)key)[5];
    state[10] = ((uint32_t *)key)[6];
    state[11] = ((uint32_t *)key)[7];
    state[12] = counter;
    state[13] = ((uint32_t *)nonce)[0];
    state[14] = ((uint32_t *)nonce)[1];
    state[15] = ((uint32_t *)nonce)[2];

    // Copy state to output
    for (int i = 0; i < 16; ++i) {
        ((uint32_t *)output)[i] = state[i];
    }

    // Perform ChaCha20 block operations
    chacha20_block(state);

    // Add the original state to the result
    for (int i = 0; i < 16; ++i) {
        ((uint32_t *)output)[i] += state[i];
    }
}

// ChaCha20 decryption function
void chacha20_decrypt(const uint8_t key[32], const uint8_t nonce[8], const uint32_t counter, const uint8_t ciphertext[64], uint8_t plaintext[64]) {
    // Decrypting ChaCha20 is the same as encrypting
    chacha20_encrypt(key, nonce, counter, plaintext);

    // XOR the ciphertext with the plaintext to get the original message
    for (int i = 0; i < 64; ++i) {
        plaintext[i] ^= ciphertext[i];
    }
}

int main() {
    // Key and nonce for ChaCha20
    uint8_t key[32] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };

    uint8_t nonce[8] = {
        0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x4a
    };

    // Counter (can be incremented for each block)
    uint32_t counter = 0;

    // Buffer for ChaCha20 output
    uint8_t output[64];

    // Plaintext message
    const char *plaintext = "Hello, ChaCha20!";
    size_t text_len = strlen(plaintext);

    // Buffer for ciphertext
    uint8_t ciphertext[text_len];

    // Encrypt the plaintext
    chacha20_encrypt(key, nonce, counter, ciphertext);

    // Print the ciphertext
    printf("Ciphertext: ");
    for (size_t i = 0; i < sizeof(ciphertext); ++i) {
        printf("%02x", ciphertext[i]);
    }
    printf("\n");

    // Buffer for decrypted plaintext
    uint8_t decrypted[text_len];

    // Decrypt the ciphertext
    chacha20_decrypt(key, nonce, counter, ciphertext, decrypted);

    // Print the decrypted plaintext
    printf("Decrypted: ");
    for (size_t i = 0; i < text_len; ++i) {
        printf("%c", decrypted[i]);
    }
    printf("\n");

    return 0;
}

