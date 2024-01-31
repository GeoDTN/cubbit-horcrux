#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <utility>
#include <set>

#include "file_split_merge.h"
#include "file.h"
#include "customexceptions.h"

void fileSplitMerge::split(size_t splitNumber,std::string& inputfilePath, std::string& outputPath ) {
  namespace fs = std::filesystem;
  std::string file_to_string = pathStringHandler::filePathToString(inputfilePath);
  size_t buffer_size = (file_to_string.length()/splitNumber)+1;
  std::ifstream ifs{};
  ifs.open(inputfilePath, std::ios::in | std::ios::binary);

  char buf[buffer_size];
  size_t file_number = 0;
  system("sudo mkdir tmp && sudo chmod -R 777 tmp");
  while (ifs.read(buf, (buffer_size-1)) || ifs.gcount()) 
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

    system(std::string("sudo cp "+splitted_component+" tmp/" ).c_str());
    system(std::string("sudo rm "+splitted_component ).c_str());
    ++file_number;

  }
  system((std::string("sudo cp -r tmp/*  ")+outputPath).c_str());
  system("sudo rm -rf tmp");
}

void fileSplitMerge::merge(std::string& filesPath,
                           std::string& outPutFilePath) {
  std::cout << "merge called:  " << std::endl;
  namespace fs = std::filesystem;
  system("sudo rm -rf tmp && sudo mkdir tmp && sudo chmod -R 777 tmp");
  system((std::string("scp -r ")+filesPath+std::string("/*  tmp")).c_str());
  fs::path encrypted_files_path = fs::path(std::string("tmp"));
  std::ofstream ofile{};
  std::cout<<(outPutFilePath+std::string("/retrieved.txt"))<<std::endl;
  ofile.open ((outPutFilePath+std::string("/retrieved.txt")), std::ofstream::out | std::ofstream::app);
  std::set<std::string> file_list{};
  ofile.unsetf(std::ios_base::skipws);
  for (const auto& file : fs::directory_iterator{encrypted_files_path}) {
   file_list.insert(std::string(((fs::path)file)));
  }
  for(const auto& file:file_list)
  {
    std::cout<<std::string(((fs::path)file))<<std::endl;
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
    str +="\n";
    //std::cout<<"read from file: "<<str<<"\n";
    ofile<<std::move(str);
    str.clear();
    }  
    std::filesystem::remove(file);
  }
  ofile.close();std::flush(ofile);
  system("sudo rm -rf tmp");
  std::cout<<"safely reached end of merge"<<std::endl;
}
