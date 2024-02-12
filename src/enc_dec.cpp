#include <cstring>
#include <set>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>

#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

#include "file_split_merge.h"
#include "enc_dec.h"
#include "customexceptions.h"

const int  BUFSIZE            = 1024;
const EVP_CIPHER *cipher_type = EVP_aes_256_cbc();

void encryptDecrypt::gen_params(byte key[KEY_SIZE]) 
{
    if (int rc = RAND_bytes(key, KEY_SIZE);rc != 1)
    throw random_generation_error("RAND_bytes key failed");
}

void encryptDecrypt::encrypt(const byte key[KEY_SIZE], const byte iv[BLOCK_SIZE], size_t horcrux_count,fs::path input, fs::path output){
    fileSplitMerge::split(horcrux_count, fs::path(input), output);
    // Allow enough space in output buffer for additional block
    int cipher_block_size = EVP_CIPHER_block_size(cipher_type);
    unsigned char in_buf[BUFSIZE], out_buf[BUFSIZE + cipher_block_size];

    int num_bytes_read, out_len;
    EVP_CIPHER_CTX *ctx;
    std::string encrypted("encrypted");
    ctx = EVP_CIPHER_CTX_new();
    if(ctx == nullptr)
    {
        std::cerr<<"ERROR: EVP_CIPHER_CTX_new failed. OpenSSL error: "<<ERR_error_string(ERR_get_error(), nullptr)<<std::endl;
        return;
    }
    EVP_CIPHER_CTX_set_padding(ctx, 0);
    // Don't set key or IV right away; we want to check lengths 
    if(!EVP_CipherInit_ex(ctx, cipher_type, nullptr, nullptr, nullptr, 1))
    {
        std::cerr<<"ERROR: EVP_CipherInit_ex failed. OpenSSL error: "<<ERR_error_string(ERR_get_error(), nullptr)<<std::endl;
        return;
    }

    OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == KEY_SIZE);
    OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx)  == BLOCK_SIZE);

    // Now we can set key and IV
    if(!EVP_CipherInit_ex(ctx, nullptr, nullptr, key, iv, 1))
    {
        std::cerr<<"ERROR: EVP_CipherInit_ex failed. OpenSSL error: "<<ERR_error_string(ERR_get_error(), nullptr)<<std::endl;
        EVP_CIPHER_CTX_cleanup(ctx);
        return;
    }
    int encrypted_file_number = 0;
    std::string encrypted_file = std::string(output)+std::string("/encrypted.txt");

    for (auto file : fs::directory_iterator{output}) 
    {
        FILE* f_input = fopen(std::string((fs::path)file).c_str(), "rb");
        if (!f_input) 
        {
        // Unable to open file for reading 
            std::cerr<<"ERROR: fopen error:"<<strerror(errno)<< "line # " <<__LINE__<<std::endl;
            return;
        }
        std::cout<< "input file s : " <<std::string(input)<<std::endl;
        encrypted_file += std::to_string(encrypted_file_number);

        FILE* ofp = fopen(encrypted_file.c_str(), "wb");
        if (!ofp) 
        {
            // Unable to open file for writing 
            std::cerr<<"ERROR: fopen error:"<<strerror(errno)<< "line # " <<__LINE__<<std::endl;
            return;
        }
        while(1)
        {
            // Read in data in blocks until EOF. Update the ciphering with each read.
            num_bytes_read = fread(in_buf, sizeof(unsigned char), BUFSIZE, f_input);
            if (ferror(f_input))
            {
                std::cerr<<"ERROR: fread error: "<<strerror(errno)<< "line # " <<__LINE__<<std::endl;
                EVP_CIPHER_CTX_cleanup(ctx);
                return;
            }
            if(!EVP_CipherUpdate(ctx, out_buf, &out_len, in_buf, num_bytes_read))
            {
                std::cerr<<"ERROR: EVP_CipherUpdate failed. OpenSSL error: "<<ERR_error_string(ERR_get_error(), nullptr)<<std::endl;
                EVP_CIPHER_CTX_cleanup(ctx);
                return;
            }
            fwrite(out_buf, sizeof(unsigned char), out_len, ofp);
            if (ferror(ofp)) 
            {
                std::cerr<<"ERROR: fwrite error: "<<strerror(errno)<< "line # " <<__LINE__<<std::endl;
                EVP_CIPHER_CTX_cleanup(ctx);
                return;
            }
            if (num_bytes_read < BUFSIZE) 
            {
                // Reached End of file 
                break;
            }
        }
        fclose(f_input);
        fs::remove((fs::path)file);

        // Now cipher the final block and write it out to file 
        if(!EVP_CipherFinal_ex(ctx, out_buf, &out_len)){
            std::cerr <<"ERROR: EVP_CipherFinal_ex failed. OpenSSL error: "<<ERR_error_string(ERR_get_error(), nullptr)<<std::endl;
            EVP_CIPHER_CTX_cleanup(ctx);
            return;
        }
        
        fwrite(out_buf, sizeof(unsigned char), out_len, ofp);
        std::cout<<"after fwrite"<< "line # " <<__LINE__<<std::endl;
        if (ferror(ofp)) 
        {
            std::cerr<<"ERROR: fwrite error: "<<strerror(errno)<< "line # " <<__LINE__<<std::endl;
            EVP_CIPHER_CTX_cleanup(ctx);
            return;
        }
        
        fclose(ofp);
        ++encrypted_file_number;
        
    }
    EVP_CIPHER_CTX_cleanup(ctx);
}

