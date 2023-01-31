#include <stdio.h>
#include <string.h>

typedef unsigned char BYTE;
typedef unsigned int WORD;

const int BLOCK_SIZE = 64;
const int HASH_SIZE = 32;

const WORD K[] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

void pad_message(BYTE message[], unsigned int message_len, BYTE padded_message[])
{
    int i;
    for (i = 0; i < message_len; i++)
        padded_message[i] = message[i];
    padded_message[message_len] = 0x80;
    for (i = message_len + 1; i < BLOCK_SIZE; i++)
        padded_message[i] = 0x00;
    if (message_len % BLOCK_SIZE > BLOCK_SIZE - 8)
    {
        for (i = BLOCK_SIZE; i < 2 * BLOCK_SIZE; i++)
            padded_message[i= 0x00];
    }
    unsigned long long bit_len = message_len * 8;
    padded_message[BLOCK_SIZE - 8] = bit_len & 0xff;
    padded_message[BLOCK_SIZE - 7] = (bit_len >> 8) & 0xff;
    padded_message[BLOCK_SIZE - 6] = (bit_len >> 16) & 0xff;
    padded_message[BLOCK_SIZE - 5] = (bit_len >> 24) & 0xff;
    padded_message[BLOCK_SIZE - 4] = (bit_len >> 32) & 0xff;
    padded_message[BLOCK_SIZE - 3] = (bit_len >> 40) & 0xff;
    padded_message[BLOCK_SIZE - 2] = (bit_len >> 48) & 0xff;
    padded_message[BLOCK_SIZE - 1] = (bit_len >> 56) & 0xff;
}

void hash_block(BYTE block[], WORD hash[])
{
    int i, j;
    WORD a, b, c, d, e, f, g, h;
    WORD w[64];
    WORD temp1, temp2;
    for (i = 0; i < 16; i++)
    {
        w[i] = (block[i * 4 + 0] << 24) | (block[i * 4 + 1] << 16) | (block[i * 4 + 2] << 8) | (block[i * 4 + 3]);
    }
    for (i = 16; i < 64; i++)
    {
        w[i] = (w[i - 16] + (((w[i - 15] >> 17) | (w[i - 15] &#8203;`oaicite:{"index":0,"invalid_reason":"Malformed citation << 15)) ^ ((w[i - 15] >>"}`&#8203; 19)) | (w[i - 15] &#8203;`oaicite:{"index":1,"invalid_reason":"Malformed citation << 13)) ^ (w[i - 15] >> 10)) + w[i - 7] + (((w[i - 2] >>"}`&#8203; 7) | (w[i - 2] &#8203;`oaicite:{"index":2,"invalid_reason":"Malformed citation << 25)) ^ ((w[i - 2] >>"}`&#8203; 18) | (w[i - 2] &#8203;`oaicite:{"index":3,"invalid_reason":"Malformed citation << 14)) ^ (w[i - 2] >>"}`&#8203; 3)));
    }
    a = hash[0];
    b = hash[1];
    c = hash[2];
    d = hash[3];
    e = hash[4];
    f = hash[5];
    g = hash[6];
    h = hash[7];
    for (i = 0; i < 64; i++)
    {
        temp1 = h + (((e >> 6) | (e &#8203;`oaicite:{"index":4,"invalid_reason":"Malformed citation << 26)) ^ ((e >>"}`&#8203; 11) | (e &#8203;`oaicite:{"index":5,"invalid_reason":"Malformed citation << 21)) ^ ((e >>"}`&#8203; 25) | (e &#8203;`oaicite:{"index":6,"invalid_reason":"Malformed citation << 7))) + ((e & f) ^ (~e & g)) + K[i] + w[i];\n        temp2 = (((a >>"}`&#8203; 2) | (a &#8203;`oaicite:{"index":7,"invalid_reason":"Malformed citation << 30)) ^ ((a >>"}`&#8203; 13) | (a &#8203;`oaicite:{"index":8,"invalid_reason":"Malformed citation << 19)) ^ ((a >>"}`&#8203; 22) | (a << 10))) + ((a & b) ^ (a & c) ^ (b & c));
        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }
    hash[0] += a;
    hash[1] += b;
    hash[2] += c;
    hash[3] += d;
    hash[4] += e;
    hash[5] += f;
    hash[6] += g;
    hash[7] += h;
}

void hash_message(BYTE message[], DWORD message_len, BYTE hash[])
{
    int i, j;
    BYTE block[BLOCK_SIZE];
    WORD hash_temp[8];
    for (i = 0; i < 8; i++)
    {
        hash_temp[i] = H[i];
    }
    for (i = 0; i < message_len / BLOCK_SIZE; i++)
    {
        for (j = 0; j < BLOCK_SIZE; j++)
        {
            block[j] = message[i * BLOCK_SIZE + j];
        }
        hash_block(block, hash_temp);
    }
    BYTE padded_message[BLOCK_SIZE];
    pad_message(message + (message_len / BLOCK_SIZE) * BLOCK_SIZE, message_len % BLOCK_SIZE, padded_message);
    hash_block(padded_message, hash_temp);
    for (i = 0; i < 8; i++)
    {
        hash[i * 4 + 0] = (hash_temp[i] >> 24) & 0xff;
        hash[i * 4 + 1] = (hash_temp[i] >> 16) & 0xff;
        hash[i * 4 + 2] = (hash_temp[i] >> 8) & 0xff;
        hash[i * 4 + 3] = (hash_temp[i]) & 0xff;
    }
}

int main(int argc, char* argv[])
{
    BYTE message[] = "The quick brown fox jumps over the lazy dog";
    DWORD message_len = strlen((char*)message);
    BYTE hash[HASH_SIZE];
    hash_message(message, message_len, hash);
    int i;
    for (i = 0; i < HASH_SIZE; i++)
    {
        printf("%02x", hash[i]);
    }
    printf("\n");
    return 0;
}

