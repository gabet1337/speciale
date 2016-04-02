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
      util::remove_directory("c_"+to_string(i));
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
  typedef struct point_type {
    point pt;
    size_t c;
    bool deleted;
    point_type() {}
    point_type(point _p, size_t _c) : pt(_p), c(_c), deleted(false) {}
    bool operator<(const point_type &p) const {
      return pt < p.pt;
    }
  } point_type;
  std::vector<point_type> result_t;
  util::load_file_to_container<vector<point_type>, point_type>
    (result_t, file_name, buffer_size);
  std::vector<point> result;
  for (auto p : result_t) result.push_back(p.pt);
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

typedef pair<point,point> range_type;
range_type find_range(const point &p, const std::set<point> &points) {
  auto it = points.upper_bound(p);
  if (it == points.end()) {
    return range_type(*points.rbegin(), INF_POINT);
  } else if (it == points.begin()) {
    return range_type(MINUS_INF_POINT, *points.begin());
  } else {
    auto right = *it;
    return range_type(*(--it), right);
  }
}

void test_find_range() {
  print_description("Starting to test find range functionality");
  set<point> points;
  points.insert(point(1,1));
  points.insert(point(3,3));
  points.insert(point(5,5));
  points.insert(point(6,6));

  assert( find_range(point(0,0), points) == range_type(point(-INF,-INF), point(1,1)) );
  assert( find_range(point(1,1), points) == range_type(point(1,1), point(3,3)) );
  assert( find_range(point(4,4), points) == range_type(point(3,3), point(5,5)) );
  assert( find_range(point(5,5), points) == range_type(point(5,5), point(6,6)) );
  assert( find_range(point(6,6), points) == range_type(point(6,6), point(INF,INF)) );
  assert( find_range(point(10,10), points) == range_type(point(6,6), point(INF,INF)) );
    
  print_success();
}

void test_constructors() {
  print_description("Starting to test construction");
  apst t(9);
  apst t2(9,0.5);
  print_success();
}

void test_destructor() {
  print_description("Starting to test construction");
  apst* t = new apst(4);
  t->insert(point(1,1));
  t->insert(point(2,2));
  t->insert(point(3,3));
  t->insert(point(4,4));
  t->insert(point(5,5));
  assert(util::file_exists("1/points"));
  delete t;
  assert(!util::file_exists("1/points"));
  print_success();
}

void test_insert1() {
  print_description("Starting to test insert of 1 element");
  apst t(9);
  t.insert(point(1,1));
  t.print();
#ifdef VALIDATE
  assert(t.is_valid());
#endif
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

#ifdef VALIDATE
  assert(t.is_valid());
#endif

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

#ifdef VALIDATE
  assert(t.is_valid());
#endif

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

#ifdef VALIDATE
  assert(t.is_valid());
#endif

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

#ifdef VALIDATE
  assert(t.is_valid());
#endif

  print_success();
}

void test_insert50() {
  print_description("Starting to test insert of 50 elements with tons of splits");
  apst t(4);
  for (int i = 0; i < 50; i++) t.insert(point(i,i));
  t.print();
#ifdef VALIDATE
  assert(t.is_valid());
#endif
  print_success();
} 

void test_insert50_reverse() {
  print_description("Starting to test insert of 50 elements with tons of splits in reverse");
  apst t(4);
  for (int i = 49; i >= 0; i--) t.insert(point(i,i));
  t.print();
#ifdef VALIDATE
  assert(t.is_valid());
#endif
  print_success();
}

void test_insert50_odd_then_even() {
  print_description("Starting to test insert of 50 elements with tons of splits odd then even");
  apst t(4);
  for (int i = 0; i < 50; i++) if (i%2 == 0) t.insert(point(i,i));
  for (int i = 0; i < 50; i++) if (i%2) t.insert(point(i,i));
  t.print();
#ifdef VALIDATE
  assert(t.is_valid());
#endif
  print_success();
}

void test_100_random_inserts() {
  print_description("Starting to test insert of 100 randomly selected points");
  test::random r;
  apst t(4);
  for (int i = 0; i < 100; i++) t.insert(point(r.next(99),r.next(99)));
  t.print();
#ifdef VALIDATE
  assert(t.is_valid());
#endif
  print_success();
}

void test_1000_random_inserts() {
  print_description("Starting to test insert of 100 randomly selected points");
  test::random r;
  apst t(64);
  for (int i = 0; i < 1000; i++) t.insert(point(r.next(999),r.next(999)));
  t.print();
#ifdef VALIDATE
  assert(t.is_valid());
#endif
  print_success();
}

void test_insert5_delete_1() {
  print_description("Starting to test insert of 5 element with split and delete 1 element");
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

#ifdef VALIDATE
  assert(t.is_valid());
#endif

  // we now delete 1 element
  t.remove(point(4,4));

  t.print();

#ifdef VALIDATE
  assert(t.is_valid());
#endif
  print_success();
}

void test_insert100_delete50() {
  print_description("Starting to test insert of 100 element and delete 50 elements");
  apst t(4);
  for (int i = 0; i < 100; i++) t.insert(point(i,i));
#ifdef VALIDATE
  assert(t.is_valid());
#endif
  for (int i = 0; i < 100; i++) if (i%2==0) t.remove(point(i,i));
#ifdef VALIDATE
  assert(t.is_valid());
#endif
  t.print();
  print_success();
}



int main() {
  cleanup();
  cout << "\033[0;33m\e[4mSTARTING TEST OF APST STRUCTURE\e[24m\033[0m" << endl;

  test_set_upper_bound();
  test_find_range();
  test_constructors();
  test_destructor();
  test_insert1();
  test_insert5();
  test_insert6();
  test_insert8();
  test_insert11();
  test_insert50();
  test_insert50_reverse();
  // test_insert50_odd_then_even();
  // test_100_random_inserts();
  //  test_1000_random_inserts();
  // test_insert5_delete_1();
  // test_insert100_delete50();

  
  cout << "\x1b[32mALL TESTS WERE SUCCESSFUL!\x1b[0m" << endl;
  cleanup();
  return 0;
}