void encryptDecrypt::decrypt(const byte key[KEY_SIZE], const byte iv[BLOCK_SIZE],fs::path input, fs::path output){

    //Allow enough space in output buffer for additional block
    int cipher_block_size = EVP_CIPHER_block_size(cipher_type);
    unsigned char in_buf[BUFSIZE], out_buf[BUFSIZE + cipher_block_size];

    int num_bytes_read, out_len;
    EVP_CIPHER_CTX *ctx;

    ctx = EVP_CIPHER_CTX_new();

    if(ctx ==nullptr )
    {
        std::cerr<<"ERROR: EVP_CIPHER_CTX_new failed. OpenSSL error: "<<ERR_error_string(ERR_get_error(), nullptr)<<std::endl;
        return;
    }
    EVP_CIPHER_CTX_set_padding(ctx, 0);
    // Don't set key or IV right away; we want to check lengths 
    if(!EVP_CipherInit_ex(ctx, cipher_type, nullptr, nullptr, nullptr, 0))
    {
        std::cerr<<"ERROR: EVP_CipherInit_ex failed. OpenSSL error: "<<ERR_error_string(ERR_get_error(), nullptr)<<std::endl;
        return;
    }

    OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == KEY_SIZE);
    OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx)  == BLOCK_SIZE);

    // Now we can set key and IV
    if(!EVP_CipherInit_ex(ctx, nullptr, nullptr, key, iv, 0))
    {
        std::cerr<<"ERROR: EVP_CipherInit_ex failed. OpenSSL error: "<<ERR_error_string(ERR_get_error(), nullptr)<<std::endl;
        EVP_CIPHER_CTX_cleanup(ctx);
        return;
    }
    int decrypted_file_number = 0;
    std::string decrypted_file = std::string(input)+std::string("/decrypted.txt");
    std::set<std::string> file_list{};
    for (const auto& file : fs::directory_iterator{input})
    {
        file_list.insert(std::string(((fs::path)file)));
    }
    for (const auto& file : file_list) 
    {
        FILE* f_input = fopen(file.c_str(), "rb");
        if (!f_input) 
        {
            std::cerr<<"ERROR: fopen error:"<<strerror(errno)<< "line # " <<__LINE__<<std::endl;
            return;
        }
        decrypted_file += std::to_string(decrypted_file_number);
        FILE* ofp = fopen(decrypted_file.c_str(), "wb");

        if (!ofp) 
        {
            // Unable to open file for writing 
            std::cerr<<"ERROR: fopen error:"<<strerror(errno)<< "line # " <<__LINE__<<std::endl;
            return;
        }

        while(1){
            // Read in data in blocks until EOF. Update the ciphering with each read.
            num_bytes_read = fread(in_buf, sizeof(unsigned char), BUFSIZE, f_input);
            if (ferror(f_input))
            {
                std::cerr<<"ERROR: fread error: "<<strerror(errno)<< "line # " <<__LINE__<<std::endl;
                EVP_CIPHER_CTX_cleanup(ctx);
                return;
            }
            if(!EVP_CipherUpdate(ctx, out_buf, &out_len, in_buf, num_bytes_read))
            {
                std::cerr<<"ERROR: EVP_CipherUpdate failed. OpenSSL error: "<<ERR_error_string(ERR_get_error(), nullptr)<<std::endl;
                EVP_CIPHER_CTX_cleanup(ctx);
                return;
            }
            fwrite(out_buf, sizeof(unsigned char), out_len, ofp);
            if (ferror(ofp)) 
            {
                std::cerr<<"ERROR: fwrite error: "<<strerror(errno)<< "line # " <<__LINE__<<std::endl;
                EVP_CIPHER_CTX_cleanup(ctx);
                return;
            }
            if (num_bytes_read < BUFSIZE) 
            {
                // Reached End of file 
                break;
            }
        }
        fclose(f_input);

        // Now cipher the final block and write it out to file
        if(!EVP_CipherFinal_ex(ctx, out_buf, &out_len))
        {
            std::cerr <<"ERROR: EVP_CipherFinal_ex failed. OpenSSL error: "<< ERR_error_string(ERR_get_error(), nullptr)<<std::endl;
            EVP_CIPHER_CTX_cleanup(ctx);
            return;
        }
        fwrite(out_buf, sizeof(unsigned char), out_len, ofp);
        if (ferror(ofp)) 
        {
            std::cerr<<"ERROR: fwrite error: "<<strerror(errno)<< "line # " <<__LINE__<<std::endl;
            EVP_CIPHER_CTX_cleanup(ctx);
            return;
        }
        fclose(ofp);
        fs::remove(file);
        ++decrypted_file_number;
    }
    EVP_CIPHER_CTX_cleanup(ctx);
    fileSplitMerge::merge(input, output); 
}