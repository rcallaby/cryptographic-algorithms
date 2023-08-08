using System;

namespace TwofishExample
{
    class Twofish
    {
        private const int BLOCK_SIZE = 16;
        private const int NUM_ROUNDS = 16;

        private uint[] subKeys;

        private static readonly byte[] SBox = {
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

        private static readonly byte[,] Mds = {
            { 0x01, 0xEF, 0x5B, 0x5B },
            { 0x5B, 0xEF, 0xEF, 0x01 },
            { 0xEF, 0x5B, 0x01, 0xEF },
            { 0xEF, 0x01, 0xEF, 0x5B }
        };

        private static uint BytesToUInt32(byte[] bytes, int offset)
        {
            return ((uint)bytes[offset] << 24) |
                   ((uint)bytes[offset + 1] << 16) |
                   ((uint)bytes[offset + 2] << 8) |
                   bytes[offset + 3];
        }

        private static void UInt32ToBytes(uint value, byte[] bytes, int offset)
        {
            bytes[offset] = (byte)((value >> 24) & 0xFF);
            bytes[offset + 1] = (byte)((value >> 16) & 0xFF);
            bytes[offset + 2] = (byte)((value >> 8) & 0xFF);
            bytes[offset + 3] = (byte)(value & 0xFF);
        }

        private uint H(byte[] x, int keyIndex)
        {
            uint result = 0;
            for (int i = 0; i < 4; i++)
            {
                result ^= subKeys[keyIndex + i] ^ x[i];
            }
            return subKeys[4 + keyIndex] ^
                   SBox[result & 0xFF] ^
                   (SBox[(result >> 8) & 0xFF] << 8) ^
                   (SBox[(result >> 16) & 0xFF] << 16) ^
                   (SBox[(result >> 24) & 0xFF] << 24);
        }

        private void KeySchedule(byte[] key)
        {
            subKeys = new uint[40];
            uint[] k = new uint[8];

            for (int i = 0; i < 8; i++)
            {
                k[i] = BytesToUInt32(key, i * 4);
            }

            for (int i = 0; i < 40; i += 2)
            {
                uint a = H(BitConverter.GetBytes(i), 0);
                uint b = H(BitConverter.GetBytes(i), 1);
                b = (b << 8) | (b >> 24);
                subKeys[i] = a + b;
                subKeys[i + 1] = (a << 9 | a >> 23) + b;
            }

            for (int i = 0; i < 4; i++)
            {
                subKeys[i] ^= k[i];
            }
        }

        private void MatrixMultiply(byte[,] matrix, byte[] vector, byte[] result)
        {
            for (int i = 0; i < 4; i++)
            {
                result[i] = 0;
                for (int j = 0; j < 4; j++)
                {
                    result[i] ^= (byte)(matrix[i, j] * vector[j]);
                }
            }
        }

        private void EncryptBlock(byte[] plaintext, byte[] ciphertext)
        {
            uint[] x = new uint[4];
            for (int i = 0; i < 4; i++)
            {
                x[i] = BytesToUInt32(plaintext, i * 4);
            }

            x[0] ^= subKeys[0];
            x[1] ^= subKeys[1];
            x[2] ^= subKeys[2];
            x[3] ^= subKeys[3];

            for (int round = 0; round < NUM_ROUNDS; round += 2)
            {
                uint t0 = H(BitConverter.GetBytes(x[0]), 4 * round);
                uint t1 = H(BitConverter.GetBytes(x[1]), 4 * round + 2);

                x[2] ^= t0 + t1;
                x[2] = (x[2] >> 1) | (x[2] << 31);
                x[3] = (x[3] << 1) | (x[3] >> 31);
                x[3] ^= t0 + 2 * t1;

                t0 = H(BitConverter.GetBytes(x[2]), 4 * round + 1);
                t1 = H(BitConverter.GetBytes(x[3]), 4 * round + 3);

                x[0] ^= t0 + t1;
                x[0] = (x[0] >> 1) | (x[0] << 31);
                x[1] = (x[1] << 1) | (x[1] >> 31);
                x[1] ^= t0 + 2 * t1;
            }

            x[2] ^= subKeys[4 * NUM_ROUNDS];
            x[3] ^= subKeys[4 * NUM_ROUNDS + 1];
            x[0] ^= subKeys[4 * NUM_ROUNDS + 2];
            x[1] ^= subKeys[4 * NUM_ROUNDS + 3];

            for (int i = 0; i < 4; i++)
            {
                UInt32ToBytes(x[i], ciphertext, i * 4);
            }
        }

        private void DecryptBlock(byte[] ciphertext, byte[] plaintext)
        {
            uint[] x = new uint[4];
            for (int i = 0; i < 4; i++)
            {
                x[i] = BytesToUInt32(ciphertext, i * 4);
            }

            x[2] ^= subKeys[4 * NUM_ROUNDS];
            x[3] ^= subKeys[4 * NUM_ROUNDS + 1];
            x[0] ^= subKeys[4 * NUM_ROUNDS + 2];
            x[1] ^= subKeys[4 * NUM_ROUNDS + 3];

            for (int round = NUM_ROUNDS - 2; round >= 0; round -= 2)
            {
                uint t0 = H(BitConverter.GetBytes(x[2]), 4 * round + 1);
                uint t1 = H(BitConverter.GetBytes(x[3]), 4 * round + 3);

                x[0] ^= t0 + t1;
                x[0] = (x[0] >> 1) | (x[0] << 31);
                x[1] = (x[1] << 1) | (x[1] >> 31);
                x[1] ^= t0 + 2 * t1;

                t0 = H(BitConverter.GetBytes(x[0]), 4 * round);
                t1 = H(BitConverter.GetBytes(x[1]), 4 * round + 2);

                x[2] ^= t0 + t1;
                x[2] = (x[2] >> 1) | (x[2] << 31);
                x[3] = (x[3] << 1) | (x[3] >> 31);
                x[3] ^= t0 + 2 * t1;
            }

            x[0] ^= subKeys[0];
            x[1] ^= subKeys[1];
            x[2] ^= subKeys[2];
            x[3] ^= subKeys[3];

            for (int i = 0; i < 4; i++)
            {
                UInt32ToBytes(x[i], plaintext, i * 4);
            }
        }

        static void Main(string[] args)
        {
            // Example usage
            Twofish twofish = new Twofish();
            byte[] key = {
                0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
                0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10,
                0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
            };

            twofish.KeySchedule(key);

            byte[] plaintext = {
                0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0,
                0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF
            };

            byte[] ciphertext = new byte[BLOCK_SIZE];

            twofish.EncryptBlock(plaintext, ciphertext);

            Console.Write("Ciphertext: ");
            foreach (byte b in ciphertext)
            {
                Console.Write(b.ToString("X2") + " ");
            }
            Console.WriteLine();

            byte[] decryptedText = new byte[BLOCK_SIZE];

            twofish.DecryptBlock(ciphertext, decryptedText);

            Console.Write("Decrypted Text: ");
            foreach (byte b in decryptedText)
            {
                Console.Write(b.ToString("X2") + " ");
            }
            Console.WriteLine();

            Console.ReadLine();
        }
    }
}
