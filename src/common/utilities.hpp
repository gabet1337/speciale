#ifndef UTILITIES_HPP
#define UTILITIES_HPP
#include "../stream/stream.hpp"
#include "point.hpp"
#include <string>
#include <fstream>
#include <ios>
#include <iterator>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>

namespace util {

  bool file_exists(const std::string &file_name) {
    struct stat st;
    return stat(file_name.c_str(),&st) == 0;
  }

  void remove_file(const std::string &file_name) {
    int s = system(std::string("rm -rf ").append(file_name).c_str());
    if (s != 0) {
      perror(std::string("could not remove file: ").append(file_name).c_str());
      exit(errno);
    }
  }

  void remove_directory(const std::string &directory_name) {
    remove_file(directory_name);
  }

  unsigned long long get_used_memory() {
    using std::ios_base;
    using std::ifstream;
    using std::string;

    ifstream stat_stream("/proc/self/stat",ios_base::in);

    string temp;

    unsigned long long rss;
    stat_stream >> temp >> temp >> temp >> temp >> temp >> temp >> temp
                >> temp >> temp >> temp >> temp >> temp >> temp
                >> temp >> temp >> temp >> temp >> temp >> temp
                >> temp >> temp >> temp >> temp >> rss; // don't care about the rest

    stat_stream.close();
    long page_size = sysconf(_SC_PAGE_SIZE);
    return rss * page_size;
  }

  template <class InputIterator, typename T>
  void flush_container_to_file
  (InputIterator first, InputIterator last, const std::string &file_name, size_t buffer_size) {
    if (file_exists(file_name)) remove_file(file_name);
    io::buffered_stream<T> file(STREAM_BUFFER_SIZE);
    file.open(file_name);
    //file.truncate();
    while (first != last) {
      file.write(*first);
      first++;
    }
    file.close();
  }

  template <class Container, typename T>
  void load_file_to_container(Container &c, const std::string &file_name, size_t buffer_size) {
    io::buffered_stream<T> file(STREAM_BUFFER_SIZE);
    file.open(file_name);
    //DEBUG_MSG("file_size: " << file.size() / sizeof(T));
    while (!file.eof()) {
      c.insert(c.end(), file.read());
    }
    file.close();
  }

  inline bool in_range(const point &p, int x1, int x2, int y) {
    return x1 <= p.x && p.x <= x2 && p.y >= y;
  }

};

#endif
