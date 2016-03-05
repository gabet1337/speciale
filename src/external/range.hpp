#ifndef RANGE_HPP
#define RANGE_HPP
#include "../common/point.hpp"
#include <iostream>
class range {
public:
  point min;
  int max_y, node_id;
  range() {}
  range(point _min, int _max_y, int _node_id) :
    min(_min), max_y(_max_y), node_id(_node_id) {}
  bool operator<(const range &r) {
    return min < r.min;
  }
  friend std::ostream& operator<<(std::ostream& os, const range &r) {
    os << r.min << ", " << r.max_y << ", " << r.node_id;
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