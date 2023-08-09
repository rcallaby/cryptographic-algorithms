#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef uint64_t u64;
typedef uint32_t u32;

#define ROL(x, n) (((x) << (n)) | ((x) >> (64 - (n))))

void threefish256_encrypt(const u64 key[5], const u64 plaintext[4], u64 ciphertext[4]) {
    u64 state[4];
    memcpy(state, plaintext, sizeof(u64) * 4);

    for (int round = 0; round < 72; ++round) {
        u64 t = state[1] + state[0] + key[round % 5];
        state[0] = ROL(state[0], 24) + t;
        state[1] = ROL(state[1], 9) + state[0];
        state[2] = ROL(state[2], 39) ^ state[1];
        state[3] = ROL(state[3], 33) ^ state[2];
        
        if (round % 4 == 0) {
            u32 temp = state[3];
            state[3] = state[2];
            state[2] = temp;
        }
        
        if (round % 4 == 2) {
            state[0] += key[(round / 4) % 5];
            state[1] += key[(round / 4 + 1) % 5];
            state[2] += key[(round / 4 + 2) % 5] + round / 4;
            state[3] += key[(round / 4 + 3) % 5] + round;
        }
    }

    for (int i = 0; i < 4; ++i) {
        ciphertext[i] = state[i] + key[i];
    }
}

void threefish256_decrypt(const u64 key[5], const u64 ciphertext[4], u64 plaintext[4]) {
    u64 state[4];
    memcpy(state, ciphertext, sizeof(u64) * 4);

    for (int round = 71; round >= 0; --round) {
        if (round % 4 == 2) {
            state[0] -= key[(round / 4) % 5];
            state[1] -= key[(round / 4 + 1) % 5];
            state[2] -= key[(round / 4 + 2) % 5] + round / 4;
            state[3] -= key[(round / 4 + 3) % 5] + round;
        }
        
        if (round % 4 == 0) {
            u32 temp = state[3];
            state[3] = state[2];
            state[2] = temp;
        }

        state[3] ^= state[2];
        state[2] = ROL(state[2], 31);
        state[3] = ROL(state[3], 27);
        state[0] = ROL(state[0] - state[1], 10);
        state[1] = ROL(state[1] - state[2], 25);
        state[0] -= state[3] + state[1] + key[round % 5];
    }

    for (int i = 0; i < 4; ++i) {
        plaintext[i] = state[i] - key[i];
    }
}

int main() {
    u64 key[5] = {0x0123456789abcdef, 0xfedcba9876543210, 0x13579bdf2468ace0, 0xacedf1357924680b, 0x2468ace0acedf135};
    u64 plaintext[4] = {0x0123456789abcdef, 0xfedcba9876543210, 0x13579bdf2468ace0, 0xacedf1357924680b};
    u64 ciphertext[4];

    threefish256_encrypt(key, plaintext, ciphertext);

    printf("Ciphertext: ");
    for (int i = 0; i < 4; ++i) {
        printf("%016llx ", ciphertext[i]);
    }
    printf("\n");

    return 0;
}



