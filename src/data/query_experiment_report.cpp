#include "../common/test_lib.hpp"
#include "../common/definitions.hpp"
#include "../common/point.hpp"
#include "../common/utilities.hpp"
#include "../stream/stream.hpp"
#include <vector>
#include <iostream>
#include <set>

using namespace std;

typedef pair<pair<int,int>, pair<int,int> > iiii;

iiii make_query_range(int x1, int x2, int y1, int y2) {
  return make_pair(make_pair(x1, x2), make_pair(y1, y2));
}

void fill_points_inside_ranges(vector<iiii> &ranges,
                               io::buffered_stream<point> &os,
                               unsigned long long num_data) {

  test::random rand;
  
  for (auto r : ranges) {

    set<point> result;
    
    int query_x1 = r.first.first;
    int query_x2 = r.first.second;
    int query_y1 = r.second.first;
    int query_y2 = r.second.second;

    while (result.size() < (num_data / sizeof(point))) {
      int x = rand.next(query_x2 - query_x1) + query_x1;
      int y = rand.next(query_y2 - query_y1) + query_y1;
      result.insert(point(x,y));
    }

    for (point p : result) {
      os.write(p);
    }

    cout << "just wrote " << result.size() << " points" << endl;
    
  }
  
}

void fill_points_outside_ranges(vector<iiii> &ranges,
                               io::buffered_stream<point> &os,
                               unsigned long long num_data) {

  test::random rand;

  for (unsigned long long i = 0; i < num_data / sizeof(point); ) {

    point p(rand.next(INF), rand.next(INF));

    bool insert = true;
    
    for (auto r : ranges) {

      int query_x1 = r.first.first;
      int query_x2 = r.first.second;
      int query_y = r.second.first;

      if (util::in_range(p, query_x1, query_x2, query_y))
        insert = false;
    }

    if (insert) {
      os.write(p);
      i++;
    }
    
  }
  
}

void generate_data_50mb() {

  vector<iiii> reserved_query_windows;

  int count = 0;
  for (size_t i = 42949672; i < INF; i += 42949672) {
    reserved_query_windows.push_back(make_query_range(966367641, 1181116005,
                                                      INF-i, INF-i+42949672));
    cout << count++ << endl;
  }

  io::buffered_stream<point> os(4096);
  os.open("query_experiment_report_50mb");

  fill_points_inside_ranges(reserved_query_windows, os, 1024*1024);
  fill_points_outside_ranges(reserved_query_windows, os,
                             1024*1024*1024-(1024*1024));

  os.close();
  
}

int main() {

  generate_data_50mb();
  
}
