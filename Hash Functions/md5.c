#include <stdio.h>
#include <string.h>
#include <math.h>

#define ROTLEFT(a, b) ((a &#8203;`oaicite:{"index":0,"invalid_reason":"Malformed citation << b) | (a >> (32-b)))\n#define ROTRIGHT(a, b) ((a >>"}`&#8203; b) | (a << (32-b)))

#define F(x, y, z) ((x & y) | (~x & z))
#define G(x, y, z) ((x & z) | (y & ~z))
#define H(x, y, z) (x ^ y ^ z)
#define I(x, y, z) (y ^ (x | ~z))

#define FF(a, b, c, d, x, s, ac) { \
    a = a + F(b, c, d) + x + ac; \
    a = ROTLEFT(a, s); \
    a = a + b; \
}
#define GG(a, b, c, d, x, s, ac) { \
    a = a + G(b, c, d) + x + ac; \
    a = ROTLEFT(a, s); \
    a = a + b; \
}
#define HH(a, b, c, d, x, s, ac) { \
    a = a + H(b, c, d) + x + ac; \
    a = ROTLEFT(a, s); \
    a = a + b; \
}
#define II(a, b, c, d, x, s, ac) { \
    a = a + I(b, c, d) + x + ac; \
    a = ROTLEFT(a, s); \
    a = a + b; \
}

unsigned char *md5(unsigned char *initial_msg, size_t initial_len) {

    static unsigned char digest[16];

    unsigned int h0, h1, h2, h3;

    unsigned char *msg = NULL;

    size_t new_len, offset;
    unsigned int w[16];
    unsigned int a, b, c, d, i, f, g, temp;

    h0 = 0x67452301;
    h1 = 0xEFCDAB89;
    h2 = 0x98BADCFE;
    h3 = 0x10325476;

    new_len = initial_len + 1;
    offset = initial_len % 64;
    if (offset < 56) {
        new_len = initial_len - offset + 64;
    } else {
        new_len = initial_len + 64 - offset + 64;
    }

    msg = (unsigned char*)calloc(new_len + 8, 1);
    memcpy(msg, initial_msg, initial_len);
    msg[initial_len] = 0x80;

    for (i = 0; i < 8; i++) {
        msg[new_len + i] = (unsigned char)((initial_len * 8) >> (i * 8));
    }

     for (i = 0; i < new_len; i += 64) {
        a = h0;
        b = h1;
        c = h2;
        d = h3;

        for (g = 0; g < 16; g++) {
            w[g] = ((unsigned int) msg[i + g * 4 + 0]) << 24 |
                   ((unsigned int) msg[i + g * 4 + 1]) << 16 |
                   ((unsigned int) msg[i + g * 4 + 2]) << 8 |
                   ((unsigned int) msg[i + g * 4 + 3]);
        }

        for (g = 0; g < 64; g++) {
            if (g < 16) {
                f = F(b, c, d);
                i = g;
            } else if (g < 32) {
                f = G(b, c, d);
                i = (5 * g + 1) % 16;
            } else if (g < 48) {
                f = H(b, c, d);
                i = (3 * g + 5) % 16;
            } else {
                f = I(b, c, d);
                i = (7 * g) % 16;
            }

            temp = d;
            d = c;
            c = b;
            b = b + ROTLEFT((a + f + 0x100000000 * abs(sin(g + 1)) + w[i]), (int)(fabs(sin(g + 1))));
            a = temp;
        }

        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
    }

    for (i = 0; i < 4; i++) {
        digest[i] = (unsigned char)(h0 >> (i * 8));
        digest[i + 4] = (unsigned char)(h1 >> (i * 8));
        digest[i + 8] = (unsigned char)(h2 >> (i * 8));
        digest[i + 12] = (unsigned char)(h3 >> (i * 8));
    }

    free(msg);
    return digest;
}

int main(int argc, char **argv) {
    unsigned char *digest;
    char str[100];
    scanf("%s", str);
    digest = md5(str, strlen(str));
    for (int i = 0; i < 16; i++) {
        printf("%02x", digest[i]);
    }
    printf("\n");
    return 0;
}

