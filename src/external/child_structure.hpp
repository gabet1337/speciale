#ifndef CHILD_STRUCTURE_HPP
#define CHILD_STRUCTURE_HPP

#include "../common/debug.hpp"
#include "../stream/stream.hpp"
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace ext {

  class child_structure {
  public:
    child_structure(size_t id, size_t buffer_size, double epsilon);
    child_structure(size_t id);
    ~child_structure();
#ifdef VALIDATE
    bool valid_disk();
    bool valid_memory();
#endif
  private:
    std::string get_info_file();
    std::string get_directory();
    size_t id;
    size_t buffer_size;
    size_t L_size;
    size_t epsilon;
    const int NUM_VARIABLES = 4;
  };

  child_structure::child_structure(size_t id) {
    
  }

  child_structure::child_structure(size_t id, size_t buffer_size, double epsilon) {
    //TODO check if exists first and error 
    this->id = id;
    this->buffer_size = buffer_size;
    this->epsilon = (size_t)(1000.0*epsilon);
    this->L_size = (size_t)pow((double)buffer_size, 1.0+this->epsilon/1000.0);
    DEBUG_MSG("constructing child structure " << id <<
              " with B=" << buffer_size << " - e=" << epsilon << " - L_size=" << L_size);
  }

  child_structure::~child_structure() {
    DEBUG_MSG("destructing child structure " << id);
    DEBUG_MSG("flushing variables to " << get_info_file());
    //check if directory exists and open:
    struct stat st;
    if (stat(get_directory().c_str(), &st) == -1) mkdir(get_directory().c_str(), 0700);
    io::buffered_stream<size_t> info_file(NUM_VARIABLES);
    info_file.open(get_info_file());
    
    

    info_file.close();
  }

  std::string child_structure::get_directory() {
    return std::string("c_").append(std::to_string(id));
  }

  std::string child_structure::get_info_file() {
      return get_directory()+std::string("/info");
  }

#ifdef VALIDATE
  bool child_structure::valid_disk() {
    io::buffered_stream<size_t> info_file(NUM_VARIABLES);
    info_file.open(get_info_file());
    if (NUM_VARIABLES*sizeof(size_t) != info_file.size()) {
      DEBUG_MSG("NUM_VARIABLES != info file size " << NUM_VARIABLES*sizeof(size_t) << " != " << info_file.size());
      return false;
    }
    info_file.close();
    return true;
  }

  bool child_structure::valid_memory() {

    return true;
  }
#endif

};

#endif
