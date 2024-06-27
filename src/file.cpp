#include "file.h"

std::string pathStringHandler::filePathToString(std::string &filePath)
{
/*   std::ifstream t(filePath.c_str());
  secure_string  str;
  t.seekg(0, std::ios::end);
  str.reserve(t.tellg());
  t.seekg(0, std::ios::beg);
  str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

  return str; */
  std::ifstream t(filePath.c_str());
  std::stringstream buffer;
  buffer << t.rdbuf();
  return buffer.str();
}

void pathStringHandler::stringToFile(secure_string& fileContent,std::string& filePath) 
{  // better passing non primitive types by reference than by value
  std::ofstream file(filePath);
  file << fileContent<<std::endl;
}

