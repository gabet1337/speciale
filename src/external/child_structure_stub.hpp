#ifndef CHILD_STRUCTURE_STUB_HPP
#define CHILD_STRUCTURE_STUB_HPP
#include <vector>
#include "../common/point.hpp"
#include "../common/utilities.hpp"
#include "../common/debug.hpp"
#include "../stream/stream.hpp"
#include "child_structure_interface.hpp"
#include <string>
#include <algorithm>
#include <set>

namespace ext {

  class child_structure_stub : public child_structure_interface {
  public:
    child_structure_stub(std::size_t id, std::size_t buffer_size,
                         double epsilon, std::vector<point> points);
    child_structure_stub(std::size_t id);
    ~child_structure_stub();
    void insert(const point &p);
    void remove(const point &p);
    std::vector<point> report(int x1, int x2, int y);
    void destroy() {};
#ifdef DEBUG
    bool valid_memory() { return true; }
    bool valid_disk() { return true; }
    std::vector<point> get_points()
    {return std::vector<point>(points.begin(), points.end());}
#endif
  private:
    std::string get_directory();
    std::string get_L_file();
    inline bool in_range(const point &p, int x1, int x2, int y);
    std::set<point> points;
    size_t id;
    size_t buffer_size;
  };

  child_structure_stub::child_structure_stub
  (std::size_t id, std::size_t buffer_size, double epsilon, std::vector<point> _points)
    : points(_points.begin(), _points.end()) {
    this->id = id;
    this->buffer_size = 1024;
  }
  
  child_structure_stub::child_structure_stub(std::size_t id) {
    this->id = id;
    this->buffer_size = 1024;
    //load in from file
    DEBUG_MSG("Loading L");
    util::load_file_to_container<std::set<point>, point>
      (points, get_L_file(), buffer_size);
  }

  child_structure_stub::~child_structure_stub() {
    if (!util::file_exists(get_directory())) mkdir(get_directory().c_str(), 0700);
    DEBUG_MSG("Flushing L");
    util::flush_container_to_file<std::set<point>::iterator,point>
      (points.begin(), points.end(), get_L_file(), buffer_size);
  }

  void child_structure_stub::insert(const point &p) {
    points.insert(p);
  }

  void child_structure_stub::remove(const point &p) {
    points.erase(p);
  }

  std::vector<point> child_structure_stub::report(int x1, int x2, int y) {
    std::vector<point> result;
    for (point p : points)
      if (in_range(p, x1, x2, y)) result.push_back(p);
    return result;
  }

  std::string child_structure_stub::get_directory() {
    return std::string("c_").append(std::to_string(id));
  }

  std::string child_structure_stub::get_L_file() {
    return get_directory()+std::string("/L");
  }

  inline bool child_structure_stub::in_range(const point &p, int x1, int x2, int y) {
    return x1 <= p.x && p.x <= x2 && p.y >= y;
  }


};


#endif
