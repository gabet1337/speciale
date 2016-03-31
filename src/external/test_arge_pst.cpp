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

const size_t buffer_size = 1024;

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

std::vector<point> get_points_from_file(const std::string &file_name) {
  std::vector<std::pair<point,size_t> > result_t;
  util::load_file_to_container<std::vector<pair<point, size_t> >, pair<point,size_t> >
    (result_t, file_name, buffer_size);
  std::vector<point> result;
  for (auto p : result_t) result.push_back(p.first);
  return result;
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
  t.print();
  print_success();
}

void test_insert5() {
  print_description("Starting to test insert of 5 element with split");
  apst t(4);
  t.insert(point(1,1));
  t.insert(point(2,2));
  t.insert(point(3,3));
  t.insert(point(4,4));
  t.insert(point(5,5));
  t.print();

  std::vector<point> points_in_2, points_in_1;
  points_in_2.push_back(point(1,1));
  points_in_2.push_back(point(2,2));
  points_in_1.push_back(point(4,4));
  points_in_1.push_back(point(5,5));
  assert(get_points_from_file("1/points") == points_in_1);
  assert(get_points_from_file("2/points") == points_in_2);

  print_success();
}

void test_insert6() {

  print_description("Starting to test insert of 6 element with split and correct element placement of 6th element");
  apst t(4);
  t.insert(point(1,1));
  t.insert(point(2,2));
  t.insert(point(3,3));
  t.insert(point(4,4));
  t.insert(point(5,5));
  t.insert(point(6,6));
  t.print();

  std::vector<point> points_in_2, points_in_1;
  points_in_2.push_back(point(1,1));
  points_in_2.push_back(point(2,2));
  points_in_1.push_back(point(4,4));
  points_in_1.push_back(point(5,5));
  points_in_1.push_back(point(6,6));
  assert(get_points_from_file("1/points") == points_in_1);
  assert(get_points_from_file("2/points") == points_in_2);

  print_success();

}

void test_insert8() {

  print_description("Starting to test insert of 8 elements with 2 splits and correct placement of elements");
  apst t(4);
  t.insert(point(1,1));
  t.insert(point(2,2));
  t.insert(point(3,3));
  t.insert(point(4,4));
  t.insert(point(5,5));
  t.insert(point(6,6));
  t.insert(point(7,7));
  t.print();

  std::vector<point> points_in_2, points_in_1, points_in_3;
  points_in_2.push_back(point(1,1));
  points_in_2.push_back(point(2,2));
  points_in_1.push_back(point(4,4));
  points_in_1.push_back(point(5,5));
  points_in_1.push_back(point(6,6));
  points_in_1.push_back(point(7,7));
  assert(get_points_from_file("1/points") == points_in_1);
  assert(get_points_from_file("2/points") == points_in_2);

  t.insert(point(8,8));
  //node 1 should split now!
  t.print();

  points_in_2.clear(); points_in_1.clear();
  points_in_1.push_back(point(7,7));
  points_in_1.push_back(point(8,8));
  points_in_2.push_back(point(1,1));
  points_in_2.push_back(point(2,2));
  points_in_3.push_back(point(4,4));
  points_in_3.push_back(point(5,5));
  
  assert(get_points_from_file("1/points") == points_in_1);
  assert(get_points_from_file("2/points") == points_in_2);
  assert(get_points_from_file("3/points") == points_in_3);

  print_success();

}

void test_insert11() {

  print_description("Starting to test insert of 11 elements with 3 splits and correct placement of elements");
  apst t(4);
  t.insert(point(1,1));
  t.insert(point(2,2));
  t.insert(point(3,3));
  t.insert(point(4,4));
  t.insert(point(5,5));
  t.insert(point(6,6));
  t.insert(point(7,7));
  t.print();

  std::vector<point> points_in_2, points_in_1, points_in_3, points_in_4,
    points_in_5, points_in_6;
  points_in_2.push_back(point(1,1));
  points_in_2.push_back(point(2,2));
  points_in_1.push_back(point(4,4));
  points_in_1.push_back(point(5,5));
  points_in_1.push_back(point(6,6));
  points_in_1.push_back(point(7,7));
  assert(get_points_from_file("1/points") == points_in_1);
  assert(get_points_from_file("2/points") == points_in_2);

  t.insert(point(8,8));
  //node 1 should split now!
  t.print();

  points_in_2.clear(); points_in_1.clear();
  points_in_1.push_back(point(7,7));
  points_in_1.push_back(point(8,8));
  points_in_2.push_back(point(1,1));
  points_in_2.push_back(point(2,2));
  points_in_3.push_back(point(4,4));
  points_in_3.push_back(point(5,5));
  
  assert(get_points_from_file("1/points") == points_in_1);
  assert(get_points_from_file("2/points") == points_in_2);
  assert(get_points_from_file("3/points") == points_in_3);

  t.insert(point(9,9));
  t.insert(point(10,10));
  t.insert(point(11,11));
  //node 1 should split again
  t.print();

  points_in_1.clear();
  points_in_1.push_back(point(10,10));
  points_in_1.push_back(point(11,11));
  points_in_4.push_back(point(7,7));
  points_in_4.push_back(point(8,8));
  points_in_5.push_back(point(9,9));
  points_in_6.push_back(point(3,3));

  assert(get_points_from_file("1/points") == points_in_1);
  assert(get_points_from_file("2/points") == points_in_2);
  assert(get_points_from_file("3/points") == points_in_3);
  assert(get_points_from_file("4/points") == points_in_4);
  assert(get_points_from_file("5/points") == points_in_5);
  assert(get_points_from_file("6/points") == points_in_6);


  print_success();

}


int main() {
  cleanup();
  cout << "\033[0;33m\e[4mSTARTING TEST OF APST STRUCTURE\e[24m\033[0m" << endl;

  test_set_upper_bound();
  test_constructors();
  test_insert1();
  test_insert5();
  test_insert6();
  test_insert8();
  test_insert11();

  cout << "\x1b[32mALL TESTS WERE SUCCESSFUL!\x1b[0m" << endl;
  cleanup();
  return 0;
}
