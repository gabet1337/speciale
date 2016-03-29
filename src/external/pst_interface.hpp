#ifndef PST_INTERFACE_HPP
#define PST_INTERFACE_HPP
#include <string>
#include "../common/point.hpp"
namespace ext {

  class pst_interface {
  public:
    pst_interface() {};
    pst_interface(size_t buffer_size, double epsilon) {};
    virtual ~pst_interface() {};
    virtual void insert(const point &p) = 0;
    virtual void remove(const point &p) = 0;
    virtual void report(int x1, int x2, int y, const std::string &output_file) = 0;
    virtual void print() = 0;
  };

};

#endif
