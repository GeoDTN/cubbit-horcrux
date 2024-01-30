#include <iostream>
#include <cstring>

#include "file.h"
#include "file_split_merge.h"
#include "customexceptions.h"

int main(int argc, char* argv[]) {
  std::string encrpt_decrypt_argument1 = std::string(argv[1]); 
  std::string encrpt_decrypt_argument2 = std::string(argv[2]);

  try {
    if (encrpt_decrypt_argument1.compare("create") == 0 &&
        encrpt_decrypt_argument2.compare("-n") == 0) {
    if (argc < 5) {
      std::cerr << "Inavlid command line arguments. Please use correct arguments"
              << std::endl;
      exit(EXIT_FAILURE);
    }
      size_t horcrux_count = static_cast<size_t>(std::stoi(argv[3]));
      std::string input_path  = std::string(argv[4]);
      std::string output_path = std::string(argv[5]);
     /* if(!std::filesystem::exists(input_path)){
    std::cerr << "Input path does not exist"<< std::endl;
    exit(EXIT_FAILURE);
  }

  if(!std::filesystem::exists(output_path)){
    std::cerr << "Output path does not exist"<< std::endl;
    exit(EXIT_FAILURE);
  }*/
 
      fileSplitMerge::split(horcrux_count, input_path, output_path);


    } else if (encrpt_decrypt_argument1.compare("load") == 0 ) {
      
     if (argc < 4) {
        std::cerr << "Inavlid command line arguments. Please use correct arguments"
              << std::endl;
        exit(EXIT_FAILURE);
      }
     std::string input_path  = std::string(argv[2]);
     std::string output_path = std::string(argv[3]);
     fileSplitMerge::merge(input_path, output_path);

    } else {
      std::cerr<<"Inavlid command line arguments. Please use correct arguments"<<'\n';
      exit(EXIT_FAILURE);
    }

  }
  catch (const std::exception& e) {
    std::cerr<<"Exception:"<<e.what ()<<"  thrown"<<std::endl;
  }
  catch (...) {
    std::cerr<<"Unexpected exception thrown"<<std::endl;
  }
  return 0;
}

