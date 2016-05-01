#include "../common/test_lib.hpp"
#include "../common/definitions.hpp"
#include "../common/point.hpp"
#include "../common/utilities.hpp"
#include "../stream/stream.hpp"
#include <vector>
#include <iostream>
#include <set>

using namespace std;

typedef pair<pair<int,int>, int> iii;

iii make_query_range(int x1, int x2, int y) {
  return make_pair(make_pair(x1, x2), y);
}

void fill_points_inside_ranges(vector<iii> &ranges,
                               io::buffered_stream<point> &os,
                               unsigned long long num_data) {

  test::random rand;
  
  for (auto r : ranges) {

    set<point> result;
    
    int query_x1 = r.first.first;
    int query_x2 = r.first.second;
    int query_y = r.second;

    while (result.size() < (num_data / sizeof(point))) {
      int x = rand.next(query_x2 - query_x1) + query_x1;
      int y = rand.next(INF - query_y) + query_y;
      result.insert(point(x,y));
    }

    for (point p : result) {
      os.write(p);
    }
    
  }
  
}

void fill_points_outside_ranges(vector<iii> &ranges,
                               io::buffered_stream<point> &os,
                               unsigned long long num_data) {

  test::random rand;

  for (size_t i = 0; i < num_data / 8; ) {

    point p(rand.next(INF), rand.next(INF));

    bool insert = true;
    
    for (auto r : ranges) {

      int query_x1 = r.first.first;
      int query_x2 = r.first.second;
      int query_y = r.second;

      if (util::in_range(p, query_x1, query_x2, query_y))
        insert = false;
    }

    if (insert) {
      os.write(p);
      i++;
    }
    
  }
  
}

int main() {

  vector<iii> reserved_query_windows;
  reserved_query_windows.push_back(make_query_range(107374182, 214748364, 0));           // 100 %
  reserved_query_windows.push_back(make_query_range(472446402, 837518622, 429496729));   // 20 %
  reserved_query_windows.push_back(make_query_range(1009317314, 1245540515, 1288490188));// 60 %
  reserved_query_windows.push_back(make_query_range(1460288879, 1760936590, 858993458)); // 40 %
  reserved_query_windows.push_back(make_query_range(1868310772, 2040109464, 1717986917));// 80 %

  io::buffered_stream<point> os(4096);
  os.open("query_experiment");
  
  fill_points_inside_ranges(reserved_query_windows, os, 5*1024*1024);
  fill_points_outside_ranges(reserved_query_windows, os, 10ULL*1024ULL*1024ULL*1024ULL-5ULL*5ULL*1024ULL*1024ULL);
  
  os.close();
  
}
