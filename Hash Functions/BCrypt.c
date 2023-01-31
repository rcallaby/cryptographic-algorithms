#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// The number of rounds to use in the key expansion process
#define ROUNDS 16

// The size of the state array
#define STATE_SIZE 256

// The salt for the password hash
const unsigned char salt[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

// The Blowfish block function
void block_function(unsigned char *state, unsigned char *key) {
  unsigned int i, j, x, y, p1, p2;
  
  for (i = 0; i < ROUNDS; i++) {
    x = (state[0] << 24) | (state[1] << 16) | (state[2] << 8) | state[3];
    y = (state[4] << 24) | (state[5] << 16) | (state[6] << 8) | state[7];
    
    p1 = key[(i * 2) % ROUNDS];
    p2 = key[(i * 2 + 1) % ROUNDS];
    
    x = (x ^ (p1 << 24)) | ((x ^ (p1 << 16)) & 0x00FF0000) | 
        ((x ^ (p1 << 8)) & 0x0000FF00) | (x ^ p1);
    y = (y ^ (p2 << 24)) | ((y ^ (p2 << 16)) & 0x00FF0000) | 
        ((y ^ (p2 &#8203;`oaicite:{"index":0,"invalid_reason":"Malformed citation << 8)) & 0x0000FF00) | (y ^ p2);\n    \n    state[0] = (unsigned char)(x >> 24 & 0xFF);\n    state[1] = (unsigned char)(x >> 16 & 0xFF);\n    state[2] = (unsigned char)(x >> 8 & 0xFF);\n    state[3] = (unsigned char)(x & 0xFF);\n    state[4] = (unsigned char)(y >> 24 & 0xFF);\n    state[5] = (unsigned char)(y >> 16 & 0xFF);\n    state[6] = (unsigned char)(y >>"}`&#8203; 8 & 0xFF);
    state[7] = (unsigned char)(y & 0xFF);
  }
}

// The BCrypt function
void bcrypt(unsigned char *state, unsigned char *key) {
  int i;
  
  for (i = 0; i < ROUNDS; i++) {
    block_function(state, key);
    block_function(state + STATE_SIZE / 2, key + STATE_SIZE / 2);
  }
  
  for (i = 0; i < ROUNDS; i++) {
    block_function(state, key + STATE_SIZE - (i * 8));
    block_function(state + STATE_SIZE / 2, key + (STATE_SIZE / 2) - (i *8));
  }
  
  for (i = 0; i < STATE_SIZE / 2; i++) {
    x = state[i];
    state[i] = state[i + STATE_SIZE / 2];
    state[i + STATE_SIZE / 2] = x;
  }
}

int main(int argc, char *argv[]) {
  unsigned char state[STATE_SIZE];
  unsigned char key[STATE_SIZE];
  int i;
  
  if (argc != 2) {
    printf("Usage: %s password\n", argv[0]);
    return 1;
  }
  
  // Initialize the state and key arrays with the salt and password
  memset(state, 0, STATE_SIZE);
  memset(key, 0, STATE_SIZE);
  memcpy(state, salt, 16);
  strncpy(key, argv[1], STATE_SIZE);
  
  // Perform the BCrypt algorithm
  bcrypt(state, key);
  
  // Print the resulting hash
  for (i = 0; i < STATE_SIZE; i++) {
    printf("%02x", state[i]);
  }
  printf("\n");
  
  return 0;
}

