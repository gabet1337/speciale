#include <iostream>
#include "point.hpp"

using namespace std;

int main() {

  Point p(10,10);
  Point p2(11,12);
  Point p3(10,10);
  Point p4(9,9);

  cout << p << endl;
  cout << p << "<" << p2 << "? " << (p<p2) << endl;
  cout << p2 << "<" << p << "? " << (p2<p) << endl;
  cout << p << "<" << p << "? " << (p<p) << endl;
  
  return 0;
}
