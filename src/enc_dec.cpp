#include <cstring>

#include "file_split_merge.h"
#include "enc_dec.h"
#include "customexceptions.h"

auto permissions        = fs::perms::owner_all|fs::perms::group_all;

void encryptDecrypt::gen_params(byte key[KEY_SIZE]) {
  if (int rc = RAND_bytes(key, KEY_SIZE);rc != 1)
   throw random_generation_error("RAND_bytes key failed");

}

void encryptDecrypt::aes_encrypt(
   const byte key[KEY_SIZE], const byte iv[BLOCK_SIZE],
   secure_string& ctext, size_t horcrux_count,
   std::string& inputPath, std::string& outPutPath) {

   const std::string ptext = pathStringHandler::filePathToString(inputPath);
   EVP_CIPHER_CTX_free_ptr ctx(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);
   //std::cout << "Plain  text from file is :" << ptext << std::endl;
   //auto ctx_ptr = ctx.get();
   //EVP_CIPHER_CTX_set_padding(ctx.get(), 0);
   if (int rc = EVP_EncryptInit_ex(ctx.get(), EVP_aes_256_cbc(), NULL, key, iv);rc != 1)    
   {
      EVP_CIPHER_CTX_reset(ctx.get());
      throw encryption_error("EVP_EncryptInit_ex failed");
    }
       // Recovered text expands upto BLOCK_SIZE
    ctext.resize(ptext.size());// + BLOCK_SIZE);
    int out_len1 = static_cast<int>(ctext.size());

   if (int rc = EVP_EncryptUpdate(ctx.get(), (byte*)&ctext[0], &out_len1,
                         (const byte*)&ptext[0], (int)ptext.size());rc != 1) 
   {
      EVP_CIPHER_CTX_reset(ctx.get());
      throw encryption_error("EVP_EncryptUpdate failed");
   } 
   int out_len2 = (int)ctext.size() - out_len1;
   if (int rc = EVP_EncryptFinal_ex(ctx.get(), (byte*)&ctext[0] + out_len1, &out_len2); rc != 1)    
   {
      EVP_CIPHER_CTX_reset(ctx.get());
      throw encryption_error("EVP_EncryptFinal_ex failed");
   }

  // Set cipher text size now that we know it

   //ctext.resize(out_len1 + out_len2);
   fs::create_directory("tmp0");
   fs::permissions("tmp0", permissions);
   std::ofstream ofile("tmp0/encrypted.txt", std::ofstream::out| std::ofstream::app);
   std::string encrypted_file = std::string("tmp0/encrypted.txt");
   //ofile.open(encrypted_file, std::ofstream::out| std::ofstream::app);
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
     
   const std::string& ctext =
   pathStringHandler::filePathToString(outputFilePath);
   std::cout <<"cipher text from file is :"<<ctext<< std::endl;
   EVP_CIPHER_CTX_free_ptr ctx(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);
   //EVP_CIPHER_CTX_set_padding(ctx.get(), 0);
   std::cerr<<"EVP_CIPHER_CTX_free_ptr initialized successfully\n";
   //auto ctx_ptr = ctx.get();
   if (int rc = EVP_DecryptInit_ex(ctx.get(), EVP_aes_256_cbc(), NULL, key, iv); rc != 1)
   {
      EVP_CIPHER_CTX_reset(ctx.get());
      std::cerr<<"EVP_DecryptInit_ex failed\n";
      throw decryption_error("EVP_DecryptInit_ex failed");
   }
   // recovered text contracts upto BLOCK_SIZE
   rtext.resize(ctext.size());//+ BLOCK_SIZE);
   int out_len1 = static_cast<int>(rtext.size());
   int out_len2 = 0;
   if (int rc = EVP_DecryptUpdate(ctx.get(), (byte*)&rtext[0], &out_len1,
                      (const byte*)&ctext[0], static_cast<int>(ctext.size())); rc != 1)
   { 
      EVP_CIPHER_CTX_reset(ctx.get());
      std::cerr<<"EVP_DecryptUpdate failed\n";
      throw decryption_error("EVP_DecryptUpdate failed");
   }

   std::cout <<"out_len1 before EVP_DecryptFinal_ex is :"<<out_len1 << std::endl;
   if(int rc = EVP_DecryptFinal_ex(ctx.get(), (byte*)&rtext[0] + out_len1, &out_len2);rc!=1)
    { 
      EVP_CIPHER_CTX_reset(ctx.get());
      std::cout <<"out_len1 = " << out_len1 << " out_len2 = " << out_len2 << std::endl;
      throw decryption_error("EVP_DecryptFinal_ex failed");
    }
    std::cout <<"out_len1 after EVP_DecryptFinal_ex is :"<<out_len1 << std::endl;
    // set recovered text size now that we know it
   rtext.resize(out_len1 + out_len2);
   std::ofstream(outputFilePath) <<rtext << '\n';
}
