def quarter_round(a, b, c, d):
    a = (a + b) & 0xFFFFFFFF
    d = (d ^ a) & 0xFFFFFFFF
    d = (d << 16 | d >> 16) & 0xFFFFFFFF

    c = (c + d) & 0xFFFFFFFF
    b = (b ^ c) & 0xFFFFFFFF
    b = (b << 12 | b >> 20) & 0xFFFFFFFF

    a = (a + b) & 0xFFFFFFFF
    d = (d ^ a) & 0xFFFFFFFF
    d = (d << 8 | d >> 24) & 0xFFFFFFFF

    c = (c + d) & 0xFFFFFFFF
    b = (b ^ c) & 0xFFFFFFFF
    b = (b << 7 | b >> 25) & 0xFFFFFFFF

    return a, b, c, d

def chacha20_block(key, counter, nonce):
    state = [
        0x61707865, 0x3320646e, 0x79622d32, 0x6b206574,  # Constants
        key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7],  # Key
        counter,  # Counter
        nonce[0], nonce[1], nonce[2], nonce[3],  # Nonce
    ]

    for _ in range(10):  # 10 rounds
        # Odd rounds
        state[0], state[4], state[8], state[12] = quarter_round(state[0], state[4], state[8], state[12])
        state[1], state[5], state[9], state[13] = quarter_round(state[1], state[5], state[9], state[13])
        state[2], state[6], state[10], state[14] = quarter_round(state[2], state[6], state[10], state[14])
        state[3], state[7], state[11], state[15] = quarter_round(state[3], state[7], state[11], state[15])

        # Even rounds
        state[0], state[5], state[10], state[15] = quarter_round(state[0], state[5], state[10], state[15])
        state[1], state[6], state[11], state[12] = quarter_round(state[1], state[6], state[11], state[12])
        state[2], state[7], state[8], state[13] = quarter_round(state[2], state[7], state[8], state[13])
        state[3], state[4], state[9], state[14] = quarter_round(state[3], state[4], state[9], state[14])

    # Add the original state to the new state
    for i in range(16):
        state[i] = (state[i] + [0x61707865, 0x3320646e, 0x79622d32, 0x6b206574][i]) & 0xFFFFFFFF

    result = b''
    for i in range(16):
        result += state[i].to_bytes(4, byteorder='little')

    return result

def chacha20_encrypt(plaintext, key, nonce, counter=0):
    ciphertext = b''
    block_count = len(plaintext) // 64

    for i in range(block_count):
        block = chacha20_block(key, counter + i, nonce)
        ciphertext += bytes(x ^ y for x, y in zip(plaintext[i * 64: (i + 1) * 64], block))

    return ciphertext

def chacha20_decrypt(ciphertext, key, nonce, counter=0):
    return chacha20_encrypt(ciphertext, key, nonce, counter)  # Encryption and decryption are the same in ChaCha20

def main():
    key = b'\x00' * 32  # Replace with your key
    nonce = b'\x00' * 8  # Replace with your nonce
    counter = 0

    # Example plaintext
    plaintext = b'This is a sample plaintext for ChaCha20 encryption.'

    # Encryption
    ciphertext = chacha20_encrypt(plaintext, key, nonce, counter)
    print(f"Ciphertext (hex): {ciphertext.hex()}")

    # Decryption
    decrypted_text = chacha20_decrypt(ciphertext, key, nonce, counter)
    print(f"Decrypted Text: {decrypted_text.decode('utf-8')}")

if __name__ == "__main__":
    main()
