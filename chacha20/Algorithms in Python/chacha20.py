from Crypto.Cipher import ChaCha20
import binascii

def read_file(file_path):
    with open(file_path, 'rb') as file:
        return file.read()

def write_file(file_path, data):
    with open(file_path, 'wb') as file:
        file.write(data)

def chacha20_encrypt(plaintext, key):
    cipher = ChaCha20.new(key=key)
    ciphertext = cipher.encrypt(plaintext)
    return ciphertext

def chacha20_decrypt(ciphertext, key):
    cipher = ChaCha20.new(key=key)
    plaintext = cipher.decrypt(ciphertext)
    return plaintext

def main():
    # Input file paths
    encrypted_text_file = 'encrypted_text.hex'
    key_file = 'key.hex'
    output_file = 'decrypted_text.txt'

    # Read hex-encoded data from files
    encrypted_text_hex = read_file(encrypted_text_file)
    key_hex = read_file(key_file)

    # Convert hex to bytes
    encrypted_text = binascii.unhexlify(encrypted_text_hex)
    key = binascii.unhexlify(key_hex)

    # Decrypt the text
    decrypted_text = chacha20_decrypt(encrypted_text, key)

    # Write the decrypted text to the output file
    write_file(output_file, decrypted_text)

if __name__ == "__main__":
    main()
