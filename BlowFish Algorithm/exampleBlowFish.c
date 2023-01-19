#include <stdio.h>
#include <string.h>
#include "blowfish.h"

unsigned char key[] = "mykey";
unsigned char pt[] = "plaintext";
unsigned char ct[sizeof pt], temp[sizeof pt];

int main()
{
    BLOWFISH_CTX ctx;
    blowfish_init(&ctx, key, strlen(key));
    blowfish_encrypt(&ctx, pt, sizeof pt, ct);

    printf("Ciphertext: ");
    for (int i = 0; i < sizeof pt; i++)
        printf("%02x", ct[i]);
    printf("\n");

    blowfish_init(&ctx, key, strlen(key));
    blowfish_decrypt(&ctx, ct, sizeof ct, temp);

    printf("Plaintext: %s\n", temp);
    return 0;
}
