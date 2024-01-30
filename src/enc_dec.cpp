#include <cstring>

#include "file_split_merge.h"
#include "enc_dec.h"
#include "customexceptions.h"

void encryptDecrypt::gen_params(byte key[KEY_SIZE]) {
  if (int rc = RAND_bytes(key, KEY_SIZE);rc != 1)
   throw random_generation_error("RAND_bytes key failed");

}

void encryptDecrypt::aes_encrypt(
    const byte key[KEY_SIZE], const byte iv[BLOCK_SIZE],
    secure_string& ctext, size_t horcrux_count,
    std::string& inputPath, std::string& outPutPath) {
    system("sudo rm -rf tmp0 && sudo mkdir -p tmp0 && sudo touch tmp0/encrypted.txt");
    std::string encrypted_file = std::string("tmp0/encrypted.txt");

    const secure_string ptext = pathStringHandler::filePathToString(inputPath);
    std::cout << "Plain  text from file is :" << ptext << std::endl;
    EVP_CIPHER_CTX_free_ptr ctx(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);
    auto ctx_ptr = ctx.get();
    EVP_CIPHER_CTX_set_padding(ctx_ptr, 0);
    if (int rc = EVP_EncryptInit_ex(ctx_ptr, EVP_aes_256_cbc(), NULL, key, iv);rc != 1)
     
    {
         EVP_CIPHER_CTX_cleanup(ctx_ptr);
         throw encryption_error("EVP_EncryptInit_ex failed");
    }

       // Recovered text expands upto BLOCK_SIZE
    ctext.resize(ptext.size() + BLOCK_SIZE);
    int out_len1 = (int)ctext.size();

     if (int rc = EVP_EncryptUpdate(ctx_ptr, (byte*)&ctext[0], &out_len1,
                         (const byte*)&ptext[0], (int)ptext.size());rc != 1) 
      {
         EVP_CIPHER_CTX_cleanup(ctx_ptr);
         throw encryption_error("EVP_EncryptUpdate failed");
      } 
     int out_len2 = (int)ctext.size() - out_len1;
     if (int rc = EVP_EncryptFinal_ex(ctx_ptr, (byte*)&ctext[0] + out_len1, &out_len2); rc != 1)    
     {
         EVP_CIPHER_CTX_cleanup(ctx_ptr);
         throw encryption_error("EVP_EncryptFinal_ex failed");
     }

  // Set cipher text size now that we know it

    ctext.resize(out_len1 + out_len2);
    std::ofstream ofile{};//((encrypted_file), std::ofstream::out | std::ofstream::app);
    system("sudo chmod -R 777 tmp0");
    ofile.open(encrypted_file, std::ofstream::out| std::ofstream::app);
    if(!ofile.is_open())
    {
        std::cout<<"Unable to open the output file.\n";
        exit(EXIT_FAILURE);
    }
    ofile.unsetf(std::ios_base::skipws);
    //std::cout<<"encrypted text: "<< ctext;
    /*std::string str{};
    while(std::getline (ctext,str))
    {
    std::cout<<str<<"\n";
    str +="\n";
    ofile<<str;
  
    }*/
    ofile << ctext;// << '\n';
    ofile.close();
   
    fileSplitMerge::split(horcrux_count, encrypted_file, outPutPath);
    
}

void encryptDecrypt::aes_decrypt(
    const byte key[KEY_SIZE], const byte iv[BLOCK_SIZE],
     secure_string& rtext,
     std::string& inputFilesPath, std::string& outputFilePath) {
     fileSplitMerge::merge(inputFilesPath, outputFilePath);
     
     const secure_string& ctext =
     pathStringHandler::filePathToString(outputFilePath);
      std::cout <<"Cipher text from file is :"<<std::endl;

      EVP_CIPHER_CTX_free_ptr ctx(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);
      auto ctx_ptr = ctx.get();
      EVP_CIPHER_CTX_set_padding(ctx_ptr, 0);
      if (int rc = EVP_DecryptInit_ex(ctx_ptr, EVP_aes_256_cbc(), NULL, key, iv); rc != 1)
      {
         EVP_CIPHER_CTX_cleanup(ctx_ptr);
         throw decryption_error("EVP_DecryptInit_ex failed");
      }
       // Recovered text contracts upto BLOCK_SIZE
       rtext.resize(ctext.size());
       int out_len1 = (int)rtext.size();

       if (int rc = EVP_DecryptUpdate(ctx_ptr, (byte*)&rtext[0], &out_len1,
                         (const byte*)&ctext[0], (int)ctext.size()); rc != 1)
      { 
         EVP_CIPHER_CTX_cleanup(ctx_ptr);
         throw decryption_error("EVP_DecryptUpdate failed");
      }

        int out_len2 = (int)rtext.size() - out_len1;
        std::cout <<"Retrived text from file is :"<<std::endl;
        if(int rc = EVP_DecryptFinal_ex(ctx_ptr, (byte*)&rtext[0] + out_len1, &out_len2);rc!=1)
         { 
           EVP_CIPHER_CTX_cleanup(ctx_ptr);
           throw decryption_error("EVP_DecryptFinal_ex failed");
         }
         // Set recovered text size now that we know it
        rtext.resize(out_len1 + out_len2);
       std::ofstream(outputFilePath) <<rtext << '\n';


}

