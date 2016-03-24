#ifndef CHILD_STRUCTURE_INTERFACE_HPP
#define CHILD_STRUCTURE_INTERFACE_HPP
#include <vector>
#include "../common/point.hpp"
#include "../common/debug.hpp"
#include <cstddef>
namespace ext {

  class child_structure_interface {
  public:
    child_structure_interface() {};
    child_structure_interface(std::size_t id, std::size_t buffer_size,
                              double epsilon, std::vector<point> points) {};
    child_structure_interface(std::size_t id) {};
    virtual ~child_structure_interface() {};
    virtual void insert(const point &p) = 0;
    virtual void remove(const point &p) = 0;
    virtual std::vector<point> report(int x1, int x2, int y) = 0;
    virtual void destroy() = 0;
    virtual std::vector<point> get_points() = 0;
  };

};

#endif
