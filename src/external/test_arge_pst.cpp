#include "arge_pst.hpp"
#include "../stream/stream.hpp"
#include "../common/utilities.hpp"
#include "../common/point.hpp"
#include "../internal/rb_tree.hpp"
#include "../common/test_lib.hpp"
#include "range.hpp"
#include <assert.h>
#include <iomanip>
#include <string>
#include <iostream>
#include <sstream>
#include <set>

using namespace std;

typedef ext::external_priority_search_tree apst;

void cleanup() {
  

  for (int i = 0; i < 10000 ; i++) {
    if (util::file_exists(to_string(i))) {
      util::remove_directory(to_string(i));
      //util::remove_directory("c_"+to_string(i));
    } //else break;
  }
  
  for (int i = 0; i < 10000 ; i++) {
    if (util::file_exists("tree_"+to_string(i)+".png"))
      util::remove_directory("tree_"+to_string(i)+".png");
    //else break;
  }

}

void print_success() {
  cout << "\x1b[32m\u2714\x1b[0m" << endl;
}

void set_width() {
  cout << left << setw(65);
}

void print_description(string description) {
  set_width();
  cout << description;
}

void test_constructors() {
  print_description("Starting to test construction");
  apst t(9);
  apst t2(9,0.5);
  print_success();
}

void test_insert1() {
  print_description("Starting to test insert of 1 element");
  apst t(9);
  t.insert(point(1,1));
  print_success();
}

void test_set_upper_bound() {
  print_description("Starting test of set upper bound functionality");
  set<pair<point, size_t> > points;

  auto p1 = make_pair(point(5,5), (size_t)1);
  auto p2 = make_pair(point(10,10), (size_t)2);
  auto p3 = make_pair(point(15,15), (size_t)3);

  points.insert(p1);
  points.insert(p2);
  points.insert(p3);

  assert(*points.upper_bound({point(4,4), -1}) == p1);
  assert(*points.upper_bound({point(5,5), -1}) == p2);
  assert(points.upper_bound({point(15,15),-1}) == points.end());

  print_success();
}

int main() {
  cleanup();
  cout << "\033[0;33m\e[4mSTARTING TEST OF APST STRUCTURE\e[24m\033[0m" << endl;

  test_constructors();
  test_insert1();
  test_set_upper_bound();

  cout << "\x1b[32mALL TESTS WERE SUCCESSFUL!\x1b[0m" << endl;
  cleanup();
  return 0;
}
