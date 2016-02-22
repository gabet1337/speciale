#ifndef UTILITIES_HPP
#define UTILITIES_HPP
#include "../stream/stream.hpp"
#include <string>
#include <iterator>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

namespace util {

  template <class InputIterator, typename T>
  void flush_container_to_file
  (InputIterator first, InputIterator last, std::string file_name, size_t buffer_size) {
    io::buffered_stream<T> file(buffer_size);
    file.open(file_name);
    while (first != last) {
      file.write(*first);
      first++;
    }
    file.close();
  }

  template <class Container, typename T>
  void load_file_to_container(Container &c, std::string file_name, size_t buffer_size) {
    io::buffered_stream<T> file(buffer_size);
    file.open(file_name);
    DEBUG_MSG("file_size: " << file.size() / sizeof(T));
    while (!file.eof()) {
      c.insert(c.end(), file.read());
    }
    file.close();
  }

  bool file_exists(std::string file_name) {
    struct stat st;
    return stat(file_name.c_str(),&st) == 0;
  }


};

#endif
