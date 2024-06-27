#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <utility>
#include <set>

#include "file_split_merge.h"
#include "file.h"
#include "customexceptions.h"

namespace fs            = std::filesystem;
const auto copy_options = fs::copy_options::recursive|fs::copy_options::update_existing;
auto permissions        = fs::perms::owner_all|fs::perms::group_all;

void fileSplitMerge::split(size_t splitNumber,std::string& inputfilePath, std::string& outputPath ) {
  std::string file_to_string = pathStringHandler::filePathToString(inputfilePath);
  size_t buffer_size = (file_to_string.length()/splitNumber)+1;
  std::ifstream ifs(inputfilePath, std::ios::in | std::ios::binary);

  char buf[buffer_size];
  size_t file_number = 0;

  fs::create_directory("tmp");
  fs::permissions("tmp", permissions);
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

    fs::copy(splitted_component, "tmp", copy_options);
    fs::remove_all(splitted_component);
    ++file_number;

  }
  fs::copy("tmp", outputPath, copy_options);
  fs::remove_all("tmp");
}

void fileSplitMerge::merge(std::string& filesPath,
                           std::string& outPutFilePath) {
  std::cout << "merge called:  " << std::endl;
  fs::create_directory("tmp");
  fs::permissions("tmp", permissions);
  fs::copy(filesPath, "tmp", copy_options);

  fs::path encrypted_files_path = fs::path("tmp");
  std::ofstream ofile((outPutFilePath+"/retrieved.txt"s), std::ofstream::out | std::ofstream::app);
  std::set<std::string> file_list{};
  ofile.unsetf(std::ios_base::skipws);
  for (const auto& file : fs::directory_iterator{encrypted_files_path}) {
   file_list.insert(std::string(((fs::path)file)));
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
    str +="\n";
    ofile<<std::move(str);
    str.clear();
    }  
    fs::remove_all(file);
  }
  ofile.close();std::flush(ofile);
  fs::remove_all(fs::path("tmp"));
}
