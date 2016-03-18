#ifndef POINT_HPP
#define POINT_HPP
/*
  This class represents a two-dimensional
  point.
 */
#include <iostream>
class point {
public:
  point(int _x, int _y) : x(_x), y(_y) {}
  point() : x(0), y(0) {}
  ~point() {}
  bool operator<(const point& p) {
    return x < p.x || (x == p.x && y < p.y);
  }
  bool operator==(const point& p) {
    return x == p.x && y == p.y;
  }
  bool operator !=(const point &p) {
    return !(p.x == x && y == p.y);
  }
  friend std::ostream& operator<<(std::ostream& os, const point& p) {
    os << "(" << p.x << "," << p.y << ")";
    return os;
  }
  int x,y;
};

bool operator<(const point &p1, const point &p2) {
  return p1.x < p2.x || (p1.x == p2.x && p1.y < p2.y);
}

bool operator==(const point &p1, const point &p2) {
  return p1.x == p2.x && p1.y == p2.y;
}

bool operator!=(const point &p1, const point &p2) {
  return !(p1 == p2);
}

bool operator<=(const point &p1, const point &p2) {
  return p1 < p2 || p1 == p2;
}

bool operator>=(const point &p1, const point &p2) {
  return p2 < p1 || p1 == p2;
}

bool comp_y(const point &p1, const point &p2) {
  return p1.y < p2.y || (p1.y == p2.y && p1.x < p2.x);
}

bool comp_x(const point &p1, const point &p2) {
  return p1 < p2;
}

#endif
