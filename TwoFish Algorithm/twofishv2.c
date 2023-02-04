#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define BLOCK_SIZE 16
#define ROUNDS 32
#define KEY_SIZE 32

uint32_t h(uint32_t x, uint8_t key[KEY_SIZE])
{
    uint32_t a = (x >> 24) & 0xff, b = (x >> 16) & 0xff, c = (x >> 8) & 0xff, d = x & 0xff;
    uint32_t f, g;

    f = ((key[3 * a] + key[3 * a + 1]) ^ key[3 * b]) + key[3 * c];
    g = ((key[3 * d] + key[3 * d + 1]) ^ key[3 * c]) + key[3 * a];
    return (f << 24) + (f << 8 & 0xff0000) + (g & 0xff00) + (g >> 8 & 0xff);
}

void encrypt(uint8_t plaintext[BLOCK_SIZE], uint8_t key[KEY_SIZE], uint32_t ciphertext[BLOCK_SIZE / 4])
{
    uint32_t x[4], k[ROUNDS + 4];
    int i, j;

    for (i = 0; i < 4; i++)
    {
        x[i] = (plaintext[4 * i] << 24) + (plaintext[4 * i + 1] << 16) + (plaintext[4 * i + 2] << 8) + plaintext[4 * i + 3];
    }

    for (i = 0; i < ROUNDS + 4; i++)
    {
        k[i] = h(x[i & 3] + i, key);
    }

    for (i = 0; i < ROUNDS; i += 2)
    {
        x[2] = x[2] ^ (k[i] + (((x[0] + x[1]) ^ k[i + 1]) >> 1));
        x[3] = x[3] ^ (k[i + 1] + (((x[2] + x[0]) ^ k[i]) >> 1));
        x[1] = x[1] ^ (k[i] + (((x[3] + x[2]) ^ k[i + 1]) >> 1));
        x[0] = x[0] ^ (k[i + 1] + (((x[1] + x[3]) ^ k[i]) >> 1));
    }

    for (i = 0; i < 4; i++)
    {
        ciphertext[i] = x[i] ^ k[ROUNDS + i];
    }
}

void decrypt(uint32_t ciphertext[BLOCK_SIZE / 4], uint8_t key[KEY_SIZE], uint8_t plaintext[BLOCK_SIZE])
{
    uint32_t x[4], k[ROUNDS + 4];
    int i, j;

    for (i = 0; i < 4; i++)
    {
        x[i] = ciphertext
                   [i] ^
               k[ROUNDS + i];
    }
    for (i = ROUNDS - 1; i >= 0; i -= 2)
    {
        x[0] = x[0] ^ (k[i + 1] + (((x[1] + x[3]) ^ k[i]) >> 1));
        x[1] = x[1] ^ (k[i] + (((x[3] + x[2]) ^ k[i + 1]) >> 1));
        x[3] = x[3] ^ (k[i + 1] + (((x[2] + x[0]) ^ k[i]) >> 1));
        x[2] = x[2] ^ (k[i] + (((x[0] + x[1]) ^ k[i + 1]) >> 1));
    }

    for (i = 0; i < 4; i++)
    {
        plaintext[4 * i] = x[i] >> 24 & 0xff;
        plaintext[4 * i + 1] = x[i] >> 16 & 0xff;
        plaintext[4 * i + 2] = x[i] >> 8 & 0xff;
        plaintext[4 * i + 3] = x[i] & 0xff;
    }
}

int main()
{
    uint8_t key[KEY_SIZE] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
                             0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
                             0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
                             0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
    uint8_t plaintext[BLOCK_SIZE] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                                     0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

    uint32_t ciphertext[BLOCK_SIZE / 4];
    uint8_t decrypted[BLOCK_SIZE];

    encrypt(plaintext, key, ciphertext);
    decrypt(ciphertext, key, decrypted);

    printf("Plaintext:  ");
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        printf("%02x ", plaintext[i]);
    }
    printf("\n");

    printf("Ciphertext: ");
    for (int i = 0; i < BLOCK_SIZE / 4; i++)
    {
        printf("%08x ", ciphertext[i]);
    }
    printf("\n");

    printf("Decrypted:  ");
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        printf("%02x ", decrypted[i]);
    }
    printf("\n");

    return 0;
}