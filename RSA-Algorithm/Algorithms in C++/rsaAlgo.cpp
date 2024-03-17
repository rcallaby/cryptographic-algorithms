#include <iostream>
#include <string>
#include <random>

constexpr int KEYLEN = 512;

struct PublicKey {
    long long n;
    long long e;
};

struct PrivateKey {
    long long n;
    long long d;
};

long long gcd(long long a, long long b) {
    while (b != 0) {
        long long temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

long long mod_exp(long long base, long long exp, long long mod) {
    long long res = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1)
            res = (res * base) % mod;
        base = (base * base) % mod;
        exp >>= 1;
    }
    return res;
}

void encrypt(const std::string& message, const PublicKey& key, std::string& ciphertext) {
    ciphertext.resize(message.size());
    for (size_t i = 0; i < message.size(); ++i)
        ciphertext[i] = static_cast<char>(mod_exp(message[i], key.e, key.n));
}

void decrypt(const std::string& ciphertext, const PrivateKey& key, std::string& message) {
    message.resize(ciphertext.size());
    for (size_t i = 0; i < ciphertext.size(); ++i)
        message[i] = static_cast<char>(mod_exp(ciphertext[i], key.d, key.n));
}

void generate_keypair(PublicKey& public_key, PrivateKey& private_key) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<long long> dis(50, KEYLEN / 2 - 50);

    long long p, q, phi, e, d;

    // Generate two prime numbers, p and q
    do {
        p = dis(gen);
        q = dis(gen);
    } while (gcd(p, q) != 1);

    // Compute n = pq
    public_key.n = p * q;
    private_key.n = public_key.n;

    // Compute phi(n) = phi(p)phi(q) = (p-1)(q-1)
    phi = (p - 1) * (q - 1);

    // Select a small odd integer e such that gcd(e, phi(n)) = 1
    e = 3;
    while (gcd(e, phi) != 1)
        e += 2;
    public_key.e = e;

    // Compute d such that de = 1 (mod phi(n))
    d = 2;
    while ((d * e) % phi != 1)
        ++d;
    private_key.d = d;
}

int main() {
    std::string message, ciphertext, decrypted_message;
    PublicKey public_key;
    PrivateKey private_key;

    std::cout << "Enter message to encrypt: ";
    std::getline(std::cin, message);

    // Generate keypair
    generate_keypair(public_key, private_key);

    // Encrypt message
    encrypt(message, public_key, ciphertext);

    // Decrypt message
    decrypt(ciphertext, private_key, decrypted_message);

    std::cout << "Original message: " << message << std::endl;
    std::cout << "Encrypted message: " << ciphertext << std::endl;
    std::cout << "Decrypted message: " << decrypted_message << std::endl;

    return 0;
}

