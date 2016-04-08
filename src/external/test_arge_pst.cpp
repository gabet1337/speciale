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
  apst t(12);
  apst t2(12,0.5);
  print_success();
}

void test_destructor() {
  print_description("Starting to test construction");
  apst* t = new apst(12);
  for (int i = 0; i < 20; i++) t->insert(point(i,i));
  assert(util::file_exists("1/points"));
  delete t;
  assert(!util::file_exists("1/points"));
  print_success();
}

void test_insert1() {
  print_description("Starting to test insert of 1 element");
  apst t(12);
  t.insert(point(1,1));
  t.print();
#ifdef VALIDATE
  assert(t.is_valid());
#endif
  print_success();
}

void test_insert5() {
  print_description("Starting to test insert of 5 elements");
  apst t(12);
  t.insert(point(1,1));
  t.insert(point(2,2));
  t.insert(point(3,3));
  t.insert(point(4,4));
  t.insert(point(5,5));
  t.print();

#ifdef VALIDATE
  assert(t.is_valid());
#endif

  print_success();
}

void test_insert6() {

  print_description("Starting to test insert of 6 elements");
  apst t(12);
  t.insert(point(1,1));
  t.insert(point(2,2));
  t.insert(point(3,3));
  t.insert(point(4,4));
  t.insert(point(5,5));
  t.insert(point(6,6));
  t.print();

#ifdef VALIDATE
  assert(t.is_valid());
#endif

  print_success();

}

void test_insert8() {

  print_description("Starting to test insert of 8");
  apst t(12);
  t.insert(point(1,1));
  t.insert(point(2,2));
  t.insert(point(3,3));
  t.insert(point(4,4));
  t.insert(point(5,5));
  t.insert(point(6,6));
  t.insert(point(7,7));
  t.insert(point(8,8));

#ifdef VALIDATE
  assert(t.is_valid());
#endif

  print_success();

}

void test_insert11() {
  
  print_description("Starting to test insert of 11 elements");
  apst t(12);
  t.insert(point(1,1));
  t.insert(point(2,2));
  t.insert(point(3,3));
  t.insert(point(4,4));
  t.insert(point(5,5));
  t.insert(point(6,6));
  t.insert(point(7,7));
  t.insert(point(8,8));
  t.insert(point(9,9));
  t.insert(point(10,10));
  t.insert(point(11,11));

#ifdef VALIDATE
  assert(t.is_valid());
#endif
  t.print();
  print_success();
}

void test_insert50() {
  print_description("Starting to test insert of 50 elements with tons of splits");
  apst t(12);
  for (int i = 0; i < 50; i++) t.insert(point(i,i));
  t.print();
#ifdef VALIDATE
  assert(t.is_valid());
#endif
  print_success();
} 

void test_insert50_reverse() {
  print_description("Starting to test insert of 50 elements with tons of splits in reverse");
  apst t(12);
  for (int i = 49; i >= 0; i--) {
    t.insert(point(i,i));
    t.print();
#ifdef VALIDATE
    assert(t.is_valid());
#endif
  }
  print_success();
}

void test_insert50_odd_then_even() {
  print_description("Starting to test insert of 50 elements with tons of splits odd then even");
  apst t(12);
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
  apst t(12);
  for (int i = 0; i < 100; i++) t.insert(point(r.next(99),r.next(99)));
  t.print();
#ifdef VALIDATE
  assert(t.is_valid());
#endif
  print_success();
}

void test_1000_random_inserts() {
  print_description("Starting to test insert of 1000 randomly selected points");
  util::remove_file("test/test_1000_random_inserts_log");
  util::remove_file("test/test_1000_random_inserts_point_log");
  io::buffered_stream<point> point_log(1);

  ofstream log;
  log.open("test/test_1000_random_inserts_log");

  test::random r;
  apst t(16);
  for (int i = 0; i < 1000; i++) {
    point p = point(r.next(999), r.next(999));
    log << p;
    log.flush();
    point_log.open("test/test_1000_random_inserts_point_log");
    point_log.seek(SEEK_CUR, SEEK_END);
    point_log.write(p);
    point_log.close();
    t.insert(p);
    // t.print();
#ifdef VALIDATE
    bool is_valid = t.is_valid();
    if (!is_valid) t.print();
    assert(is_valid);
#endif
  }
  log.close();
  print_success();
}

void test_insert_from_file(const string &f) {
  print_description("Starting to test insert from file");

  io::buffered_stream<point> file(buffer_size);
  file.open(f);
  size_t file_size = file.size();
  DEBUG_MSG(file_size);
  apst t(16);
  point p;
  size_t idx = 0;
  while (!file.eof()) {
    DEBUG_MSG_FAIL(idx);
    p = file.read();
    t.insert(p);
#ifdef VALIDATE
    if (idx > file_size-5) {
      bool is_valid = t.is_valid();
      if (!is_valid) t.print();
      assert(is_valid);
    }
#endif
    idx++;
  }
  
  file.close();
}

