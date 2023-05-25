#include <iostream>
#include <cstdint>

// Twofish constants
const int BLOCK_SIZE = 16;
const int NUM_ROUNDS = 16;

// Twofish key schedule
struct TwofishKey {
    uint32_t subKeys[40];
};

// Forward S-Box lookup table
const uint8_t sBox[256] = {
    // S-box 0
    0xA9, 0x67, 0xB3, 0xE8, 0x04, 0xFD, 0xA3, 0x76,
    0x9A, 0x92, 0x80, 0x78, 0xE4, 0xDD, 0xD1, 0x38,
    // S-box 1
    0x0B, 0x0D, 0x0C, 0x0E, 0x0F, 0x08, 0x09, 0x0A,
    0x04, 0x05, 0x06, 0x07, 0x00, 0x01, 0x02, 0x03,
    // S-box 2
    0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38,
    0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
    // S-box 3
    0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87,
    0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB
};

// MDS matrix multiplication constants
const uint8_t mds[4][4] = {
    {0x01, 0xEF, 0x5B, 0x5B},
    {0x5B, 0xEF, 0xEF, 0x01},
    {0xEF, 0x5B, 0x01, 0xEF},
    {0xEF, 0x01, 0xEF, 0x5B}
};

// Helper function to convert a 4-byte array to a 32-bit integer
uint32_t bytesToUInt32(const uint8_t* bytes) {
    return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
}

// Helper function to convert a 32-bit integer to a 4-byte array
void uInt32ToBytes(uint32_t value, uint8_t* bytes) {
    bytes[0] = (value >> 24) & 0xFF;
    bytes[1] = (value >> 16) & 0xFF;
    bytes[2] = (value >> 8) & 0xFF;
    bytes[3] = value & 0xFF;
}

// Helper function for the g() function
uint32_t h(uint8_t* x, const TwofishKey& key, int keyIndex) {
    uint32_t result = 0;
    for (int i = 0; i < 4; i++) {
        result ^= key.subKeys[keyIndex + i] ^ x[i];
    }
    return key.subKeys[4 + keyIndex] ^ sBox[result & 0xFF] ^ (sBox[(result >> 8) & 0xFF] << 8) ^
        (sBox[(result >> 16) & 0xFF] << 16) ^ (sBox[(result >> 24) & 0xFF] << 24);
}

// Key schedule function
void keySchedule(uint8_t* key, TwofishKey& tfKey) {
    // Generate subkeys
    uint32_t* k = reinterpret_cast<uint32_t*>(key);
    for (int i = 0; i < 40; i += 2) {
        uint32_t a = h(reinterpret_cast<uint8_t*>(&i), tfKey, 0);
        uint32_t b = h(reinterpret_cast<uint8_t*>(&i), tfKey, 1);
        b = b << 8 | b >> 24;
        tfKey.subKeys[i] = a + b;
        tfKey.subKeys[i + 1] = (a << 9 | a >> 23) + b;
    }

    // Whitening keys
    for (int i = 0; i < 4; i++) {
        tfKey.subKeys[i] ^= k[i];
    }
}

// Helper function to multiply a matrix by a vector
void matrixMultiply(const uint8_t matrix[4][4], const uint8_t* vector, uint8_t* result) {
    for (int i = 0; i < 4; i++) {
        result[i] = 0;
        for (int j = 0; j < 4; j++) {
            result[i] ^= matrix[i][j] * vector[j];
        }
    }
}

