#ifndef CUSTOMEXCEPTIONS_H_
#define CUSTOMEXCEPTIONS_H_

#include <stdexcept>


class invalid_file: public std::runtime_error {
 public:
     /*std::string is copy constructible but itself could throw std::bad_alloc.*/
  explicit invalid_file(const std::string& arg) : std::runtime_error{arg} {}
    /*char * is nothrow copy-constructable. This means that the exception itself can be copied without the copy generating an exception.*/
  explicit invalid_file(const char *arg) : std::runtime_error{arg} {}

  virtual ~invalid_file() {}
};

#endif /* CUSTOMEXCEPTIONS_H_ */
