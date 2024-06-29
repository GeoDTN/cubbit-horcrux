#include <string>

#include "enc_dec_tests.h"

void encryptDecrypt_Tests::SetUp() { encrypt_decrypt = new encryptDecrypt(); }

void encryptDecrypt_Tests::TearDown() { delete encrypt_decrypt; }
bool encryptDecrypt_Tests::isValid() {
  byte iv[]  =
      {   0x00, 0x01, 0x02, 0x03,
       0x04, 0x05, 0x06, 0x07,
       0x08, 0x09, 0x0a, 0x0b,
       0x0c, 0x0d, 0x0e, 0x0f };
  byte key[KEY_SIZE];
  encrypt_decrypt->gen_params(key);
  fs::path plain_input_path      = "original.txt";
  fs::path encrypted_output_path = "encrypted.txt";
  fs::path decrypted_output_path = "decrypted.txt";
  encrypt_decrypt->encrypt(key, iv, 3, plain_input_path,
                               encrypted_output_path);

  encrypt_decrypt->decrypt(key, iv, encrypted_output_path,
                               decrypted_output_path);
  OPENSSL_cleanse(key, KEY_SIZE);
  OPENSSL_cleanse(iv, BLOCK_SIZE);
  if(fs::file_size(plain_input_path) != fs::file_size(decrypted_output_path)) 
  return false;
  std::ifstream infile1 { plain_input_path}; 
	std::string file_contents1 {std::istreambuf_iterator<char>(infile1), std::istreambuf_iterator<char>()}; 
	std::ifstream infile2 { decrypted_output_path }; 
	std::string file_contents2 {std::istreambuf_iterator<char>(infile2), std::istreambuf_iterator<char>()}; 
	return file_contents1 == file_contents2; 

}
TEST_F(encryptDecrypt_Tests, Test_aes_encrypt_decrypt_positive) {
  EXPECT_EQ(true, isValid());
}

/*
TEST(encryptDecrypt_Tests, Test_aes_encrypt_decrypt_negative) {
{

}
*/
