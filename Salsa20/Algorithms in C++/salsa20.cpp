#include <iostream>
#include <array>

constexpr uint32_t ROUNDS = 20;

void salsa20_core(std::array<uint32_t, 16>& state) {
    for (uint32_t i = 0; i < ROUNDS; i += 2) {
        state[4] ^= ((state[0] + state[12]) << 7) | ((state[0] + state[12]) >> 25);
        state[8] ^= ((state[4] + state[0]) << 9) | ((state[4] + state[0]) >> 23);
        state[12] ^= ((state[8] + state[4]) << 13) | ((state[8] + state[4]) >> 19);
        state[0] ^= ((state[12] + state[8]) << 18) | ((state[12] + state[8]) >> 14);

        state[9] ^= ((state[5] + state[1]) << 7) | ((state[5] + state[1]) >> 25);
        state[13] ^= ((state[9] + state[5]) << 9) | ((state[9] + state[5]) >> 23);
        state[1] ^= ((state[13] + state[9]) << 13) | ((state[13] + state[9]) >> 19);
        state[5] ^= ((state[1] + state[13]) << 18) | ((state[1] + state[13]) >> 14);

        state[14] ^= ((state[10] + state[6]) << 7) | ((state[10] + state[6]) >> 25);
        state[2] ^= ((state[14] + state[10]) << 9) | ((state[14] + state[10]) >> 23);
        state[6] ^= ((state[2] + state[14]) << 13) | ((state[2] + state[14]) >> 19);
        state[10] ^= ((state[6] + state[2]) << 18) | ((state[6] + state[2]) >> 14);

        state[3] ^= ((state[15] + state[11]) << 7) | ((state[15] + state[11]) >> 25);
        state[7] ^= ((state[3] + state[15]) << 9) | ((state[3] + state[15]) >> 23);
        state[11] ^= ((state[7] + state[3]) << 13) | ((state[7] + state[3]) >> 19);
        state[15] ^= ((state[11] + state[7]) << 18) | ((state[11] + state[7]) >> 14);

        state[1] ^= ((state[0] + state[3]) << 7) | ((state[0] + state[3]) >> 25);
        state[2] ^= ((state[1] + state[0]) << 9) | ((state[1] + state[0]) >> 23);
        state[3] ^= ((state[2] + state[1]) << 13) | ((state[2] + state[1]) >> 19);
        state[0] ^= ((state[3] + state[2]) << 18) | ((state[3] + state[2]) >> 14);

        state[6] ^= ((state[5] + state[4]) << 7) | ((state[5] + state[4]) >> 25);
        state[7] ^= ((state[6] + state[5]) << 9) | ((state[6] + state[5]) >> 23);
        state[4] ^= ((state[7] + state[6]) << 13) | ((state[7] + state[6]) >> 19);
        state[5] ^= ((state[4] + state[7]) << 18) | ((state[4] + state[7]) >> 14);

        state[11] ^= ((state[10] + state[9]) << 7) | ((state[10] + state[9]) >> 25);
        state[8] ^= ((state[11] + state[10]) << 9) | ((state[11] + state[10]) >> 23);
        state[9] ^= ((state[8] + state[11]) << 13) | ((state[8] + state[11]) >> 19);
        state[10] ^= ((state[9] + state[8]) << 18) | ((state[9] + state[8]) >> 14);

        state[12] ^= ((state[15] + state[14]) << 7) | ((state[15] + state[14]) >> 25);
        state[13] ^= ((state[12] + state[15]) << 9) | ((state[12] + state[15]) >> 23);
        state[14] ^= ((state[13] + state[12]) << 13) | ((state[13] + state[12]) >> 19);
        state[15] ^= ((state[14] + state[13]) << 18) | ((state[14] + state[13]) >> 14);
    }
}

void salsa20_encrypt(const std::uint8_t* input, std::uint8_t* output, const std::array<uint32_t, 8>& key, const std::array<uint32_t, 2>& nonce) {
    std::array<uint32_t, 16> state = {
        0x61707865, key[0], key[1], key[2], key[3], 0x3320646e, nonce[0], nonce[1],
        0, 0, 0x79622d32, 0x6b206574, 0x6574616e, 0x79742d32, 0x7265746e, 0x64203333
    };

    std::array<uint32_t, 16> block;
    std::size_t i = 0;

    while (input[i]) {
        for (std::size_t j = 0; j < 16; j++) {
            block[j] = ((uint32_t)input[4 * j]) |
                       ((uint32_t)input[4 * j + 1] << 8) |
                       ((uint32_t)input[4 * j + 2] << 16) |
                       ((uint32_t)input[4 * j + 3] << 24);
        }

        salsa20_core(state);

        for (std::size_t j = 0; j < 16; j++) {
            block[j] += state[j];
        }

        for (std::size_t j = 0; j < 16
    }
}

        for (std::size_t j = 0; j < 16; j++) {
            output[4 * j] = block[j] & 0xFF;
            output[4 * j + 1] = (block[j] >> 8) & 0xFF;
            output[4 * j + 2] = (block[j] >> 16) & 0xFF;
            output[4 * j + 3] = (block[j] >> 24) & 0xFF;
        }

        input += 64;
        output += 64;
    }
}

int main() {
    std::uint8_t input[256] = "Hello, Salsa20!";
    std::uint8_t output[256] = {0};
    std::array<uint32_t, 8> key = {0x01020304, 0x05060708, 0x090a0b0c, 0x0d0e0f10, 0x11121314, 0x15161718, 0x191a1b1c, 0x1d1e1f20};
    std::array<uint32_t, 2> nonce = {0x00000000, 0x00000001};

    salsa20_encrypt(input, output, key, nonce);

    std::cout << "Encrypted: ";
    for (char c : output) {
        std::cout << c;
    }
    std::cout << std::endl;

    return 0;
}