// Encryption function
void encryptBlock(const uint8_t* plaintext, const TwofishKey& tfKey, uint8_t* ciphertext) {
    // Split the plaintext into four 32-bit words
    uint32_t x[4];
    for (int i = 0; i < 4; i++) {
        x[i] = bytesToUInt32(&plaintext[i * 4]);
    }

    // Whitening
    x[0] ^= tfKey.subKeys[0];
    x[1] ^= tfKey.subKeys[1];
    x[2] ^= tfKey.subKeys[2];
    x[3] ^= tfKey.subKeys[3];

    // Iterate through the rounds
    for (int round = 0; round < NUM_ROUNDS; round += 2) {
        // F-function
        uint32_t t0 = h(reinterpret_cast<uint8_t*>(&x[0]), tfKey, 4 * round);
        uint32_t t1 = h(reinterpret_cast<uint8_t*>(&x[1]), tfKey, 4 * round + 2);

        // XOR and swap
        x[2] ^= t0 + t1;
        x[2] = (x[2] >> 1) | (x[2] << 31);
        x[3] = (x[3] << 1) | (x[3] >> 31);
        x[3] ^= t0 + 2 * t1;

        // F-function
        t0 = h(reinterpret_cast<uint8_t*>(&x[2]), tfKey, 4 * round + 1);
        t1 = h(reinterpret_cast<uint8_t*>(&x[3]), tfKey, 4 * round + 3);

        // XOR and swap
        x[0] ^= t0 + t1;
        x[0] = (x[0] >> 1) | (x[0] << 31);
        x[1] = (x[1] << 1) | (x[1] >> 31);
        x[1] ^= t0 + 2 * t1;
    }

    // Final whitening
    x[2] ^= tfKey.subKeys[4 * NUM_ROUNDS];
    x[3] ^= tfKey.subKeys[4 * NUM_ROUNDS + 1];
    x[0] ^= tfKey.subKeys[4 * NUM_ROUNDS + 2];
    x[1] ^= tfKey.subKeys[4 * NUM_ROUNDS + 3];

    // Combine the four 32-bit words into the ciphertext
    for (int i = 0; i < 4; i++) {
        uInt32ToBytes(x[i], &ciphertext[i * 4]);
    }
}

// Decryption function
void decryptBlock(const uint8_t* ciphertext, const TwofishKey& tfKey, uint8_t* plaintext) {
    // Split the ciphertext into four 32-bit words
    uint32_t x[4];
    for (int i = 0; i < 4; i++) {
        x[i] = bytesToUInt32(&ciphertext[i * 4]);
    }

    // Initial whitening
    x[2] ^= tfKey.subKeys[4 * NUM_ROUNDS];
    x[3] ^= tfKey.subKeys[4 * NUM_ROUNDS + 1];
    x[0] ^= tfKey.subKeys[4 * NUM_ROUNDS + 2];
    x[1] ^= tfKey.subKeys[4 * NUM_ROUNDS + 3];

    // Iterate through the rounds in reverse
    for (int round = NUM_ROUNDS - 2; round >= 0; round -= 2) {
        // F-function
        uint32_t t0 = h(reinterpret_cast<uint8_t*>(&x[0]), tfKey, 4 * round);
        uint32_t t1 = h(reinterpret_cast<uint8_t*>(&x[1]), tfKey, 4 * round + 2);

        // XOR and swap
        x[1] ^= t0 + t1;
        x[1] = (x[1] >> 1) | (x[1] << 31);
        x[0] = (x[0] << 1) | (x[0] >> 31);
        x[0] ^= t0 + 2 * t1;

        // F-function
        t0 = h(reinterpret_cast<uint8_t*>(&x[2]), tfKey, 4 * round + 1);
        t1 = h(reinterpret_cast<uint8_t*>(&x[3]), tfKey, 4 * round + 3);

        // XOR and swap
        x[3] ^= t0 + t1;
        x[3] = (x[3] >> 1) | (x[3] << 31);
        x[2] = (x[2] << 1) | (x[2] >> 31);
        x[2] ^= t0 + 2 * t1;
    }

    // Final whitening
    x[0] ^= tfKey.subKeys[0];
    x[1] ^= tfKey.subKeys[1];
    x[2] ^= tfKey.subKeys[2];
    x[3] ^= tfKey.subKeys[3];

    // Combine the four 32-bit words into the plaintext
    for (int i = 0; i < 4; i++) {
        uInt32ToBytes(x[i], &plaintext[i * 4]);
    }
}

int main() {
    // Example usage
    TwofishKey tfKey;
    uint8_t key[32] = {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
        0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10,
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
    };

    keySchedule(key, tfKey);

    uint8_t plaintext[BLOCK_SIZE] = {
        0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0,
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF
    };

    uint8_t ciphertext[BLOCK_SIZE];

    encryptBlock(plaintext, tfKey, ciphertext);

    std::cout << "Ciphertext: ";
    for (int i = 0; i < BLOCK_SIZE; i++) {
        std::cout << std::hex << (int)ciphertext[i] << " ";
    }
    std::cout << std::endl;

    uint8_t decryptedText[BLOCK_SIZE];

    decryptBlock(ciphertext, tfKey, decryptedText);

    std::cout << "Decrypted Text: ";
    for (int i = 0; i < BLOCK_SIZE; i++) {
        std::cout << std::hex << (int)decryptedText[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
