#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define BLOCK_SIZE 16 /* Twofish uses 128-bit blocks */

// TODO: Needs some corrections

/* Key schedule */
uint32_t s[4][256];
uint32_t k[40];

/* Twofish round function */
uint32_t f(uint32_t x)
{
    return (s[0][x & 0xff] + s[1][(x >> 8) & 0xff]) ^ s[2][(x >> 16) & 0xff] + s[3][(x >> 24) & 0xff];
}

/* Twofish encryption */
void twofish_encrypt(uint8_t *ciphertext, uint8_t *plaintext, uint8_t *key)
{
    int i, j, rounds;
    uint32_t a, b, c, d;

    /* Key setup */
    rounds = 32; /* Twofish uses 32 rounds */
    for (i = 0; i < rounds; i++)
    {
        k[i] = (key[4 * i] << 24) | (key[4 * i + 1] << 16) | (key[4 * i + 2] << 8) | key[4 * i + 3];
    }

    /* Key schedule */
    for (i = 0; i < 256; i++)
    {
        s[0][i] = s[1][i] = s[2][i] = s[3][i] = 0;
    }
    for (i = 0; i < 256; i++)
    {
        s[0][i] = (uint32_t)pgm_read_dword(&mds[0][i]);
        s[1][i] = (uint32_t)pgm_read_dword(&mds[1][i]);
        s[2][i] = (uint32_t)pgm_read_dword(&mds[2][i]);
        s[3][i] = (uint32_t)pgm_read_dword(&mds[3][i]);
    }

    /* Encryption */
    a = (plaintext[0] << 24) | (plaintext[1] << 16) | (plaintext[2] << 8) | plaintext[3];
    b = (plaintext[4] << 24) | (plaintext[5] << 16) | (plaintext[6] << 8) | plaintext[7];
    c = (plaintext[8] << 24) | (plaintext[9] << 16) | (plaintext[10] << 8) | plaintext[11];
    d = (plaintext[12] << 24) | (plaintext[13] << 16) | (plaintext[14] << 8) | plaintext[15];

    for (i = 0; i < rounds; i += 2)
    {
        a = (a + f(b) ^ k[i]) & 0xffffffff;
        d = (d ^ a) & 0xffffffff;
        d = (d >> 1) | (d << 31);
        c = (c + d) & 0xffffffff;
        b = (b ^ c) & 0xffffffff;
        b = (b >> 1) | (b << 31);

        a = (a + f(b) ^ k[i + 1]) & 0xffffffff;
        d = (d ^ a) & 0xffffffff;
        d = (d >> 1) | (d << 31);
        c = (c + d) & 0xffffffff;
        b = (b ^ c) & 0xffffffff;
        b = (b >> 1) | (b << 31);
    }

    /* Output transformation */
    a = (a + k[rounds]) & 0xffffffff;
    d = (d ^ a) & 0xffffffff;
    d = (d >> 1) | (d << 31);
    c = (c + d) & 0xffffffff;
    b = (b ^ c) & 0xffffffff;
    b = (b >> 1) | (b << 31);

    ciphertext[0] = a >> 24;
    ciphertext[1] = a >> 16;
    ciphertext[2] = a >> 8;
    ciphertext[3] = a;
    ciphertext[4] = b >> 24;
    ciphertext[5] = b >> 16;
    ciphertext[6] = b >> 8;
    ciphertext[7] = b;
    ciphertext[8] = c >> 24;
    ciphertext[9] = c >> 16;
    ciphertext[10] = c >> 8;
    ciphertext[11] = c;
    ciphertext[12] = d >> 24;
    ciphertext[13] = d >> 16;
    ciphertext[14] = d >> 8;
    ciphertext[15] = d;
}
