#include <stdio.h>
#include <stdint.h>

//TODO: This needs some fixing, just some minor errors. Will track down and eliminate later.

const int IP[64] = {
  58, 50, 42, 34, 26, 18, 10, 2,
  60, 52, 44, 36, 28, 20, 12, 4,
  62, 54, 46, 38, 30, 22, 14, 6,
  64, 56, 48, 40, 32, 24, 16, 8,
  57, 49, 41, 33, 25, 17, 9, 1,
  59, 51, 43, 35, 27, 19, 11, 3,
  61, 53, 45, 37, 29, 21, 13, 5,
  63, 55, 47, 39, 31, 23, 15, 7
};

const int FP[64] = {
  40, 8, 48, 16, 56, 24, 64, 32,
  39, 7, 47, 15, 55, 23, 63, 31,
  38, 6, 46, 14, 54, 22, 62, 30,
  37, 5, 45, 13, 53, 21, 61, 29,
  36, 4, 44, 12, 52, 20, 60, 28,
  35, 3, 43, 11, 51, 19, 59, 27,
  34, 2, 42, 10, 50, 18, 58, 26,
  33, 1, 41, 9, 49, 17, 57, 25
};

const int E[48] = {
  32, 1, 2, 3, 4, 5,
  4, 5, 6, 7, 8, 9,
  8, 9, 10, 11, 12, 13,
  12, 13, 14, 15, 16, 17,
  16, 17, 18, 19, 20, 21,
  20, 21, 22, 23, 24, 25,
  24, 25, 26, 27, 28, 29,
  28, 29, 30, 31, 32, 1
};

unsigned long long permute(unsigned long long block, const int *table, int table_size) {
  unsigned long long permuted_block = 0;
  for (int i = 0; i < table_size; i++) {
    int bit = (block >> (64 - table[i])) & 1;
    permuted_block = (permuted_block << 1) | bit;
  }
  return permuted_block;
}

unsigned long long expand_block(unsigned long long block) {
  unsigned long long expanded_block = 0;
  for (int i = 0; i < 48; i++) {
    int bit = (block >> (32 - E[i])) & 1;
    expanded_block = (expanded_block << 1) | bit;
  }
  return expanded_block;
}

unsigned long long xor_with_key(unsigned long long expanded_block, unsigned long long key) {
  return expanded_block ^ key;
}

unsigned long long encrypt_block(unsigned long long block, unsigned long long key) {
  block = permute(block, IP, 64);
  unsigned long long expanded_block = expanded_block(block);
  block = xor_with_key(expanded_block, key);
  block = permute(block, FP, 64);
  return block;
}

unsigned long long decrypt_block(unsigned long long block, unsigned long long key) {
  block = permute(block, IP, 64);
  unsigned long long expanded_block = expanded_block(block);
  block = xor_with_key(expanded_block, key);
  block = permute(block, FP, 64);
  return block;
}

int main() {
  unsigned long long plaintext = 0x0123456789ABCDEF;
  unsigned long long key = 0x133457799BBCDFF1;
  unsigned long long ciphertext = encrypt_block(plaintext, key);
  printf("Ciphertext: %llx\n", ciphertext);
  unsigned long long decrypted = decrypt_block(ciphertext, key);
  printf("Decrypted: %llx\n", decrypted);
  return 0;
}
