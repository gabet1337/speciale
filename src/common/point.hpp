#ifndef POINT_HPP
#define POINT_HPP
/*
  This class represents a two-dimensional
  point.
 */
#include <iostream>
class Point {
public:
  Point(int _x, int _y) : x(_x), y(_y) {}
  Point() : x(0), y(0) {}
  ~Point() {}
  bool operator<(const Point& p) {
    return x < p.x || (x == p.x && y < p.y);
  }
  friend std::ostream& operator<<(std::ostream& os, const Point& p) {
    os << "(" << p.x << "," << p.y << ")";
    return os;
  }
  int x,y;
};

bool operator<(const Point &p1, const Point &p2) {
  return p1.x < p2.x || (p1.x == p2.x && p1.y < p2.y);
}

#endif
