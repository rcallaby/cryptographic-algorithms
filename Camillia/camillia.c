#include <stdint.h>
#include <stdio.h>

typedef uint8_t BYTE;
typedef uint32_t WORD;

#define CAMELLIA_BLOCK_SIZE 16
#define CAMELLIA_ROUNDS 18

#define GETU32(pt) (((WORD)(pt)[0] << 24) ^ ((WORD)(pt)[1] << 16) ^ ((WORD)(pt)[2] << 8) ^ ((WORD)(pt)[3]))
#define PUTU32(ct, st) { (ct)[0] = (BYTE)((st) >> 24); (ct)[1] = (BYTE)((st) >> 16); (ct)[2] = (BYTE)((st) >> 8); (ct)[3] = (BYTE)(st); }

static const WORD SIGMA[] = {
    0xa09e667f, 0x3bcc908b, 0xb67ae858, 0x4caa73b2,
    0xc6ef372f, 0xe94f82be, 0x54ff53a5, 0xf1d36f1c
};

void camellia_subkey(WORD *KL, WORD *KR, const BYTE *mk) {
    WORD ka[4], kb[4], t0, t1, w0, w1;
    int i;

    ka[0] = GETU32(mk);
    ka[1] = GETU32(mk + 4);
    ka[2] = GETU32(mk + 8);
    ka[3] = GETU32(mk + 12);

    kb[0] = GETU32(mk + 16);
    kb[1] = GETU32(mk + 20);
    kb[2] = GETU32(mk + 24);
    kb[3] = GETU32(mk + 28);

    *KL = ka[0];
    *KR = ka[1];
    t0 = ka[2];
    t1 = ka[3];

    for (i = 0; i < CAMELLIA_ROUNDS / 2 - 1; i++) {
        if (i % 2 == 0) {
            w0 = t0 ^ SIGMA[i];
            w1 = t1 ^ SIGMA[i + 1];
        } else {
            w0 = t0 ^ SIGMA[i + 1];
            w1 = t1 ^ SIGMA[i];
        }

        t0 = ((w0 >> 8) | (w1 << 24)) ^ (w1 >> 8) ^ (w1 << 16);
        t1 = ((w1 >> 8) | (w0 << 24)) ^ (w0 >> 8) ^ w0;
        *KL ^= t0;
        *KR ^= t1;
        t0 = *KL;
        t1 = *KR;
    }

    *KL ^= kb[0];
    *KR ^= kb[1];
    *KR ^= ka[0];
    *KL ^= ka[1];
    *KL ^= kb[2];
    *KR ^= kb[3];
}

void camellia_f2(WORD *xl, WORD *xr, const WORD *kl, const WORD *kr) {
    WORD x, t;
    x = *xr ^ kl[0];
    t = *xl ^ ((x >> 4) | (x << 28));
    *xl = *xl ^ (t ^ (x >> 1) ^ (x << 31));
    *xr = t;
    *xr = *xr ^ kr[0];
}

void camellia_encrypt(const BYTE *input, BYTE *output, const WORD *kl, const WORD *kr) {
    WORD xl, xr, t0, t1;
    int r;

    xl = GETU32(input);
    xr = GETU32(input + 4);

    for (r = 0; r < CAMELLIA_ROUNDS / 2; r++) {
        camellia_f2(&xl, &xr, kl + 2 * r, kr + 2 * r);
        camellia_f2(&xr, &xl, kl + 2 * r + 1, kr + 2 * r + 1);
    }

    t0 = xl;
    t1 = xr;
    xl = t1;
    xr = t0;

    xl = xl ^ kl[CAMELLIA_ROUNDS];
    xr = xr ^ kl[CAMELLIA_ROUNDS + 1];
    t0 = xl;
    t1 = xr;

    PUTU32(output, t0);
    PUTU32(output + 4, t1);
}

int main() {
    BYTE input[CAMELLIA_BLOCK_SIZE] = "Hello, Camellia!";
    BYTE output[CAMELLIA_BLOCK_SIZE] = {0};
    BYTE key[CAMELLIA_BLOCK_SIZE] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10};

    WORD kl[34], kr[34];
    camellia_subkey(kl, kr, key);

    camellia_encrypt(input, output, kl, kr);

    printf("Encrypted: ");
    for (int i = 0; i < CAMELLIA_BLOCK_SIZE; i++) {
        printf("%02x", output[i]);
    }
    printf("\n");

    return 0;
}
