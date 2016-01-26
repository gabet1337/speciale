#include "priority_search_tree.hpp"
#include <iostream>
#include <vector>

using namespace std;

int main() {
  
  vector<Point> points;

  points.push_back(Point(1,2));
  points.push_back(Point(4,1));
  points.push_back(Point(5,6));
  points.push_back(Point(9,4));
  points.push_back(Point(13,3));
  points.push_back(Point(16,20));
  points.push_back(Point(19,9));
  points.push_back(Point(20,3));

  PrioritySearchTree pst(points);
  pst.print();


  return 0;
}
