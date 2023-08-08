#include <stdint.h>

#define N 16

typedef struct {
    uint32_t p[N + 2];
    uint32_t s[4][256];
} BLOWFISH_CTX;

void blowfish_init(BLOWFISH_CTX *ctx, unsigned char *key, int keylen);
void blowfish_encrypt(BLOWFISH_CTX *ctx, unsigned char *pt, int ptlen, unsigned char *ct);
void blowfish_decrypt(BLOWFISH_CTX *ctx, unsigned char *ct, int ctlen, unsigned char *pt);

uint32_t F(BLOWFISH_CTX *ctx, uint32_t x);
void blowfish_init(BLOWFISH_CTX *ctx, unsigned char *key, int keylen)
{
    int i, j, k;
    uint32_t data, datal, datar;

    /* Initialize the first N + 2 elements of the p array with the hex digits of pi */
    static const uint32_t init_p[N + 2] = {
        0x243f6a88, 0x85a308d3, 0x13198a2e, 0x03707344,
        0xa4093822, 0x299f31d0, 0x082efa98, 0xec4e6c89,
        0x452821e6, 0x38d01377, 0xbe5466cf, 0x34e90c6c,
        0xc0ac29b7, 0xc97c50dd, 0x3f84d5b5, 0xb5470917,
        0x9216d5d9, 0x8979fb1b
    };

    /* Initialize the first 256 elements of the s arrays with the identity permutation */
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 256; j++) {
            ctx->s[i][j] = j;
        }
    }

    /* Copy the key over the p array */
    for (i = 0, j = 0; i < N + 2; i++) {
        data = 0x00000000;
        for (k = 0; k < 4; k++) {
            data = (data << 8) | key[j++];
            if (j >= keylen) {
                j = 0;
            }
        }
        ctx->p[i] = init_p[i] ^ data;
    }

    /* Perform the initial permutation of the p array */
    datal = 0x00000000;
    datar = 0x00000000;
    for (i = 0; i < N + 2; i += 2) {
        datal ^= F(ctx, datar);
        datar ^= F(ctx, datal);
    }

    /* Perform the initial permutation of the s arrays */
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 256; j += 2) {
            datal ^= F(ctx, datar);
            datar ^= F(ctx, datal);
            ctx->s[i][j] = datal;
            ctx->s[i][j + 1] = datar;
        }
    }
}

void blowfish_encrypt(BLOWFISH_CTX *ctx, unsigned char *pt, int ptlen, unsigned char *ct)
{
    int i;
    uint32_t datal, datar;

    for (i = 0; i < ptlen; i += 8) {
        datal = ((uint32_t)pt[i] << 24)
            | ((uint32_t)pt[i + 1] << 16)
            | ((uint32_t)pt[i + 2] << 8)
            | ((uint32_t)pt[i + 3]);
        datar = ((uint32_t)pt[i + 4] << 24)
            | ((uint32_t)pt[i + 5] << 16)
            | ((uint32_t)pt[i + 6] << 8)
            | ((uint32_t)pt[i + 7]);

        datal ^= ctx->p[0];
        datar ^= F(ctx, datal) ^ ctx->p[1];
        datal ^= F(ctx, datar) ^ ctx->p[2];
        datar ^= F(ctx, datal) ^ ctx->p[3];
        datal ^= F(ctx, datar) ^ ctx->p[4];
        datar ^= F(ctx, datal) ^ ctx->p[5];
        datal ^= F(ctx, datar) ^ ctx->p[6];
        datar ^= F(ctx, datal) ^ ctx->p[7];
        datal ^= F(ctx, datar) ^ ctx->p[8];
        datar ^= F(ctx, datal) ^ ctx->p[9];
        datal ^= F(ctx, datar) ^ ctx->p[10];
        datar ^= F(ctx, datal) ^ ctx->p[11];
        datal ^= F(ctx, datar) ^ ctx->p[12];
        datar ^= F(ctx, datal) ^ ctx->p[13];
        datal ^= F(ctx, datar) ^ ctx->p[14];
        datar ^= F(ctx, datal) ^ ctx->p[15];
        datal ^= F(ctx, datar) ^ ctx->p[16];

        ct[i] = datal >> 24;
        ct[i + 1] = datal >> 16;
        ct[i + 2] = datal >> 8;
        ct[i + 3] = datal;
        ct[i + 4] = datar >> 24;
        ct[i + 5] = datar >> 16;
        ct[i + 6] = datar >> 8;
        ct[i + 7] = datar;
    }
}

void blowfish_decrypt(BLOWFISH_CTX *ctx, unsigned char *ct, int ctlen, unsigned char *pt)
{
    int i;
    uint32_t datal, datar;

    for (i = 0; i < ctlen; i += 8) {
        datal = ((uint32_t)ct[i] << 24)
            | ((uint32_t)ct[i + 1] << 16)
            | ((uint32_t)ct[i + 2] << 8)
            | ((uint32_t)ct[i + 3]);
        datar = ((uint32_t)ct[i + 4] << 24)
            | ((uint32_t)ct[i + 5] << 16)
            | ((uint32_t)ct[i + 6] << 8)
            | ((uint32_t)ct[i + 7]);

        datal ^= ctx->p[17];
        datar ^= F(ctx, datal) ^ ctx->p[16];
        datal ^= F(ctx, datar) ^ ctx->p[15];
        datar ^= F(ctx, datal) ^ ctx->p[14];
        datal ^= F(ctx, datar) ^ ctx->p[13];
        datar ^= F(ctx, datal) ^ ctx->p[12];
        datal ^= F(ctx, datar) ^ ctx->p[11];
        datar ^= F(ctx, datal) ^ ctx->p[10];
        datal ^= F(ctx, datar) ^ ctx->p[9];
        datar ^= F(ctx, datal) ^ ctx->p[8];
        datal ^= F(ctx, datar) ^ ctx->p[7];
        datar ^= F(ctx, datal) ^ ctx->p[6];
        datal ^= F(ctx, datar) ^ ctx->p[5];
        datar ^= F(ctx, datal) ^ ctx->p[4];
        datal ^= F(ctx, datar) ^ ctx->p[3];
        datar ^= F(ctx, datal) ^ ctx->p[2];
        datal ^= F(ctx, datar) ^ ctx->p[1];

        pt[i] = datal >> 24;
        pt[i + 1] = datal >> 16;
        pt[i + 2] = datal >> 8;
        pt[i + 3] = datal;
        pt[i + 4] = datar >> 24;
        pt[i + 5] = datar >> 16;
        pt[i + 6] = datar >> 8;
        pt[i + 7] = datar;
    }
}

uint32_t F(BLOWFISH_CTX *ctx, uint32_t x)
{
    uint32_t a, b, c, d;

    a = x >> 24;
    b = (x >> 16) & 0xff;
    c = (x >> 8) & 0xff;
    d = x & 0xff;

    return ((ctx->s[0][a] + ctx->s[1][b])^ ctx->s[2][c]) + ctx->s[3][d];
}