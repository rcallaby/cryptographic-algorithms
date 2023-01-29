#include <CUnit/Basic.h>
#include "blowfish.h"

/*
    This is an example of how to construct a unit test using the CUnit test program.
*/

void test_encryption(void) {
    unsigned char key[] = "mysecretkey";
    unsigned char plaintext[] = "This is a test message.";
    unsigned char ciphertext[sizeof(plaintext)];
    unsigned char decrypted[sizeof(plaintext)];
    
    BF_KEY bfkey;
    BF_set_key(&bfkey, strlen(key), key);
    BF_ecb_encrypt(plaintext, ciphertext, &bfkey, BF_ENCRYPT);
    BF_ecb_encrypt(ciphertext, decrypted, &bfkey, BF_DECRYPT);
    
    CU_ASSERT_NSTRING_EQUAL(plaintext, decrypted, sizeof(plaintext));
}

int main() {
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("BlowFish Tests", NULL, NULL);
    CU_add_test(suite, "Encryption Test", test_encryption);
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}
