#include "priority_search_tree.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <assert.h>
using namespace std;

void test1() {
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
  // pst.print();
  vector<Point> r = pst.report(0, 20, 4);
  // for (auto p : r) {
  //   cout << p << endl;
  // }
  sort(r.begin(), r.end());
  vector<Point> true_res;
  true_res.push_back(Point(16,20));
  true_res.push_back(Point(5,6));
  true_res.push_back(Point(9,4));
  true_res.push_back(Point(19,9));
  sort(true_res.begin(), true_res.end());
  assert(equal(r.begin(),r.end(), true_res.begin(),
	       [] (const Point& p1, const Point& p2) -> bool {
		 return p1.x == p2.x && p1.y == p2.y;
	       }) && "Query does not return correct result");
}

void test2() {
  vector<Point> points;
  for (int i = 1; i <= 10; i++)
    for (int j = 1; j <= 10; j++)
      points.push_back(Point(i,j));

  PrioritySearchTree pst(points);
  
  // pst.print();
  // vector<Point> res = pst.report(1,10,5);
  assert(pst.report(1,10,6).size() == 50 && "Incorrect size of the result for query");
  // for (auto p : pst.report(1,10,5))
  //   cout << p << endl;
  
}

int main() {
  test1();
  test2();

  cout << "\x1b[32mALL TESTS WERE SUCCESSFUL!\x1b[0m" << endl;

  return 0;
}
