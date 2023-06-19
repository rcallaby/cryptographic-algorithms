#include <stdint.h>
#include <stdio.h>

#define ROUNDS 20

void salsa20_core(uint32_t* state) {
    int i;
    for (i = 0; i < ROUNDS; i += 2) {
        // Quarter round operations
        state[4] ^= ((state[0] + state[12]) << 7) | ((state[0] + state[12]) >> 25);
        state[8] ^= ((state[4] + state[0]) << 9) | ((state[4] + state[0]) >> 23);
        state[12] ^= ((state[8] + state[4]) << 13) | ((state[8] + state[4]) >> 19);
        state[0] ^= ((state[12] + state[8]) << 18) | ((state[12] + state[8]) >> 14);

        state[9] ^= ((state[5] + state[1]) << 7) | ((state[5] + state[1]) >> 25);
        state[13] ^= ((state[9] + state[5]) << 9) | ((state[9] + state[5]) >> 23);
        state[1] ^= ((state[13] + state[9]) << 13) | ((state[13] + state[9]) >> 19);
        state[5] ^= ((state[1] + state[13]) << 18) | ((state[1] + state[13]) >> 14);

        state[14] ^= ((state[10] + state[6]) << 7) | ((state[10] + state[6]) >> 25);
        state[2] ^= ((state[14] + state[10]) << 9) | ((state[14] + state[10]) >> 23);
        state[6] ^= ((state[2] + state[14]) << 13) | ((state[2] + state[14]) >> 19);
        state[10] ^= ((state[6] + state[2]) << 18) | ((state[6] + state[2]) >> 14);

        state[3] ^= ((state[15] + state[11]) << 7) | ((state[15] + state[11]) >> 25);
        state[7] ^= ((state[3] + state[15]) << 9) | ((state[3] + state[15]) >> 23);
        state[11] ^= ((state[7] + state[3]) << 13) | ((state[7] + state[3]) >> 19);
        state[15] ^= ((state[11] + state[7]) << 18) | ((state[11] + state[7]) >> 14);

        // Row and column rounds
        state[1] ^= ((state[0] + state[3]) << 7) | ((state[0] + state[3]) >> 25);
        state[2] ^= ((state[1] + state[0]) << 9) | ((state[1] + state[0]) >> 23);
        state[3] ^= ((state[2] + state[1]) << 13) | ((state[2] + state[1]) >> 19);
        state[0] ^= ((state[3] + state[2]) << 18) | ((state[3] + state[2]) >> 14);

        state[6] ^= ((state[5] + state[4]) << 7) | ((state[5] + state[4]) >> 25);
        state[7] ^= ((state[6] + state[5]) << 9) | ((state[6] + state[5]) >> 23);
        state[4] ^= ((state[7] + state[6]) << 13) | ((state[7] + state[6]) >> 19);
        state[5] ^= ((state[4] + state[7]) << 18) | ((state[4] + state[7]) >> 14);

        state[11] ^= ((state[10] + state[9]) << 7) | ((state[10] + state[9]) >> 25);
        state[8] ^= ((state[11] + state[10]) << 9) | ((state[11] + state[10]) >> 23);
        state[9] ^= ((state[8] + state[11]) << 13) | ((state[8] + state[11]) >> 19);
        state[10] ^= ((state[9] + state[8]) << 18) | ((state[9] + state[8]) >> 14);

        state[12] ^= ((state[15] + state[14]) << 7) | ((state[15] + state[14]) >> 25);
        state[13] ^= ((state[12] + state[15]) << 9) | ((state[12] + state[15]) >> 23);
        state[14] ^= ((state[13] + state[12]) << 13) | ((state[13] + state[12]) >> 19);
        state[15] ^= ((state[14] + state[13]) << 18) | ((state[14] + state[13]) >> 14);
    }
}

void salsa20_encrypt(const uint8_t* input, uint8_t* output, const uint32_t* key, const uint32_t* nonce) {
    uint32_t state[16] = {
        0x61707865, key[0], key[1], key[2], key[3], 0x3320646e, nonce[0], nonce[1],
        0, 0, 0x79622d32, 0x6b206574, 0x6574616e, 0x79742d32, 0x7265746e, 0x64203333
    };

    uint32_t block[16];
    int i;

    while (*input) {
        // Generate a 64-byte block
        for (i = 0; i < 16; i++) {
            block[i] = ((uint32_t)input[4 * i]) |
                       ((uint32_t)input[4 * i + 1] << 8) |
                       ((uint32_t)input[4 * i + 2] << 16) |
                       ((uint32_t)input[4 * i + 3] << 24);
        }

        salsa20_core(state);

        for (i = 0; i < 16; i++) {
            block[i] += state[i];
        }

        // Copy the encrypted block to the output
        for (i = 0; i < 16; i++) {
            output[4 * i] = block[i] & 0xFF;
            output[4 * i + 1] = (block[i] >> 8) & 0xFF;
            output[4 * i + 2] = (block[i] >> 16) & 0xFF;
            output[4 * i + 3] = (block[i] >> 24) & 0xFF;
        }

        input += 64;
        output += 64;
    }
}

int main() {
    uint8_t input[256] = "Hello, Salsa20!";
    uint8_t output[256] = {0};
    uint32_t key[8] = {0x01020304, 0x05060708, 0x090a0b0c, 0x0d0e0f10, 0x11121314, 0x15161718, 0x191a1b1c, 0x1d1e1f20};
    uint32_t nonce[2] = {0x00000000, 0x00000001};

    salsa20_encrypt(input, output, key, nonce);

    printf("Encrypted: %s\n", output);

    return 0;
}
