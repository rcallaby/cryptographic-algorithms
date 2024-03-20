import random

KEYLEN = 512

class PublicKey:
    def __init__(self):
        self.n = 0
        self.e = 0

class PrivateKey:
    def __init__(self):
        self.n = 0
        self.d = 0

def gcd(a, b):
    while b != 0:
        a, b = b, a % b
    return a

def mod_exp(base, exp, mod):
    res = 1
    base %= mod
    while exp > 0:
        if exp & 1:
            res = (res * base) % mod
        base = (base * base) % mod
        exp >>= 1
    return res

def encrypt(message, key):
    ciphertext = []
    for char in message:
        ciphertext.append(mod_exp(ord(char), key.e, key.n))
    return ciphertext

def decrypt(ciphertext, key):
    decrypted_message = ''
    for char in ciphertext:
        decrypted_message += chr(mod_exp(char, key.d, key.n))
    return decrypted_message

def generate_keypair():
    public_key = PublicKey()
    private_key = PrivateKey()

    p, q, phi, e, d = 0, 0, 0, 0, 0

    # Generate two prime numbers, p and q
    while True:
        p = random.randint(50, KEYLEN // 2 - 50)
        q = random.randint(50, KEYLEN // 2 - 50)
        if gcd(p, q) == 1:
            break

    # Compute n = pq
    public_key.n = p * q
    private_key.n = public_key.n

    # Compute phi(n) = phi(p)phi(q) = (p-1)(q-1)
    phi = (p - 1) * (q - 1)

    # Select a small odd integer e such that gcd(e, phi(n)) = 1
    e = 3
    while gcd(e, phi) != 1:
        e += 2
    public_key.e = e

    # Compute d such that de = 1 (mod phi(n))
    d = 2
    while (d * e) % phi != 1:
        d += 1
    private_key.d = d

    return public_key, private_key

def main():
    message = input("Enter message to encrypt: ")

    # Generate keypair
    public_key, private_key = generate_keypair()

    # Encrypt message
    ciphertext = encrypt(message, public_key)

    # Decrypt message
    decrypted_message = decrypt(ciphertext, private_key)

    print("Original message:", message)
    print("Encrypted message:", ciphertext)
    print("Decrypted message:", decrypted_message)

if __name__ == "__main__":
    main()
