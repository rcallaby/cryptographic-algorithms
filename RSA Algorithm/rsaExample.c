#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

int main()
{
    // Generate RSA key pair
    RSA *keypair = RSA_generate_key(2048, RSA_F4, NULL, NULL);

    // Extract public key from key pair
    RSA *public_key = RSAPublicKey_dup(keypair);

    // Extract private key from key pair
    RSA *private_key = RSAPrivateKey_dup(keypair);

    // Encrypt message
    char *message = "Hello, World!";
    unsigned char *encrypted_message = (unsigned char *)malloc(RSA_size(public_key));
    int encrypted_message_len = RSA_public_encrypt(strlen(message) + 1, (unsigned char *)message, encrypted_message, public_key, RSA_PKCS1_OAEP_PADDING);

    // Decrypt message
    unsigned char *decrypted_message = (unsigned char *)malloc(encrypted_message_len);
    RSA_private_decrypt(encrypted_message_len, encrypted_message, decrypted_message, private_key, RSA_PKCS1_OAEP_PADDING);

    printf("Original message: %s\n", message);
    printf("Encrypted message: %s\n", encrypted_message);
    printf("Decrypted message: %s\n", decrypted_message);

    // Clean up
    RSA_free(keypair);
    RSA_free(public_key);
    RSA_free(private_key);
    free(encrypted_message);
    free(decrypted_message);

    return 0;
}
