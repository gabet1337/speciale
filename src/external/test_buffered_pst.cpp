#include "buffered_pst.hpp"
#include <assert.h>
#include <iomanip>
#include <string>

using namespace std;

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

void test_test() {
  print_description("starting test_test");
  assert ( (true == true) && "testing test");
  print_success();
}

void test_construction() {
  print_description("starting test_construction");
  ext::buffered_pst epst(9,0.5);
  assert ( (epst.is_valid() == true) && "invariants broken");
  ext::buffered_pst epst2(15,0.5);
  assert ( (epst2.is_valid() == true) && "invariants broken");
  print_success();
}

void test_insert() {
  print_description("starting test_insert");
  ext::buffered_pst epst(9,0.5);
  epst.insert(point(1,2));
  assert ( (epst.is_valid() == true) && "invariants broken");
  print_success();
}

void test_buffer_points_less_than_point_buffer_points() {
  print_description("starting test of invariant testing for all p in Iv U Dv : p < min-y in Pv  ");
  set<point> insert_buffer, delete_buffer, point_buffer;
  insert_buffer.insert(point(1,1));
  delete_buffer.insert(point(1,2));
  point_buffer.insert(point(1,3));
  std::vector<point> union_iv_dv;
  std::set_union(insert_buffer.begin(), insert_buffer.end(),
                 delete_buffer.begin(), delete_buffer.end(),
                 std::back_inserter(union_iv_dv));
  int min_y = std::min_element(point_buffer.begin(), point_buffer.end(),
                               [] (point p1, point p2) { return p1.y < p2.y; })->y;
  assert ( (std::any_of(union_iv_dv.begin(), union_iv_dv.end(),
                        [&min_y] (point p) {
                          return p.y > min_y;
                        }) == false));

  point_buffer.insert(point(0,0));
  min_y = std::min_element(point_buffer.begin(), point_buffer.end(),
                           [] (point p1, point p2) { return p1.y < p2.y; })->y;

  assert ( (std::any_of(union_iv_dv.begin(), union_iv_dv.end(),
                        [&min_y] (point p) {
                          return p.y > min_y;
                        }) == true));

  print_success();
}

void test_no_duplicates_in_pv_iv_dv() {
  print_description("starting test of invariant testing no duplicates in iv,dv,pv");

  std::vector<point> intersection;
  //insert \cap delete = OE
  std::set<point> insert_buffer, delete_buffer;
  insert_buffer.insert(point(1,1)); insert_buffer.insert(point(1,2));
  delete_buffer.insert(point(2,2)); delete_buffer.insert(point(2,3));
  std::set_intersection(insert_buffer.begin(), insert_buffer.end(),
                        delete_buffer.begin(), delete_buffer.end(),
                        std::back_inserter(intersection));
  assert( (intersection.empty() == true) );

  insert_buffer.insert(point(2,2));
  intersection.clear();
  std::set_intersection(insert_buffer.begin(), insert_buffer.end(),
                        delete_buffer.begin(), delete_buffer.end(),
                        std::back_inserter(intersection));

  assert( intersection.empty() == false);

  print_success();
}

void test_insert_buffer_overflow() {
  print_description("starting test of insert buffer overflow");

  ext::buffered_pst epst(9,0.5);
  for (int i = 0; i < 10; i++) epst.insert(point(i,i));
  assert ( epst.is_valid() );
  for (int i = 10; i < 18; i++) epst.insert(point(i,i));
  assert ( epst.is_valid() );
  epst.insert(point(19,19));
  print_success();
}

int main() {

  cout << "\033[0;33m\e[4mSTARTING TEST OF CHILD STRUCTURE\e[24m\033[0m" << endl;
  
  test_test();
  test_construction();
  test_insert();
  test_buffer_points_less_than_point_buffer_points();
  test_no_duplicates_in_pv_iv_dv();
  test_insert_buffer_overflow();
  
  cout << "\x1b[32mALL TESTS WERE SUCCESSFUL!\x1b[0m" << endl;
  return 0;
  
}
