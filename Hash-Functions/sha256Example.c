#include <openssl/sha.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    unsigned char data[] = "Hello, World!";
    unsigned char hash[SHA256_DIGEST_LENGTH];

    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, data, strlen(data));
    SHA256_Final(hash, &ctx);

    int i;
    for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
        printf("%02x", hash[i]);
    printf("\n");

    return 0;
}
