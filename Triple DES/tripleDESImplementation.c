#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/des.h>

int main(int argc, char *argv[])
{
    // Check for correct usage
    if (argc != 4) {
        printf("Usage: %s plaintext key1 key2 key3\n", argv[0]);
        return 1;
    }

    // Get the plaintext and keys from the command line
    unsigned char *plaintext = (unsigned char *)argv[1];
    unsigned char *key1 = (unsigned char *)argv[2];
    unsigned char *key2 = (unsigned char *)argv[3];
    unsigned char *key3 = (unsigned char *)argv[4];
    int plaintext_len = strlen(plaintext);

    // Create the ciphertext buffer
    unsigned char ciphertext[plaintext_len];

    // Set up the encryption and decryption keys
    DES_cblock key1_block, key2_block, key3_block;
    memcpy(key1_block, key1, 8);
    memcpy(key2_block, key2, 8);
    memcpy(key3_block, key3, 8);
    DES_key_schedule schedule1, schedule2, schedule3;
    DES_set_key_unchecked(&key1_block, &schedule1);
    DES_set_key_unchecked(&key2_block, &schedule2);
    DES_set_key_unchecked(&key3_block, &schedule3);

    // Encrypt the plaintext
    DES_ede3_cbc_encrypt(plaintext, ciphertext, plaintext_len, &schedule1, &schedule2, &schedule3, &key1_block, DES_ENCRYPT);

    // Print the ciphertext
    printf("Ciphertext: ");
    for (int i = 0; i < plaintext_len; i++) {
        printf("%02x", ciphertext[i]);
    }
    printf("\n");

    return 0;
}
