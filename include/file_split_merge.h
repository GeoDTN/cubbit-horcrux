#ifndef FILE_SPLIT_MERGE_H
#define FILE_SPLIT_MERGE_H

#include <string>
#include "file.h"


class fileSplitMerge {
 public:

  static void split(size_t splitNumber,const fs::path& inputPath, fs::path& outPutPath );
  static void merge(fs::path& filesPath,fs::path& outPutFilePath);
 private:

  fileSplitMerge(fileSplitMerge const&) = delete;
  fileSplitMerge(fileSplitMerge&&) = delete;
  fileSplitMerge& operator=(fileSplitMerge const&) = delete;
  fileSplitMerge& operator=(fileSplitMerge&&) = delete;

};

#endif //FILE_SPLIT_MERGE_H

