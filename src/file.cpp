#include "file.h"

std::string pathStringHandler::filePathToString(const std::string &filePath)
{
  std::ifstream t(filePath.c_str());
  std::stringstream buffer;
  buffer << t.rdbuf();
  return buffer.str();
}

void pathStringHandler::stringToFile(const secure_string& fileContent,std::string& filePath) 
{  // Better passing non basic types by reference than by value
  std::ofstream file(filePath);
  file << fileContent<<std::endl;
}

