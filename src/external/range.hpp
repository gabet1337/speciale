#ifndef RANGE_HPP
#define RANGE_HPP
#include "../common/point.hpp"
#include <iostream>
class range {
public:
  point min, min_y, max_y;
  int node_id;
  range() {}
  ~range() {}
  range(point _min, point _min_y, point _max_y, int _node_id) :
    min(_min), min_y(_min_y), max_y(_max_y), node_id(_node_id) {}

  bool operator<(const range &r) {
    return min < r.min;
  }
  
  friend std::ostream& operator<<(std::ostream& os, const range &r) {
    os << "minx: " << r.min << " "
       << "miny: " << r.min_y << std::endl
       << "maxy: " << r.max_y << " "
       << "id: " << r.node_id;
    return os;
  }
};

bool operator<(const range &r1, const range &r2) {
  return r1.min < r2.min;
}

bool operator==(const range &r1, const range &r2) {
  return r1.min == r2.min;
}

bool operator!=(const range &r1, const range &r2) {
  return !(r1.min == r2.min);
}
#endif