void test_insert5_delete_1() {
  print_description("Starting to test insert of 5 element with split and delete 1 element");
  apst t(12);
  t.insert(point(1,1));
  t.insert(point(2,2));
  t.insert(point(3,3));
  t.insert(point(4,4));
  t.insert(point(5,5));
  t.print();

  // std::vector<point> points_in_2, points_in_1;
  // points_in_2.push_back(point(1,1));
  // points_in_2.push_back(point(2,2));
  // points_in_1.push_back(point(4,4));
  // points_in_1.push_back(point(5,5));
  // assert(get_points_from_file("1/points") == points_in_1);
  // assert(get_points_from_file("2/points") == points_in_2);

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
  apst t(16);
  for (int i = 0; i < 100; i++) {
    t.insert(point(i,i));
#ifdef VALIDATE
    bool is_valid = t.is_valid();
    if (!is_valid) t.print();
    assert(is_valid);
#endif
  }
  for (int i = 0; i < 100; i++) {
    if (i%2==0) t.remove(point(i,i));
#ifdef VALIDATE
    bool is_valid = t.is_valid();
    if (!is_valid) t.print();
    assert(is_valid);
#endif
  }
  t.print();
  print_success();
}



void test_100_random_inserts_50_deletes() {
  print_description("Starting to test insert of 100 randomly selected points");
  util::remove_file("test/test_100_random_inserts_50_deletes_log");
  util::remove_file("test/test_100_random_inserts_50_deletes_point_log");
  io::buffered_stream<point> point_log(1);

  ofstream log;
  log.open("test/test_100_random_inserts_50_deletes_log");
  std::vector<point> inserted_points;
  test::random r;
  apst t(16);
  for (int i = 0; i < 100; i++) {
    point p = point(r.next(999), r.next(999));
    inserted_points.push_back(p);
    log << p;
    log.flush();
    point_log.open("test/test_100_random_inserts_50_deletes_point_log");
    point_log.seek(SEEK_CUR, SEEK_END);
    point_log.write(p);
    point_log.close();
    
    t.insert(p);
#ifdef VALIDATE
    bool is_valid = t.is_valid();
    if (!is_valid) t.print();
    assert(is_valid);
#endif
  }

  random_shuffle(inserted_points.begin(), inserted_points.end());
  for (int i = 0; i < 50; i++) {
    point p = inserted_points[i];
    log << p;
    log.flush();
    point_log.open("test/test_100_random_inserts_50_deletes_point_log");
    point_log.seek(SEEK_CUR, SEEK_END);
    point_log.write(p);
    point_log.close();
    t.insert(p);
#ifdef VALIDATE
    bool is_valid = t.is_valid();
    if (!is_valid) t.print();
    assert(is_valid);
#endif
  }
  log.close();

  print_success();
}

void test_insert5_report() {
  print_description("Starting to test insert of 5 elements with report");
  apst t(12);
  for (int i = 0; i < 5; i++) t.insert(point(i,i));
  std::vector<point> true_result {point(1,1), point(2,2), point(3,3)};
  t.report(1,3,0,"test/test_insert5_report.res");
  std::vector<point> actual_result;
  util::load_file_to_container<std::vector<point>, point>
    (actual_result, "test/test_insert5_report.res", buffer_size);
  assert(actual_result == true_result);
  print_success();
}

void test_insert50_report() {
  print_description("Starting to test insert of 50 elements with report");
  apst t(12);
  for (int i = 0; i < 50; i++) {
    t.insert(point(i,i));
  }
  t.print();
  std::vector<point> true_result {point(1,1), point(2,2), point(3,3)};
  t.report(1,3,0,"test/test_insert50_report.res");
  std::vector<point> actual_result;
  util::load_file_to_container<std::vector<point>, point>
    (actual_result, "test/test_insert50_report.res", buffer_size);
  assert(actual_result == true_result);

  std::vector<point> true_result2;
  for (int i = 4; i <= 30; i++) true_result2.push_back(point(i,i));
  t.report(4,30,0,"test/test_insert50_report2.res");
  std::vector<point> actual_result2;
  util::load_file_to_container<std::vector<point>, point>
    (actual_result2, "test/test_insert50_report2.res", buffer_size);
  sort(actual_result2.begin(), actual_result2.end());
  assert(actual_result2 == true_result2);
  print_success();
}


int main() {
  cleanup();
  
  mkdir("test", 0700);

  cout << "\033[0;33m\e[4mSTARTING TEST OF APST STRUCTURE\e[24m\033[0m" << endl;

  // test_set_upper_bound();
  // test_find_range();
  // test_constructors();
  // test_destructor();
  // test_insert1();
  // test_insert5();
  // test_insert6();
  // test_insert8();
  // test_insert11();
  // test_insert50();
  // test_insert50_reverse();
  // test_insert50_odd_then_even();
  // test_100_random_inserts();
  //test_1000_random_inserts();
  //test_insert_from_file("test/test_1000_random_inserts_point_log");
  //test_insert5_delete_1();
  //test_insert100_delete50();
  //test_100_random_inserts_50_deletes();

  test_insert5_report();
  test_insert50_report();
  
  cout << "\x1b[32mALL TESTS WERE SUCCESSFUL!\x1b[0m" << endl;
  cleanup();
  return 0;
}
