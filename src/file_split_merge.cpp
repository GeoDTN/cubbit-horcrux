#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <set>

#include "file_split_merge.h"
#include "file.h"
#include "customexceptions.h"

void fileSplitMerge::split(size_t splitNumber,std::string& inputfilePath, std::string& outputPath ) {
  namespace fs = std::filesystem;
  secure_string file_to_string = pathStringHandler::filePathToString(inputfilePath);
  size_t buffer_size = (file_to_string.length()/splitNumber)+1;
  std::ifstream ifs(inputfilePath, std::ios::in | std::ios::binary);

  char buf[buffer_size];
  size_t file_number = 0;
  system("mkdir tmp");
  while (ifs.read(buf, sizeof(buf)) || ifs.gcount()) 
  {
    // use what was read in.

    // gcount() is the number of bytes actually read
    std::string chunk(buf, ifs.gcount());
    auto file_name = std::string(fs::path(inputfilePath).filename());
    std::string splitted_component=file_name+std::to_string(file_number);
    {
      //Create scope with {} to use RAII of std::ofstream to close file
      std::ofstream(splitted_component)<< chunk << '\n';
    }

    //fs::copy(fs::path(split_component),fs::path(outputPath),fs::copy_options::overwrite_existing);
    fs::copy(fs::path(splitted_component),fs::path(std::string_view("tmp/")),fs::copy_options::overwrite_existing);
    std::filesystem::remove(splitted_component);
   ++file_number;

  }
  system((std::string("scp -r tmp/* ")+outputPath).c_str());
  system("sudo rm -rf tmp");
}

void fileSplitMerge::merge(std::string& filesPath,
                           std::string& outPutFilePath) {
    std::cout << "merge called:  " << std::endl;
  namespace fs = std::filesystem;
  std::set<fs::path> files_to_merge;
  system("mkdir tmp");
  system((std::string("scp -r ")+filesPath+std::string("/*  tmp")).c_str());
  fs::path encrypted_files_path = fs::path(std::string("tmp"));
  std::ofstream ofile;
  std::cout<<(outPutFilePath+std::string("/retrieved.txt"))<<std::endl;
  ofile.open ((outPutFilePath+std::string("/retrieved.txt")), std::ofstream::out | std::ofstream::app);
 
  ofile.unsetf(std::ios_base::skipws);
  for (const auto& file : fs::directory_iterator{encrypted_files_path}) {
   std::cout<<std::string(((fs::path)file));
    //files_to_merge.insert((fs::path)file);
  //} 
  //std::ofstream ofile(outPutFilePath+std::string("/retrieved.txt"), std::ios::out | std::ios::binary|std::ios::app);
  //std::string str;
  //ofile.unsetf(std::ios_base::skipws);
  //for (auto& file : files_to_merge) 
  //{
    std::ifstream ifile{};//((fs::path)file)/*, std::ios::in | std::ios::binary)*/;
    ifile.open(std::string((fs::path)file), std::ios::in | std::ios::binary);
    if (!ifile.is_open())
    {
      throw invalid_file("File opening error: ");
      break;
    }
    std::string str{};
    while(std::getline (ifile,str))
    {
    std::cout<<str<<"\n";
    str +="\n";
    ofile<<str;
  
    }
   
    //std::filesystem::remove(std::string(file));
  }

  //std::flush(ofile);
  system("sudo rm -rf tmp");

}
