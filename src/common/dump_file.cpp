#include "../stream/stream.hpp"
#include "point.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

int main(int argc, char *argv[]) {

  io::buffered_stream<point> bs(4096);
  bs.open(string(argv[1]));
  vector<point> points;
  while (!bs.eof()) points.push_back(bs.read());

  //sort(points.begin(), points.end());

  cout << "SIZE: " << points.size() << endl;
  for (int i = 1; i <= points.size(); i++)
    cout << i << " - " << points[i-1] << endl;

  return 0;
}
