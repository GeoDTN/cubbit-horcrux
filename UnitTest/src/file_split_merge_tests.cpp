#include <fstream>
#include <iterator>
#include <string>
#include <algorithm>

#include "file_split_merge_tests.h"

bool compareFiles(const std::string& p1, const std::string& p2) {
  if(fs::file_size(p1) != fs::file_size(p2)) 
  return false;
  std::ifstream f1(p1, std::ifstream::binary | std::ifstream::ate);
  std::ifstream f2(p2, std::ifstream::binary | std::ifstream::ate);

  if (f1.fail() || f2.fail()) {
    return false;  // file problem
  }

  if (f1.tellg() != f2.tellg()) {
    return false;  // size mismatch
  }

  // seek back to beginning and use std::equal to compare contents
  f1.seekg(0, std::ifstream::beg);
  f2.seekg(0, std::ifstream::beg);
  return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
                    std::istreambuf_iterator<char>(),
                    std::istreambuf_iterator<char>(f2.rdbuf()));
}

TEST(fileSplitMerge_Tests, Test_split_merge_positive) 
{
  size_t horcrux_count       = 3;
  fs::path inputfilePath     = "source.txt";
  fs::path  outputPath       = "split";
  fs::path  mergeOutputPath  = "merged.txt";

  fileSplitMerge::split(horcrux_count, inputfilePath,outputPath );
  fileSplitMerge::merge(outputPath,mergeOutputPath);
  EXPECT_EQ(true,compareFiles(inputfilePath,mergeOutputPath));

}

/*
TEST(fileSplitMerge_Tests, Test_split_merge_negative)
{

}
*/

