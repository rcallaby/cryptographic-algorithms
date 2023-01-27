#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define KEYLEN 512

typedef struct {
    long long n;
    long long e;
} PublicKey;

typedef struct {
    long long n;
    long long d;
} PrivateKey;

long long gcd(long long a, long long b) {
    return b ? gcd(b, a % b) : a;
}

long long mod_exp(long long base, long long exp, long long mod) {
    long long res = 1;
    while (exp > 0) {
        if (exp & 1) res = (res * base) % mod;
        base = (base * base) % mod;
        exp >>= 1;
    }
    return res;
}

void encrypt(const char *message, const PublicKey *key, char *ciphertext) {
    for (int i = 0; i < strlen(message); i++) {
        ciphertext[i] = (char)(mod_exp(message[i], key->e, key->n));
    }
}

void decrypt(const char *ciphertext, const PrivateKey *key, char *message) {
    for (int i = 0; i < strlen(ciphertext); i++) {
        message[i] = (char)(mod_exp(ciphertext[i], key->d, key->n));
    }
}

void generate_keypair(PublicKey *public_key, PrivateKey *private_key) {
    long long p, q, phi, e, d;

    // Generate two prime numbers, p and q
    p = rand() % (KEYLEN / 2 - 50) + 50;
    q = rand() % (KEYLEN / 2 - 50) + 50;
    while (gcd(p, q) != 1) {
        p = rand() % (KEYLEN / 2 - 50) + 50;
        q = rand() % (KEYLEN / 2 - 50) + 50;
    }

    // Compute n = pq
    public_key->n = p * q;
    private_key->n = public_key->n;

    // Compute phi(n) = phi(p)phi(q) = (p-1)(q-1)
    phi = (p - 1) * (q - 1);

    // Select a small odd integer e such that gcd(e, phi(n)) = 1
    e = 3;
    while (gcd(e, phi) != 1) {
        e += 2;
    }
    public_key->e = e;

    // Compute d such that de = 1 (mod phi(n))
    d = 2;
    while ((d * e) % phi != 1) {
        d++;
    }
    private_key->d = d;
}

int main() {
    char message[KEYLEN], ciphertext[KEYLEN], decrypted_message[KEYLEN];
    PublicKey public_key;
    PrivateKey private_key;

    printf("Enter message to encrypt: ");
    scanf("%[^\n]s", message);

    // Generate keypair
    generate_keypair(&public_key, &private_key);

    // Encrypt message
    encrypt(message, &public_key, ciphertext);

    // Decrypt message
    decrypt(ciphertext, &private_key, decrypted_message);

    printf("Original message: %s\n", message);
    printf("Encrypted message: %s\n", ciphertext);
    printf("Decrypted message: %s\n", decrypted_message);

    return 0;
}

