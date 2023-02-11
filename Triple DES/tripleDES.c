#include <stdio.h>

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

unsigned long long initial_permutation(unsigned long long block) {
  unsigned long long permuted_block = 0;
  for (int i = 0; i < 64; i++) {
    int bit = (block >> (64 - IP[i])) & 1;
    permuted_block = (permuted_block << 1) | bit;
  }
  return permuted_block;
}

const int PC1[56] = {
  57, 49, 41, 33, 25, 17, 9,
  1, 58, 50, 42, 34, 26, 18,
  10, 2, 59, 51, 43, 35, 27,
  19, 11, 3, 60, 52, 44, 36,
  63, 55, 47, 39, 31, 23, 15,
  7, 62, 54, 46, 38, 30, 22,
  14, 6, 61, 53, 45, 37, 29,
  21, 13, 5, 28, 20, 12, 4
};

const int PC2[48] = {
  14, 17, 11, 24, 1, 5,
  3, 28, 15, 6, 21, 10,
  23, 19, 12, 4, 26, 8,
  16, 7, 27, 20, 13, 2,
  41, 52, 31, 37, 47, 55,
  30, 40, 51, 45, 33, 48,
  44, 49, 39, 56, 34, 53,
  46, 42, 50, 36, 29, 32
};

unsigned long long generate_key(unsigned long long key) {
  unsigned long long C = 0, D = 0;
  for (int i = 0; i < 56; i++) {
    int bit = (key >> (64 - PC1[i])) & 1;
    if (i < 28) {
      C = (C << 1) | bit;
    } else {
      D = (D << 1) | bit;
    }
  }

  unsigned long long generated_key = 0;
  for (int i = 0; i < 16; i++) {
    int shift = (i == 0 || i == 1) ? 1 : 2;
    C = ((C << shift) & 0xfffffff) | (C >> (28 - shift));
    D = ((D << shift) & 0xfffffff) | (D >> (28 - shift));

    unsigned long long CD = (C << 28) | D;
    for (int j = 0; j < 48; j++) {
      int bit = (CD >> (56 - PC2[j])) & 1;
      generated_key = (generated_key << 1) | bit;
    }
  }

  return generated_key;
}

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

unsigned long long xor_with_key(unsigned long long expanded_block, unsigned long long key) {
  return expanded_block ^ key;
}

unsigned long long encrypt_block(unsigned long long block, unsigned long long key) {
  block = permute(block, IP, 64);
  expanded_block = expand_block(block);
  block = xor_with_key(expanded_block, key);
 block = permute(block, FP, 64);
  return block;
}

unsigned long long decrypt_block(unsigned long long block, unsigned long long key) {
  block = permute(block, IP, 64);
  expanded_block = expand_block(block);
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







