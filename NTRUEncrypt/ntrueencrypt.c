#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 2048
#define P 3
#define Q 677

void generatePrivateKey(int *f) {
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        f[i] = rand() % 3 - 1;
    }
}

void generatePublicKey(const int *f, int *h, int *g) {
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        h[i] = rand() % 3 - 1;
    }

    for (int i = 0; i < N; i++) {
        g[i] = ((Q / P) * f[i]) % Q;
        g[i] = (g[i] + h[i]) % Q;
    }
}

void encrypt(int message, const int *h, const int *g, int *c) {
    int r[N];
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        r[i] = rand() % 3 - 1;
    }

    int e[N];
    for (int i = 0; i < N; i++) {
        e[i] = ((Q / P) * r[i]) % Q;
    }

    for (int i = 0; i < N; i++) {
        c[i] = ((message * h[i]) + (r[i] * g[i]) + e[i]) % Q;
    }
}

int decrypt(const int *c, const int *f) {
    int m[N];
    for (int i = 0; i < N; i++) {
        m[i] = (c[i] * f[i]) % Q;
    }

    int sum = 0;
    for (int i = 0; i < N; i++) {
        sum += m[i];
    }

    int message = (sum + (Q / 2)) / Q;
    return message;
}

int main() {
    int privateKey[N];
    generatePrivateKey(privateKey);

    int publicKeyH[N];
    int publicKeyG[N];
    generatePublicKey(privateKey, publicKeyH, publicKeyG);

    int message = 7;
    int ciphertext[N];
    encrypt(message, publicKeyH, publicKeyG, ciphertext);

    int decryptedMessage = decrypt(ciphertext, privateKey);

    printf("Original Message: %d\n", message);
    printf("Decrypted Message: %d\n", decryptedMessage);

    return 0;
}
