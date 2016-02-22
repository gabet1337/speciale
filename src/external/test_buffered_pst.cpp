#include "buffered_pst.hpp"
#include "../stream/stream.hpp"
#include <assert.h>
#include <iomanip>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

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

bool file_exists(string file_name) {
  struct stat st;
  return stat(file_name.c_str(),&st) == 0;
}

void test_root_split() {
  
  int lol = system("rm -rf 1/");
  lol = system("rm -rf 2/");
  lol++;
    
  print_description("starting test of root split");

  ext::buffered_pst epst(9,0.5);
  for (int i = 0; i < 9; i++) epst.insert(point(i,i));

  assert ((!file_exists("1/point_buffer") && !file_exists("2/point_buffer"))
	  && "We should have no children");

  epst.insert(point(9,9));

  assert ((file_exists("1/point_buffer") && file_exists("2/point_buffer"))
	  && "We should have children");

  io::buffered_stream<point> bs(4096);
  bs.open("1/point_buffer");
  assert (bs.read() == point(0,0) && bs.read() == point(1,1));
  bs.close();

  bs.open("2/point_buffer");
  assert (bs.read() == point(2,2) && bs.read() == point(3,3) && bs.read() == point(4,4));
  bs.close();
  
  print_success();
}

void test_root_split_insert_overflow() {
  
  int lol = system("rm -rf 1/");
  lol = system("rm -rf 2/");
  lol++;
    
  print_description("starting test of root split");

  ext::buffered_pst epst(9,0.5);
  for (int i = 0; i < 9; i++) epst.insert(point(i,i));

  assert ((!file_exists("1/point_buffer") && !file_exists("2/point_buffer"))
	  && "We should have no children");

  epst.insert(point(9,9));

  assert ((file_exists("1/point_buffer") && file_exists("2/point_buffer"))
	  && "We should have children");

  io::buffered_stream<point> bs(4096);
  bs.open("1/point_buffer");
  assert (bs.read() == point(0,0) && bs.read() == point(1,1));
  bs.close();

  bs.open("2/point_buffer");
  assert (bs.read() == point(2,2) && bs.read() == point(3,3) && bs.read() == point(4,4));
  bs.close();

  for (int i = 10; i < 23; i++) epst.insert(point(i,i));
  epst.insert(point(23,23));
  
  print_success();
}


int main() {

  cout << "\033[0;33m\e[4mSTARTING TEST OF CHILD STRUCTURE\e[24m\033[0m" << endl;
  
  test_test();
  test_construction();
  test_insert();
  test_buffer_points_less_than_point_buffer_points();
  test_no_duplicates_in_pv_iv_dv();
  //test_insert_buffer_overflow();
  test_root_split();
  test_root_split_insert_overflow();
  
  cout << "\x1b[32mALL TESTS WERE SUCCESSFUL!\x1b[0m" << endl;
  return 0;
  
}
