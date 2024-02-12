#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <utility>
#include <set>

#include "file_split_merge.h"
#include "customexceptions.h"

const auto copy_options_all   = fs::copy_options::recursive|fs::copy_options::update_existing|fs::copy_options::overwrite_existing;
const auto permissions_all    = fs::perms::owner_all|fs::perms::group_all;

void fileSplitMerge::split(size_t splitNumber,const fs::path& inputfilePath, fs::path& outputPath ) 
{
  //TO DO : use file instead of string, as string for large file could be huge in memory
  std::string file_to_string = pathStringHandler::filePathToString(std::string(inputfilePath));
  size_t buffer_size = (file_to_string.length()/splitNumber);
  size_t modulus = file_to_string.length() % splitNumber;

  size_t file_number = 0;
  std::string chunk{};
  fs::create_directory("tmp");
  fs::permissions("tmp", permissions_all);
  std::cout<<"before while in split"<< "line # " <<__LINE__<<std::endl;
  auto it = file_to_string.begin();

  for(size_t i = 0; i < splitNumber; i++)
  {
    // use what was read in.
    std::copy_n(it, buffer_size, std::back_inserter(chunk));
    auto file_name = std::string(fs::path(inputfilePath).filename());
    std::string splitted_component = file_name+std::to_string(file_number);
    {
      //Create scope with {} to use RAII of std::ofstream to close file
      std::ofstream(splitted_component)<< chunk;
    }
    fs::copy(splitted_component.c_str(), "tmp", copy_options_all);
    fs::remove(splitted_component);
    ++file_number;
    it += buffer_size+1;
    chunk.clear();
  }
  std::cout<<"file number is : "<< file_number << std::endl;
  std::copy_n(it,modulus, std::back_inserter(chunk));
  auto file_name = std::string(fs::path(inputfilePath).filename());
  std::string splitted_component = file_name+std::to_string(file_number);
  {
    //Create scope with {} to use RAII of std::ofstream to close file
    std::ofstream(splitted_component)<< chunk;
  }
  fs::copy(splitted_component, "tmp", copy_options_all);
  fs::remove(splitted_component);
  fs::copy("tmp", std::string(outputPath).c_str(), copy_options_all);
  fs::remove_all(std::string("tmp"));
}

void fileSplitMerge::merge(fs::path & filesPath,fs::path & outPutFilePath) 
{
  fs::remove_all(std::string("tmp"));
  fs::create_directory("tmp");
  fs::permissions("tmp", permissions_all);
  //TO DO : copy from network address
  fs::copy(filesPath, "tmp", copy_options_all);
  fs::path encrypted_files_path = fs::path(std::string("tmp"));
  std::ofstream ofile((std::string(outPutFilePath)+std::string("/decrypted.txt")), std::ofstream::out | std::ofstream::app);
 
  std::cout<<(std::string(outPutFilePath)+std::string("/decrypted.txt"))<<std::endl;
  std::set<std::string> file_list{};
  ofile.unsetf(std::ios_base::skipws);
  for (const auto& file : fs::directory_iterator{encrypted_files_path}) {
   file_list.insert(std::string((fs::path)file));
  }
  for(const auto& file:file_list)
  {
    std::cout<<std::string(((fs::path)file))<<std::endl;
    std::ifstream ifile(std::string((fs::path)file), std::ios::in | std::ios::binary);
    if (!ifile.is_open())
    {
      throw invalid_file("File opening error: ");
      break;
    }
    std::string str{};
    while(std::getline (ifile,str))
    {
    str += "\n";
    //std::cout<<"read from file: "<<str<<"\n";
    ofile<<str;
    str.clear();
    }
    ifile.close();
    std::cout<<"removing file: "<<std::string(((fs::path)file))<<std::endl;
    fs::remove(file);
  }
  ofile.close();std::flush(ofile);

  fs::remove_all(std::string("tmp"));
}
