#ifndef BOOST_R_TREE_HPP
#define BOOST_R_TREE_HPP
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/function_output_iterator.hpp>
#include "../common/pst_interface.hpp"
#include "../common/definitions.hpp"
#include "../common/point.hpp"
#include <string>
#include "../stream/stream.hpp"

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef bg::model::point<int, 2, bg::cs::cartesian> pt;

namespace internal {

  class boost_r_tree : public common::pst_interface {
  public:
    boost_r_tree();
    boost_r_tree(size_t buffer_size, double epsilon);
    ~boost_r_tree();
    void insert(const point &p);
    void remove(const point &p);
    void report(int x1, int x2, int y, const std::string &output_file);
    size_t size();
    void print();
  private:
    bgi::rtree<pt, bgi::quadratic<16> > rtree;
    unsigned next_id;
    size_t buffer_size;
  };

  boost_r_tree::boost_r_tree() {
    next_id = 0;
    buffer_size = 1024;
  }

  boost_r_tree::boost_r_tree(size_t buffer_size, double epsilon) {
    next_id = 0;
    this->buffer_size = buffer_size;
  }

  boost_r_tree::~boost_r_tree() {
    rtree.clear();
  }

  void boost_r_tree::insert(const point &p) {
    rtree.insert(pt(p.x,p.y));
  }

  void boost_r_tree::remove(const point &p) {
    rtree.remove(pt(p.x,p.y));
  }

  size_t boost_r_tree::size() {
    return rtree.size();
  }

  void boost_r_tree::report(int x1, int x2, int y, const std::string &output_file) {
    io::buffered_stream<point> output(STREAM_BUFFER_SIZE);
    output.open(output_file);
    rtree.query(bgi::satisfies([&](pt const& v) {
          int xp = v.get<0>();
          int yp = v.get<1>();
          return x1 <= xp && xp <= x2 && yp >= y;
        }),
      boost::make_function_output_iterator([&output](pt const& val) {
          output.write(point(val.get<0>(), val.get<1>()));
        })
      );
    output.close();
  }

  void boost_r_tree::print() {
  }
  
};

#endif
