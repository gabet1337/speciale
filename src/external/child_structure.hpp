#ifndef CHILD_STRUCTURE_HPP
#define CHILD_STRUCTURE_HPP

#include "../common/debug.hpp"
#include "../common/point.hpp"
#include "../stream/stream.hpp"
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <error.h>
#include <vector>
#include <set>

namespace ext {

  class child_structure {
  public:
    child_structure(size_t id, size_t buffer_size, double epsilon, std::vector<point> points);
    child_structure(size_t id);
    ~child_structure();
    void insert(point p);
    void remove(point p);
#ifdef VALIDATE
    bool valid_disk();
    bool valid_memory();
#endif
  private:
    std::string get_info_file();
    std::string get_directory();
    std::string get_L_file();
    std::string get_I_file();
    std::string get_D_file();
    void construct(std::vector<point> points);
    void rebuild();
    bool file_exists(std::string file_name);
    template <class InputIterator>
    void flush_container_to_file(InputIterator first, InputIterator last, std::string file_name);
    std::vector<point> L;
    std::set<point> I, D;
    size_t id, buffer_size, L_buffer_size, epsilon, L_size, I_size, D_size;
    const int NUM_VARIABLES = 6;
  };

  child_structure::child_structure(size_t id) {
    this->id = id;

    io::buffered_stream<size_t> info_file(NUM_VARIABLES);
    info_file.open(get_info_file());
    this->id = info_file.read();
    this->buffer_size = info_file.read();
    this->epsilon = info_file.read();
    this->L_size = info_file.read();
    this->I_size = info_file.read();
    this->D_size = info_file.read();
  }

  child_structure::child_structure(size_t id, size_t buffer_size,
				   double epsilon, std::vector<point> points) {

    this->id = id;
    
    //TODO use exceptions
    if (file_exists(get_info_file()))
      error(1, EEXIST, "child structure already exists");
    
    this->buffer_size = buffer_size;
    this->epsilon = (size_t)(1000.0*epsilon);
    this->L_buffer_size = (size_t)pow((double)buffer_size, 1.0+this->epsilon/1000.0);
    DEBUG_MSG("constructing child structure " << id <<
              " with B=" << buffer_size << " - e=" << epsilon << " - L_size=" << L_buffer_size);

    construct(points);
  }

  child_structure::~child_structure() {
    DEBUG_MSG("destructing child structure " << id);
    DEBUG_MSG("flushing variables to " << get_info_file());

    //check if directory exists and open:
    if (!file_exists(get_directory())) mkdir(get_directory().c_str(), 0700);
    io::buffered_stream<size_t> info_file(NUM_VARIABLES);
    info_file.open(get_info_file());

    info_file.write(id);
    info_file.write(buffer_size);
    info_file.write(epsilon);
    info_file.write(L.size());
    info_file.write(I.size());
    info_file.write(D.size());
    
    info_file.close();


    DEBUG_MSG("Flushing L");
    flush_container_to_file(L.begin(), L.end(), get_L_file());

    DEBUG_MSG("Flushing I");
    flush_container_to_file(I.begin(), I.end(), get_I_file());

    DEBUG_MSG("Flushing D");
    flush_container_to_file(D.begin(), D.end(), get_D_file());
    
  }

  template <class InputIterator>
  void child_structure::flush_container_to_file(InputIterator first, InputIterator last, std::string file_name) {
    io::buffered_stream<point> file(buffer_size);
    file.open(file_name);
    while (first != last) {
      file.write(*first);
      first++;
    }
    file.close();
  }

  void child_structure::construct(std::vector<point> points) {
    L = points;
    L_size = points.size();
    // sweep and construct rest of L
  }

  void child_structure::insert(point p) {
    DEBUG_MSG("Insert point " << p);

    DEBUG_MSG(" - deleting from I");
    auto it = I.find(p);
    if (it != I.end()) I.erase(it);

    DEBUG_MSG(" - deleting from D");
    it = D.find(p);
    if (it != D.end()) D.erase(it);

    I.insert(p);

    if (I.size() > buffer_size) rebuild();
  }

  void child_structure::remove(point p) {
    DEBUG_MSG("Remove point " << p);

    DEBUG_MSG(" - deleting from I");
    auto it = I.find(p);
    if (it != I.end()) I.erase(it);

    DEBUG_MSG(" - deleting from D");
    it = D.find(p);
    if (it != D.end()) D.erase(it);

    D.insert(p);

    if (D.size() > buffer_size) rebuild();
  }

  void child_structure::rebuild() {
    //maintain size of L, i.e. L_size
  }
  
  bool child_structure::file_exists(std::string file_name) {
    DEBUG_MSG("Checking if " << file_name << " exists");
    struct stat st;
    return !(stat(file_name.c_str(), &st) == -1);
  }
  
  std::string child_structure::get_directory() {
    return std::string("c_").append(std::to_string(id));
  }

  std::string child_structure::get_info_file() {
      return get_directory()+std::string("/info");
  }

  std::string child_structure::get_L_file() {
    return get_directory()+std::string("/L");
  }

  std::string child_structure::get_I_file() {
    return get_directory()+std::string("/I");
  }

  std::string child_structure::get_D_file() {
    return get_directory()+std::string("/D");
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
    if (!std::is_sorted(L.begin(), L.begin()+L_size)) {
      DEBUG_MSG("L is not sorted w.r.t x");
      return false;
    }
    // TODO: Check I_size, D_size, L_size is correct w.r.t. files.
    
    return true;
  }
#endif

};

#endif
