#ifndef ENC_DEC_H_
#define ENC_DEC_H_

#include "file.h"

class encryptDecrypt {
 public:
  static void gen_params(byte key[KEY_SIZE]);
  static void encrypt(const byte key[KEY_SIZE], const byte iv[BLOCK_SIZE], size_t horcrux_count,fs::path, fs::path);
  static void decrypt(const byte key[KEY_SIZE], const byte iv[BLOCK_SIZE], fs::path input, fs::path output);
  
};

#endif



