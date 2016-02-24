#ifndef UTILITIES_HPP
#define UTILITIES_HPP
#include "../stream/stream.hpp"
#include <string>
#include <iterator>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>

namespace util {

  bool file_exists(std::string file_name) {
    struct stat st;
    return stat(file_name.c_str(),&st) == 0;
  }

  void remove_file(std::string file_name) {
    int s = system(std::string("rm -rf ").append(file_name).c_str());
    if (s != 0) {
      perror(std::string("could not remove file: ").append(file_name).c_str());
      exit(errno);
    }
  }

  void remove_directory(std::string directory_name) {
    remove_file(directory_name);
  }

  template <class InputIterator, typename T>
  void flush_container_to_file
  (InputIterator first, InputIterator last, std::string file_name, size_t buffer_size) {
    if (file_exists(file_name)) remove_file(file_name);
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

};

#endif
