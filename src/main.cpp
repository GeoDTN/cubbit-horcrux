#include <iostream>
#include <cstring>

#include "enc_dec.h"
#include "base64.h"
#include "customexceptions.h"

int main(int argc, char* argv[]) {

  if (argc < 5) {
    std::cerr << "Inavlid command line arguments. Please use correct arguments"
              << std::endl;
    exit(EXIT_FAILURE);
  }
  std::filesystem::path input{argv[4]};
  std::filesystem::path output{argv[5]};

  if(!std::filesystem::exists(input)){
    std::cerr << "Input path does not exist"<< std::endl;
    exit(EXIT_FAILURE);
  }

  if(!std::filesystem::exists(output)){
    std::cerr << "Output path does not exist"<< std::endl;
    exit(EXIT_FAILURE);
  }

  byte iv[]  =
  {    0x00, 0x01, 0x02, 0x03,
       0x04, 0x05, 0x06, 0x07,
       0x08, 0x09, 0x0a, 0x0b,
       0x0c, 0x0d, 0x0e, 0x0f };
  // Load the necessary cipher
  EVP_add_cipher(EVP_aes_256_cbc());
  encryptDecrypt encrpt_decrypt{};
  secure_string ctext{}, rtext{};

  byte key[KEY_SIZE];
  encrpt_decrypt.gen_params(key);
  //unsigned char key[] = "70bbc518c57acca2c2001694648c40ddaf19e3b4fe1376ad656de8887a0a5ec2";
  std::string encrpt_decrypt_argument1 = std::string(argv[1]); 
  std::string encrpt_decrypt_argument2 = std::string(argv[2]);
  std::string input_path  = std::string(argv[4]);
  std::string output_path = std::string(argv[5]);

  try {
    if (encrpt_decrypt_argument1.compare("create") == 0 &&
        encrpt_decrypt_argument2.compare("-n") == 0) 
      {
      size_t horcrux_count = static_cast<size_t>(std::stoi(argv[3]));
      std::cout << "encrytion key  before encoding is :  " << key << std::endl;
      std::cout << "encrytion key is: " <<base64::base64_encode(key,sizeof (key))<< std::endl;
      encrpt_decrypt.aes_encrypt(key, iv, ctext, horcrux_count, input_path,
                                 output_path);
    } 
    else if (encrpt_decrypt_argument1.compare("load") == 0 &&
               encrpt_decrypt_argument2.compare("-k") == 0) 
    {
      const std::string& decryptionKey=base64::base64_decode(std::string(argv[3]));
      std::cout<<"decryption key is : " << decryptionKey << std::endl;
      std::cout<<"size of decryption key is : " << decryptionKey.size() << std::endl;
      byte d_key[KEY_SIZE];
      std::copy(decryptionKey.begin(), decryptionKey.end(), d_key);
     /*  for(auto i:d_key){
        std::cout<<"decryption_key:"<<i<<std::endl;
      } */
      
      encrpt_decrypt.aes_decrypt(d_key, iv, rtext, input_path, output_path);

    } else {
      std::cerr<<"Inavlid command line arguments. Please use correct arguments"<<'\n';
      exit(EXIT_FAILURE);
    }

  }
  catch (const std::exception& e) {
    std::cerr<<"Exception:"<<e.what()<<" thrown @ "<<__FILE__<<":"<<__LINE__<<":"<<std::endl;
  }
  catch (...) {
    std::cerr<<"Unexpected exception thrown"<<__FILE__<<":"<<__LINE__<<":"<<std::endl;
  }

  OPENSSL_cleanse(key, KEY_SIZE);
  OPENSSL_cleanse(iv, BLOCK_SIZE);

  return 0;
}

