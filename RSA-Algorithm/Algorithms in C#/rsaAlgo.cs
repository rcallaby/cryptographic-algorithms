using System;
using System.Numerics;
using System.Text;

class Program
{
    const int KEYLEN = 512;

    struct PublicKey
    {
        public BigInteger n;
        public BigInteger e;
    }

    struct PrivateKey
    {
        public BigInteger n;
        public BigInteger d;
    }

    static BigInteger GCD(BigInteger a, BigInteger b)
    {
        while (b != 0)
        {
            BigInteger temp = b;
            b = a % b;
            a = temp;
        }
        return a;
    }

    static BigInteger ModExp(BigInteger baseValue, BigInteger exp, BigInteger mod)
    {
        BigInteger res = 1;
        baseValue %= mod;
        while (exp > 0)
        {
            if ((exp & 1) == 1)
                res = (res * baseValue) % mod;
            baseValue = (baseValue * baseValue) % mod;
            exp >>= 1;
        }
        return res;
    }

    static void Encrypt(string message, PublicKey key, ref StringBuilder ciphertext)
    {
        for (int i = 0; i < message.Length; ++i)
        {
            BigInteger charValue = message[i];
            BigInteger encryptedChar = ModExp(charValue, key.e, key.n);
            ciphertext.Append((char)encryptedChar);
        }
    }

    static void Decrypt(string ciphertext, PrivateKey key, ref StringBuilder message)
    {
        for (int i = 0; i < ciphertext.Length; ++i)
        {
            BigInteger charValue = ciphertext[i];
            BigInteger decryptedChar = ModExp(charValue, key.d, key.n);
            message.Append((char)decryptedChar);
        }
    }

    static void GenerateKeyPair(out PublicKey publicKey, out PrivateKey privateKey)
    {
        Random random = new Random();
        BigInteger p, q, phi, e, d;

        // Generate two prime numbers, p and q
        do
        {
            p = GeneratePrime(random, KEYLEN / 2);
            q = GeneratePrime(random, KEYLEN / 2);
        } while (GCD(p, q) != 1);

        // Compute n = pq
        publicKey.n = p * q;
        privateKey.n = publicKey.n;

        // Compute phi(n) = phi(p)phi(q) = (p-1)(q-1)
        phi = (p - 1) * (q - 1);

        // Select a small odd integer e such that gcd(e, phi(n)) = 1
        e = 3;
        while (GCD(e, phi) != 1)
            e += 2;
        publicKey.e = e;

        // Compute d such that de = 1 (mod phi(n))
        d = 2;
        while ((d * e) % phi != 1)
            ++d;
        privateKey.d = d;
    }

    static BigInteger GeneratePrime(Random random, int bitLength)
    {
        BigInteger prime;
        do
        {
            byte[] bytes = new byte[bitLength / 8];
            random.NextBytes(bytes);
            prime = new BigInteger(bytes);
            prime = BigInteger.Abs(prime);
        } while (!IsProbablePrime(prime, 20));
        return prime;
    }

    static bool IsProbablePrime(BigInteger n, int k)
    {
        if (n <= 1)
            return false;
        if (n <= 3)
            return true;
        if ((n & 1) == 0)
            return false;

        BigInteger d = n - 1;
        int s = 0;
        while ((d & 1) == 0)
        {
            d >>= 1;
            s += 1;
        }

        for (int i = 0; i < k; i++)
        {
            using (RandomNumberGenerator rng = RandomNumberGenerator.Create())
            {
                byte[] bytes = new byte[n.ToByteArray().LongLength];
                BigInteger a;
                do
                {
                    rng.GetBytes(bytes);
                    a = new BigInteger(bytes);
                } while (a < 2 || a >= n - 2);

                BigInteger x = BigInteger.ModPow(a, d, n);
                if (x == 1 || x == n - 1)
                    continue;

                for (int r = 1; r < s; r++)
                {
                    x = BigInteger.ModPow(x, 2, n);
                    if (x == 1)
                        return false;
                    if (x == n - 1)
                        break;
                }

                if (x != n - 1)
                    return false;
            }
        }
        return true;
    }

    static void Main()
    {
        Console.Write("Enter message to encrypt: ");
        string message = Console.ReadLine();

        PublicKey publicKey;
        PrivateKey privateKey;

        // Generate keypair
        GenerateKeyPair(out publicKey, out privateKey);

        StringBuilder ciphertext = new StringBuilder();
        StringBuilder decryptedMessage = new StringBuilder();

        // Encrypt message
        Encrypt(message, publicKey, ref ciphertext);

        // Decrypt message
        Decrypt(ciphertext.ToString(), privateKey, ref decryptedMessage);

        Console.WriteLine("Original message: " + message);
        Console.WriteLine("Encrypted message: " + ciphertext);
        Console.WriteLine("Decrypted message: " + decryptedMessage);
    }
}
