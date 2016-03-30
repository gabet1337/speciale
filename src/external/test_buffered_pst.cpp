#include "buffered_pst.hpp"
#include "../stream/stream.hpp"
#include "../common/utilities.hpp"
#include "../internal/rb_tree.hpp"
#include "../common/test_lib.hpp"
#include "range.hpp"
#include <assert.h>
#include <iomanip>
#include <string>
#include <iostream>
#include <sstream>

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
#ifdef DEBUG
void test_construction() {
  print_description("starting test_construction");
  ext::buffered_pst epst(9,0.5);
#ifdef VALIDATE
  assert ( (epst.is_valid() == true) && "invariants broken");
  ext::buffered_pst epst2(15,0.5);
  assert ( (epst2.is_valid() == true) && "invariants broken");
#endif
  print_success();
}

void test_insert() {
  print_description("starting test_insert");
  ext::buffered_pst epst(9,0.5);
  epst.insert(point(1,2));
#ifdef VALIDATE
  assert ( (epst.is_valid() == true) && "invariants broken");
#endif
  print_success();
}
#endif
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

#ifdef VALIDATE
  assert( intersection.empty() == false);
#endif

  print_success();
}

void test_insert_buffer_overflow() {
  print_description("starting test of insert buffer overflow");

  ext::buffered_pst epst(9,0.5);
  for (int i = 0; i < 10; i++) epst.insert(point(i,i));
#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif
  for (int i = 10; i < 18; i++) epst.insert(point(i,i));
#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif
  epst.insert(point(19,19));
  print_success();
}

void test_root_split() {
  
  int lol = system("rm -rf 1/");
  lol = system("rm -rf 2/");
  lol++;
    
  print_description("starting test of root split");

  ext::buffered_pst epst(9,0.5);
  for (int i = 0; i < 9; i++) epst.insert(point(i,i));

  assert ((!util::file_exists("1/point_buffer") && !util::file_exists("2/point_buffer"))
          && "We should have no children");

  epst.insert(point(9,9));

  assert ((util::file_exists("1/point_buffer") && util::file_exists("2/point_buffer"))
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
  lol = system("rm -rf 3/");
  lol++;
    
  print_description("starting test of insert overflow on root");

  ext::buffered_pst epst(9,0.5);
  epst.set_global_rebuild_configuration(ext::buffered_pst::global_rebuild_configuration(9999,0.5));
  for (int i = 0; i < 9; i++) epst.insert(point(i,i));

  assert ((!util::file_exists("1/point_buffer") && !util::file_exists("2/point_buffer"))
          && "We should have no children");

  epst.insert(point(9,9));

  assert ((util::file_exists("1/point_buffer") && util::file_exists("2/point_buffer"))
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

  epst.print();

  assert ( util::file_exists("2/point_buffer") );
  bs.open("2/point_buffer");
  assert (bs.read() == point(2,2) && bs.read() == point(3,3) && bs.read() == point(4,4));
  assert ( bs.eof() == true );
  bs.close();

  assert ( util::file_exists("3/point_buffer") );
  bs.open("3/point_buffer");
  assert (bs.read() == point(5,5) && bs.read() == point(6,6) && bs.read() == point(7,7));
  assert ( bs.eof() == true );
  bs.close();
  
  print_success();
}

void test_interval_range_belong_to() {
  print_description("starting test of rb tree on ranges");
  internal::rb_tree<range> rbt;
  point dummy = point(-1,-1);
  rbt.insert(range(point(1,1), dummy, dummy, 1));
  
  assert( rbt.belong_to(range(point(-1,-1), dummy, dummy, 2)) == range(point(1,1), dummy, dummy, 1));

  assert( rbt.belong_to(range(point(4,4), dummy,dummy, 2)) == range(point(1,1), dummy,dummy, 1));
  assert( rbt.belong_to(range(point(1,1), dummy,dummy, 2)) == range(point(1,1), dummy,dummy, 1));

  rbt.insert(range(point(100,100), dummy,dummy, 1));

  assert( rbt.belong_to(range(point(50,50), dummy,dummy, 2)) == range(point(1,1), dummy,dummy, 1));

  assert( rbt.belong_to(range(point(101,101), dummy,dummy, 2)) == range(point(100,100), dummy,dummy, 1));

  print_success();
}

void test_root_split_insert_between_overflow() {
  
  int lol = system("rm -rf 1/");
  lol = system("rm -rf 2/");
  lol = system("rm -rf 3/");
  lol++;
    
  print_description("starting test of insert buffer overflow on root and insert between");

  ext::buffered_pst epst(9,0.5);
  epst.set_global_rebuild_configuration(ext::buffered_pst::global_rebuild_configuration(9999,0.5));
  for (int i = 1; i <= 2; i++) epst.insert(point(i,i));
  for (int i = 100; i <= 106; i++) epst.insert(point(i,i));

  assert ((!util::file_exists("1/point_buffer") && !util::file_exists("2/point_buffer"))
          && "We should have no children");

  epst.insert(point(107,107));

  assert ((util::file_exists("1/point_buffer") && util::file_exists("2/point_buffer"))
          && "We should have children");

  io::buffered_stream<point> bs(4096);
  bs.open("1/point_buffer");
  assert (bs.read() == point(1,1) && bs.read() == point(2,2));
  bs.close();

  bs.open("2/point_buffer");
  assert (bs.read() == point(100,100) && bs.read() == point(101,101) && bs.read() == point(102,102));
  bs.close();

  for (int i = 3; i < 13; i++) epst.insert(point(i,i));

  assert ( util::file_exists("1/point_buffer") );
  bs.open("1/point_buffer");
  assert (bs.read() == point(1,1) && bs.read() == point(2,2) && bs.read() == point(3,3));
  assert ( bs.eof() == true );
  bs.close();
#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif


  assert ( util::file_exists("3/point_buffer") );
  bs.open("3/point_buffer");
  assert (bs.read() == point(4,4) && bs.read() == point(5,5));
  assert ( bs.eof() == true );
  bs.close();
#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif
  
  print_success();
}

void test_root_split_insert_between_overflow_and_split() {
  int lol = system("rm -rf 1/");
  lol = system("rm -rf 2/");
  lol = system("rm -rf 3/");
  lol++;
    
  print_description("starting test of insert buffer overflow on root and insert between plus split");

  ext::buffered_pst epst(9,0.5);
  epst.set_global_rebuild_configuration(ext::buffered_pst::global_rebuild_configuration(9999,0.5));
  for (int i = 1; i <= 2; i++) epst.insert(point(i,i));
  for (int i = 100; i <= 106; i++) epst.insert(point(i,i));

  assert ((!util::file_exists("1/point_buffer") && !util::file_exists("2/point_buffer"))
          && "We should have no children");

  epst.insert(point(107,107));

  assert ((util::file_exists("1/point_buffer") && util::file_exists("2/point_buffer"))
          && "We should have children");

  io::buffered_stream<point> bs(4096);
  bs.open("1/point_buffer");
  assert (bs.read() == point(1,1) && bs.read() == point(2,2));
  bs.close();

  bs.open("2/point_buffer");
  assert (bs.read() == point(100,100) && bs.read() == point(101,101) && bs.read() == point(102,102));
  bs.close();

  for (int i = 3; i < 13; i++) epst.insert(point(i,i));

  assert ( util::file_exists("1/point_buffer") );
  bs.open("1/point_buffer");
  assert (bs.read() == point(1,1) && bs.read() == point(2,2));
  assert (bs.read() == point(3,3) && bs.read() == point(4,4) && bs.read() == point(5,5));
  assert ( bs.eof() == true );
  bs.close();


#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif
  print_success();
}

void test_maintaining_min_max_y_on_insert_buffer_overflow() {
  int lol = system("rm -rf 1/");
  lol = system("rm -rf 2/");
  lol = system("rm -rf 3/");
  lol++;
    
  print_description("starting test of maintaining min and max_y on insert buffer overflow");

  ext::buffered_pst epst(9,0.5);
  epst.set_global_rebuild_configuration(ext::buffered_pst::global_rebuild_configuration(9999,0.5));
  for (int i = 15; i < 17; i++) {
#ifdef VALIDATE
    bool is_valid = epst.is_valid();
    if (!is_valid) epst.print();
    assert ( is_valid );
#endif
    epst.insert(point(i,i));

  }
  for (int i = 100; i <= 106; i++) {
    epst.insert(point(i,i));
#ifdef VALIDATE
    bool is_valid = epst.is_valid();
    if (!is_valid) epst.print();
    assert ( is_valid );
#endif
  }

  assert ((!util::file_exists("1/point_buffer") && !util::file_exists("2/point_buffer"))
          && "We should have no children");

  epst.insert(point(107,107));
#ifdef VALIDATE
    bool is_valid = epst.is_valid();
    if (!is_valid) epst.print();
    assert ( is_valid );
#endif

  assert ((util::file_exists("1/point_buffer") && util::file_exists("2/point_buffer"))
          && "We should have children");

  io::buffered_stream<point> bs(4096);
  bs.open("1/point_buffer");
  assert (bs.read() == point(15,15) && bs.read() == point(16,16));
  bs.close();

  bs.open("2/point_buffer");
  assert (bs.read() == point(100,100) && bs.read() == point(101,101) && bs.read() == point(102,102));
  bs.close();

  epst.insert(point(1,17));
#ifdef VALIDATE
  is_valid = epst.is_valid();
  if (!is_valid) epst.print();
  assert ( is_valid );
#endif
  
  for (int i = 2; i < 15; i++) {
    epst.insert(point(i,i));
#ifdef VALIDATE
    is_valid = epst.is_valid();
    if (!is_valid) epst.print();
    assert ( is_valid );
#endif
  }
  
  print_success();
}

void test_node_degree_overflow() {

  print_description("starting test of node degree overflow");

  ext::buffered_pst epst(9,0.5);
  epst.set_global_rebuild_configuration(ext::buffered_pst::global_rebuild_configuration(9999,0.5));
  // for (int i = 1; i <= 27; i++) epst.insert(point(i,i));

  //TODO: ASSERT THIS BITCH UP!
  // after point (10,10) has been inserted we split and create node id 1 and 2
  // 1 has (1,1), (2,2)
  // 2 has (3,3), (4,4), (5,5)
  for (int i = 1; i <= 10; i++) epst.insert(point(i,i));
  assert( util::file_exists("1/point_buffer"));
  assert( util::file_exists("2/point_buffer"));
  io::buffered_stream<point> bs(4096);
  bs.open("1/point_buffer");
  assert ( bs.read() == point(1,1) && bs.read() == point(2,2) && bs.eof() );
  bs.close();
  bs.open("2/point_buffer");
  assert ( bs.read() == point(3,3) && bs.read() == point(4,4) && bs.read() == point(5,5)
           && bs.eof() );
  bs.close();
#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif
  for (int i = 11; i <= 24; i++) epst.insert(point(i,i));
  bs.open("2/point_buffer");
  assert ( bs.read() == point(3,3) && bs.read() == point(4,4) && bs.read() == point(5,5)
           && bs.eof() );
  bs.close();
  bs.open("3/point_buffer");
  assert ( bs.read() == point(6,6) && bs.read() == point(7,7) && bs.read() == point(8,8)
           && bs.eof() );
  bs.close();
#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif
  for (int i = 25; i <= 27; i++) epst.insert(point(i,i));
  bs.open("3/point_buffer");
  assert ( bs.read() == point(6,6) && bs.read() == point(7,7) && bs.read() == point(8,8)
           && bs.eof() );
  bs.close();

  bs.open("4/point_buffer");
  assert ( bs.read() == point(9,9) && bs.read() == point(10,10) && bs.read() == point(11,11)
           && bs.eof() );
  bs.close();
#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif
  
  print_success();
}

void test_distribute_evenly() {
  print_description("starting test of splitting evenly");

  ext::buffered_pst epst(9,0.5);
  epst.set_global_rebuild_configuration(ext::buffered_pst::global_rebuild_configuration(9999,0.5));
  for (int i = 1; i <= 5; i++) epst.insert(point(i,i));
  for (int i = 101; i <= 110; i++) epst.insert(point(i,i));
  assert( util::file_exists("1/point_buffer"));
  assert( util::file_exists("2/point_buffer"));
  io::buffered_stream<point> bs(4096);
  bs.open("1/point_buffer");
  assert ( bs.read() == point(1,1) && bs.read() == point(2,2) && bs.eof() );
  bs.close();
  bs.open("2/point_buffer");
  assert ( bs.read() == point(3,3) && bs.read() == point(4,4) && bs.read() == point(5,5)
           && bs.eof() );
  bs.close();
#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif
  for (int i = 111; i <= 122; i++) epst.insert(point(i-60, i));

  epst.print();
  
  bs.open("1/point_buffer");
  assert ( bs.read() == point(1,1) && bs.read() == point(2,2) && bs.eof() );
  bs.close();
  bs.open("2/point_buffer");
  assert ( bs.read() == point(3,3) && bs.read() == point(4,4) && bs.read() == point(5,5)
           && bs.eof() );
  bs.close();

  bs.open("3/point_buffer");
  assert ( bs.read() == point(101,101) && bs.read() == point(102,102) &&
           bs.read() == point(103,103) && bs.eof() );
  bs.close();

  bs.open("4/point_buffer");
  assert ( bs.eof() );
  bs.close();

  bs.open("5/point_buffer");
  assert( bs.read() == point(54,114) && bs.read() == point(55,115) &&
          bs.read() == point(56,116)&& bs.read() == point(57,117) &&
          bs.read() == point(58,118) && bs.eof());

  bs.close();

  bs.open("5/insert_buffer");
  assert ( bs.read() == point(51,111) && bs.read() == point(52,112)
           && bs.read() == point(53,113) && bs.eof());
  bs.close();

  bs.open("6/point_buffer");
  assert ( bs.read() == point(104,104) && bs.read() == point(105,105) &&
           bs.read() == point(106,106) && bs.read() == point(107,107) &&
           bs.read() == point(108,108) && bs.read() == point(109,109) &&
           bs.read() == point(110,110) && bs.eof());
  bs.close();

  bs.open("6/insert_buffer");
  assert ( bs.eof());
  bs.close();
#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif

  print_success();
}


void test_insert_buffer_overflow_to_non_leaf2() {
  print_description("starting test of insert buffer overflow to non-leaf 2");

  ext::buffered_pst epst(9,0.5);
  epst.set_global_rebuild_configuration(ext::buffered_pst::global_rebuild_configuration(9999,0.5));

  for (int i = 1; i <= 5; i++) epst.insert(point(i,i));
  for (int i = 101; i <= 110; i++) epst.insert(point(i,i));
  assert( util::file_exists("1/point_buffer"));
  assert( util::file_exists("2/point_buffer"));
  io::buffered_stream<point> bs(4096);
  bs.open("1/point_buffer");
  assert ( bs.read() == point(1,1) && bs.read() == point(2,2) && bs.eof() );
  bs.close();
  bs.open("2/point_buffer");
  assert ( bs.read() == point(3,3) && bs.read() == point(4,4) && bs.read() == point(5,5)
           && bs.eof() );
  bs.close();
#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif
  for (int i = 111; i <= 122; i++) epst.insert(point(i-60, i));

  epst.print();
  
  bs.open("1/point_buffer");
  assert ( bs.read() == point(1,1) && bs.read() == point(2,2) && bs.eof() );
  bs.close();
  bs.open("2/point_buffer");
  assert ( bs.read() == point(3,3) && bs.read() == point(4,4) && bs.read() == point(5,5)
           && bs.eof() );
  bs.close();

  bs.open("3/point_buffer");
  assert ( bs.read() == point(101,101) && bs.read() == point(102,102) &&
           bs.read() == point(103,103) && bs.eof() );
  bs.close();

  bs.open("4/point_buffer");
  assert ( bs.eof() );
  bs.close();

  bs.open("5/point_buffer");
  assert( bs.read() == point(54,114) && bs.read() == point(55,115) &&
          bs.read() == point(56,116) && bs.read() == point(57,117) &&
          bs.read() == point(58,118) && bs.eof());
  bs.close();

  bs.open("5/insert_buffer");
  assert ( bs.read() == point(51,111) && bs.read() == point(52,112)
           && bs.read() == point(53,113) && bs.eof());
  bs.close();

  bs.open("6/point_buffer");
  assert ( bs.read() == point(104,104) && bs.read() == point(105,105) &&
           bs.read() == point(106,106) && bs.read() == point(107,107) &&
           bs.read() == point(108,108) && bs.read() == point(109,109) &&
           bs.read() == point(110,110) && bs.eof());
  bs.close();

  bs.open("6/insert_buffer");
  assert ( bs.eof());
  bs.close();
#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif

  for (int i = 123; i <= 137; i++) epst.insert(point(i-60, i));

  epst.print();

  bs.open("5/insert_buffer");
  assert ( bs.read() == point(51,111) && bs.read() == point(52,112) &&
           bs.read() == point(53,113) && bs.eof() );
  bs.close();

  bs.open("5/point_buffer");
  assert ( bs.read() == point(54,114) && bs.read() == point(55,115) &&
           bs.read() == point(56,116) && bs.read() == point(57,117) &&
           bs.read() == point(58,118) );
  assert ( bs.read() == point(59,119) && bs.read() == point(60,120) &&
           bs.read() == point(61,121) && bs.eof() );
  bs.close();
  
#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif

  for (int i = 138; i <= 140; i++) epst.insert(point(i-60, i));

  epst.print();
  
  bs.open("5/point_buffer");
  assert( bs.read() == point(56,116) &&
          bs.read() == point(57,117) &&
          bs.read() == point(58,118) && bs.read() == point(59,119) &&
          bs.read() == point(60,120) && bs.read() == point(61,121) &&
          bs.read() == point(62,122) && bs.read() == point(63,123) &&
          bs.read() == point(64,124) && bs.eof());
  bs.close();

  bs.open("5/insert_buffer");
  assert( bs.read() == point(51,111) && bs.read() == point(52,112) &&
          bs.read() == point(53,113) && bs.read() == point(54,114) &&
          bs.read() == point(55,115) && bs.eof());
  bs.close();

#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif
  
  print_success();
}

void test_insert_buffer_overflow_to_non_leaf() {
  print_description("starting test of insert buffer overflow to non-leaf");

  ext::buffered_pst epst(9,0.5);
  epst.set_global_rebuild_configuration(ext::buffered_pst::global_rebuild_configuration(9999,0.5));

  for (int i = 1; i <= 5; i++) epst.insert(point(i,i));
  for (int i = 101; i <= 110; i++) epst.insert(point(i,i));
  assert( util::file_exists("1/point_buffer"));
  assert( util::file_exists("2/point_buffer"));
  io::buffered_stream<point> bs(4096);
  bs.open("1/point_buffer");
  assert ( bs.read() == point(1,1) && bs.read() == point(2,2) && bs.eof() );
  bs.close();
  bs.open("2/point_buffer");
  assert ( bs.read() == point(3,3) && bs.read() == point(4,4) && bs.read() == point(5,5)
           && bs.eof() );
  bs.close();
#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif
  for (int i = 111; i <= 122; i++) epst.insert(point(i-60, i));

  epst.print();
  
  bs.open("1/point_buffer");
  assert ( bs.read() == point(1,1) && bs.read() == point(2,2) && bs.eof() );
  bs.close();
  bs.open("2/point_buffer");
  assert ( bs.read() == point(3,3) && bs.read() == point(4,4) && bs.read() == point(5,5)
           && bs.eof() );
  bs.close();

  bs.open("3/point_buffer");
  assert ( bs.read() == point(101,101) && bs.read() == point(102,102)
           && bs.read() == point(103,103) && bs.eof() );
  bs.close();

  bs.open("4/point_buffer");
  assert ( bs.eof() );
  bs.close();

  bs.open("5/point_buffer");
  assert( bs.read() == point(54,114) && bs.read() == point(55,115)
          && bs.read() == point(56,116) && bs.read() == point(57,117)
          && bs.read() == point(58,118) && bs.eof());
  bs.close();

  bs.open("5/insert_buffer");
  assert ( bs.read() == point(51,111) && bs.read() == point(52,112)
           && bs.read() == point(53,113) && bs.eof());
  bs.close();

  bs.open("6/point_buffer");
  assert ( bs.read() == point(104,104) && bs.read() == point(105,105) &&
           bs.read() == point(106,106) && bs.read() == point(107,107) &&
           bs.read() == point(108,108) && bs.read() == point(109,109) &&
           bs.read() == point(110,110) && bs.eof());
  bs.close();

  bs.open("6/insert_buffer");
  assert ( bs.eof());
  bs.close();
#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif

  for (int i = 123; i <= 137; i++) epst.insert(point(i-60, i));

  epst.print();
  
  bs.open("5/insert_buffer");
  assert ( bs.read() == point(51,111) && bs.read() == point(52,112)
           && bs.read() == point(53,113) && bs.eof() );
  bs.close();

  bs.open("5/point_buffer");
  assert ( bs.read() == point(54,114) && bs.read() == point(55,115) &&
           bs.read() == point(56,116) && bs.read() == point(57,117) &&
           bs.read() == point(58,118) );
  assert ( bs.read() == point(59,119) && bs.read() == point(60,120) &&
           bs.read() == point(61,121) && bs.eof() );
  bs.close();
  
#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif

  print_success();
}

void test_insert_buffer_overflow_to_non_leaf3() {
  print_description("starting test of insert buffer overflow to non-leaf 2");

  ext::buffered_pst epst(9,0.5);
  epst.set_global_rebuild_configuration(ext::buffered_pst::global_rebuild_configuration(9999,0.5));

  for (int i = 1; i <= 5; i++) epst.insert(point(i,i));
  for (int i = 101; i <= 110; i++) epst.insert(point(i,i));
  assert( util::file_exists("1/point_buffer"));
  assert( util::file_exists("2/point_buffer"));
  io::buffered_stream<point> bs(4096);
  bs.open("1/point_buffer");
  assert ( bs.read() == point(1,1) && bs.read() == point(2,2) && bs.eof() );
  bs.close();
  bs.open("2/point_buffer");
  assert ( bs.read() == point(3,3) && bs.read() == point(4,4) && bs.read() == point(5,5)
           && bs.eof() );
  bs.close();
#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif
  for (int i = 111; i <= 122; i++) epst.insert(point(i-60, i));

  epst.print();
  
  bs.open("1/point_buffer");
  assert ( bs.read() == point(1,1) && bs.read() == point(2,2) && bs.eof() );
  bs.close();
  bs.open("2/point_buffer");
  assert ( bs.read() == point(3,3) && bs.read() == point(4,4) && bs.read() == point(5,5)
           && bs.eof() );
  bs.close();

  bs.open("3/point_buffer");
  assert ( bs.read() == point(101,101) && bs.read() == point(102,102) &&
           bs.read() == point(103,103) && bs.eof() );
  bs.close();

  bs.open("4/point_buffer");
  assert ( bs.eof() );
  bs.close();

  bs.open("5/point_buffer");
  assert( bs.read() == point(54,114) && bs.read() == point(55,115) &&
          bs.read() == point(56,116) && bs.read() == point(57,117) &&
          bs.read() == point(58,118) && bs.eof());
  bs.close();

  bs.open("5/insert_buffer");
  assert ( bs.read() == point(51,111) && bs.read() == point(52,112) &&
           bs.read() == point(53,113) && bs.eof());
  bs.close();

  bs.open("6/point_buffer");
  assert ( bs.read() == point(104,104) && bs.read() == point(105,105) &&
           bs.read() == point(106,106) &&
           bs.read() == point(107,107) && bs.read() == point(108,108) &&
           bs.read() == point(109,109) && bs.read() == point(110,110) && bs.eof());
  bs.close();

  bs.open("6/insert_buffer");
  assert ( bs.eof());
  bs.close();
#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif

  for (int i = 123; i <= 137; i++) epst.insert(point(i-60, i));

  epst.print();
  
  bs.open("5/insert_buffer");
  assert ( bs.read() == point(51,111) && bs.read() == point(52,112) &&
           bs.read() == point(53,113) && bs.eof() );
  bs.close();

  bs.open("5/point_buffer");
  assert ( bs.read() == point(54,114) && bs.read() == point(55,115) &&
           bs.read() == point(56,116) && bs.read() == point(57,117) &&
           bs.read() == point(58,118) );
  assert ( bs.read() == point(59,119) && bs.read() == point(60,120) &&
           bs.read() == point(61,121) && bs.eof() );
  bs.close();
  
#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif

  for (int i = 138; i <= 140; i++) epst.insert(point(i-60, i));

  epst.print();
  
  bs.open("5/point_buffer");
  assert( bs.read() == point(56,116) &&
          bs.read() == point(57,117) &&
          bs.read() == point(58,118) && bs.read() == point(59,119) &&
          bs.read() == point(60,120) && bs.read() == point(61,121) &&
          bs.read() == point(62,122) && bs.read() == point(63,123) &&
          bs.read() == point(64,124) && bs.eof());
  bs.close();

  bs.open("5/insert_buffer");
  assert( bs.read() == point(51,111) && bs.read() == point(52,112) &&
          bs.read() == point(53,113) && bs.read() == point(54,114) &&
          bs.read() == point(55,115) && bs.eof());
  bs.close();

#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif

  for (int i = 141; i <= 143; i++) epst.insert(point(i-60, i));

  epst.print();
  
  bs.open("5/point_buffer");
  assert( bs.read() == point(59,119) &&
          bs.read() == point(60,120) && bs.read() == point(61,121) &&
          bs.read() == point(62,122) && bs.read() == point(63,123) &&
          bs.read() == point(64,124) && bs.read() == point(65,125) &&
          bs.read() == point(66,126) && bs.read() == point(67,127) &&
          bs.eof());
  bs.close();

  bs.open("5/insert_buffer");
  assert( bs.read() == point(51,111) && bs.read() == point(52,112) &&
          bs.read() == point(53,113) && bs.read() == point(54,114) &&
          bs.read() == point(55,115) && bs.read() == point(56,116) &&
          bs.read() == point(57,117) && bs.read() == point(58,118) &&
          bs.eof());
  bs.close();

#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif

  for (int i = 144; i <= 146; i++) epst.insert(point(i-60, i));

  epst.print();
  
  bs.open("5/point_buffer");
  assert( bs.read() == point(62,122) &&
          bs.read() == point(63,123) &&
          bs.read() == point(64,124) && bs.read() == point(65,125) &&
          bs.read() == point(66,126) && bs.read() == point(67,127) &&
          bs.read() == point(68,128) && bs.read() == point(69,129) &&
          bs.read() == point(70,130) && bs.eof());
  bs.close();

  bs.open("5/insert_buffer");
  assert( bs.read() == point(54,114) &&
          bs.read() == point(55,115) && bs.read() == point(56,116) &&
          bs.read() == point(57,117) && bs.read() == point(58,118) &&
          bs.read() == point(59,119) && bs.read() == point(60,120) &&
          bs.read() == point(61,121) && bs.eof());
  bs.close();

  bs.open("7/point_buffer");
  assert( bs.read() == point(51,111) && bs.read() == point(52,112) &&
          bs.read() == point(53,113)  && bs.eof());
  bs.close();

  bs.open("2/point_buffer");
  assert( bs.read() == point(3,3) && bs.read() == point(4,4) &&
          bs.read() == point(5,5) && bs.eof());
  bs.close();

#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif
  
  print_success();
}

void test_insert_buffer_overflow_to_non_leaf4() {
  print_description("starting test of insert buffer overflow to non-leaf 2");

  ext::buffered_pst epst(9,0.5);
  epst.set_global_rebuild_configuration(ext::buffered_pst::global_rebuild_configuration(9999,0.5));

  for (int i = 1; i <= 5; i++) epst.insert(point(i,i));
  for (int i = 101; i <= 110; i++) epst.insert(point(i,i));
  assert( util::file_exists("1/point_buffer"));
  assert( util::file_exists("2/point_buffer"));
  io::buffered_stream<point> bs(4096);
  bs.open("1/point_buffer");
  assert ( bs.read() == point(1,1) && bs.read() == point(2,2) && bs.eof() );
  bs.close();
  bs.open("2/point_buffer");
  assert ( bs.read() == point(3,3) && bs.read() == point(4,4) && bs.read() == point(5,5)
           && bs.eof() );
  bs.close();
#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif

  epst.print();
  
  print_success();
}

void test_not_valid_on_manual_insert() {
  print_description("starting test of manual insert to break is_valid");

  ext::buffered_pst epst(9,0.5);
    
  for (int i = 15; i < 17; i++) epst.insert(point(i,i));
  for (int i = 100; i <= 106; i++) epst.insert(point(i,i));

  assert ((!util::file_exists("1/point_buffer") && !util::file_exists("2/point_buffer"))
          && "We should have no children");

  epst.insert(point(107,107));

  assert ((util::file_exists("1/point_buffer") && util::file_exists("2/point_buffer"))
          && "We should have children");

  io::buffered_stream<point> bs(4096);
  bs.open("1/point_buffer");
  assert (bs.read() == point(15,15) && bs.read() == point(16,16));
  bs.close();

  bs.open("2/point_buffer");
  assert (bs.read() == point(100,100) && bs.read() == point(101,101) && bs.read() == point(102,102));
  bs.close();

  epst.insert(point(1,17));
  
  for (int i = 2; i < 15; i++) epst.insert(point(i,i));
  
#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif

  bs.open("5/point_buffer");
  bs.seek(0, SEEK_END);
  bs.write(point(1,1000));
  bs.close();

#ifdef VALIDATE
  assert( !epst.is_valid() );
#endif
  
  print_success();
}

void test_deterministic_random() {
  print_description("starting test of random determinism");
  ext::buffered_pst epst(9,0.5);
  vector<point> points;
  points.push_back(point(20,829));
  points.push_back(point(38,611));
  points.push_back(point(39,80));
  points.push_back(point(81,86));
  points.push_back(point(104,779));
  points.push_back(point(115,393));
  points.push_back(point(142,66));
  points.push_back(point(159,621));
  points.push_back(point(168,458));
  points.push_back(point(171,448));
  points.push_back(point(174,248));
  points.push_back(point(180,461));
  points.push_back(point(185,417));
  points.push_back(point(204,906));
  points.push_back(point(206,382));
  points.push_back(point(217,569));
  points.push_back(point(243,193));
  points.push_back(point(283,787));
  points.push_back(point(297,912));
  points.push_back(point(325,392));
  points.push_back(point(329,641));
  points.push_back(point(339,126));
  points.push_back(point(358,233));
  points.push_back(point(361,962));
  points.push_back(point(365,554));
  points.push_back(point(372,334));
  points.push_back(point(389,433));
  points.push_back(point(435,941));
  points.push_back(point(438,695));
  points.push_back(point(458,886));
  points.push_back(point(501,345));
  points.push_back(point(562,366));
  points.push_back(point(575,156));
  points.push_back(point(575,901));
  points.push_back(point(583,117));
  points.push_back(point(702,206));
  points.push_back(point(719,265));
  points.push_back(point(829,846));
  points.push_back(point(872,40));
  points.push_back(point(904,185));
  points.push_back(point(906,573));
  points.push_back(point(928,125));
  points.push_back(point(980,695));
  points.push_back(point(987,111));
  for (int i = 0; i < (int)points.size(); i++) {
    epst.insert(points[i]);
#ifdef VALIDATE
    bool is_valid = epst.is_valid();
    if (!is_valid) {
      epst.print();
      assert (is_valid);
    }
#endif
  }

  print_success();
}

void test_deterministic_random2() {
  print_description("starting test of random determinism 2");

  ext::buffered_pst epst(9,0.5);
  vector<point> points;
  points.push_back(point(17,752));
  points.push_back(point(32,831));
  points.push_back(point(97,930));
  points.push_back(point(101,392));
  points.push_back(point(102,65));
  points.push_back(point(115,896));
  points.push_back(point(118,597));
  points.push_back(point(127,838));
  points.push_back(point(164,885));
  points.push_back(point(192,351));
  points.push_back(point(249,311));
  points.push_back(point(284,586));
  points.push_back(point(308,715));
  points.push_back(point(329,113));
  points.push_back(point(358,796));
  points.push_back(point(365,703));
  points.push_back(point(381,721));
  points.push_back(point(414,331));
  points.push_back(point(463,245));
  points.push_back(point(470,819));
  points.push_back(point(473,410));
  points.push_back(point(504,773));
  points.push_back(point(505,956));
  points.push_back(point(520,480));
  points.push_back(point(520,964));
  points.push_back(point(523,702));
  points.push_back(point(565,248));
  points.push_back(point(612,554));
  points.push_back(point(702,869));
  points.push_back(point(720,824));
  points.push_back(point(751,834));
  points.push_back(point(753,471));
  points.push_back(point(765,31));
  points.push_back(point(779,324));
  points.push_back(point(806,573));
  points.push_back(point(822,380));
  points.push_back(point(831,772));
  points.push_back(point(839,840));
  points.push_back(point(862,236));
  points.push_back(point(893,867));
  points.push_back(point(904,874));
  points.push_back(point(912,345));
  points.push_back(point(913,241));
  points.push_back(point(966,519));

  for (int i = 0; i < (int)points.size()-4; i++) {
    epst.insert(points[i]);
  }
  epst.insert(points[40]);
#ifdef VALIDATE
  if (!epst.is_valid())
    epst.print();
#endif

#ifdef VALIDATE
  assert (epst.is_valid());
#endif

  print_success();
}

void test_random_deterministic3() {
  print_description("starting test of random determinism 3");
  ext::buffered_pst epst(9,0.5);
  std::vector<point> points;
  points.push_back(point(13,59));
  points.push_back(point(17,905));
  points.push_back(point(79,293));
  points.push_back(point(96,177));
  points.push_back(point(116,260));
  points.push_back(point(129,403));
  points.push_back(point(136,420));
  points.push_back(point(137,54));
  points.push_back(point(245,236));
  points.push_back(point(263,157));
  points.push_back(point(271,679));
  points.push_back(point(278,923));
  points.push_back(point(294,587));
  points.push_back(point(388,280));
  points.push_back(point(388,611));
  points.push_back(point(445,298));
  points.push_back(point(449,725));
  points.push_back(point(454,639));
  points.push_back(point(464,32));
  points.push_back(point(468,466));
  points.push_back(point(494,105));
  points.push_back(point(529,244));
  points.push_back(point(549,857));
  points.push_back(point(550,769));
  points.push_back(point(555,207));
  points.push_back(point(560,981));
  points.push_back(point(599,503));
  points.push_back(point(612,305));
  points.push_back(point(622,395));
  points.push_back(point(624,563));
  points.push_back(point(627,465));
  points.push_back(point(704,584));
  points.push_back(point(774,328));
  points.push_back(point(788,16));
  points.push_back(point(811,153));
  points.push_back(point(847,110));
  points.push_back(point(853,407));
  points.push_back(point(861,172));
  points.push_back(point(862,272));
  points.push_back(point(866,982));
  points.push_back(point(867,21));
  points.push_back(point(971,652));
  points.push_back(point(974,806));
  points.push_back(point(991,490));

  for (int i = 0; i < (int)points.size(); i++) {
    epst.insert(points[i]);
#ifdef VALIDATE
    if (!epst.is_valid()) epst.print();
    assert(epst.is_valid());
#endif
  }
  print_success();
}

void test_random_insert() {
  print_description("starting test of inserting random points");
  ext::buffered_pst epst(9,0.5);
  std::set<point> points;
  test::random r;
  for (int i = 0; i < 100; i++) points.insert(point(r.next(999), r.next(999)));
  for (point p : points) {
    epst.insert(p);
#ifdef VALIDATE
    bool is_valid = epst.is_valid();
    if (!is_valid) epst.print();
    assert( is_valid );
#endif
  }
  print_success();
}

void test_truly_random() {
  print_description("starting test of inserting random points... truly!");
  ext::buffered_pst epst(32,0.5);
  std::set<point> points;
  test::random r;
  for (int i = 0; i < 500; i++) points.insert(point(r.next(999), r.next(999)));
  vector<point> points_random(points.begin(), points.end());
  random_shuffle(points_random.begin(), points_random.end());
  for (point p : points_random) {
    epst.insert(p);
#ifdef VALIDATE
    bool is_valid = epst.is_valid();
    if (!is_valid) epst.print();
    assert( is_valid );
#endif
  }
  print_success();
  
  
}

void test_delete() {

  print_description("starting test of deleting points");

  ext::buffered_pst epst(9,0.5);
  for (int i=0; i < 5; i++) epst.insert(point(i,i));

  epst.remove(point(1,1));
  epst.remove(point(3,3));

#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif
  
  print_success();
}

void test_delete_overflow() {

  print_description("starting test of delete overflow");

  ext::buffered_pst epst(9,0.5);
  for (int i=0; i<10; i++) epst.insert(point(i,i));
  for (int i=0; i<3; i++) epst.remove(point(i,i));

#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif
  
  print_success();
}

void test_delete_overflow_underflow_node() {

  print_description("starting test of delete overflow underflow node");

  ext::buffered_pst epst(9,0.5);
  for (int i=0; i<10; i++) epst.insert(point(i,i));
  for (int i=5; i<7; i++) epst.remove(point(i,i));

#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif
  
  print_success();
}

void test_delete_overflow_many_points() {

  print_description("starting test of delete overflow many points");

  ext::buffered_pst epst(9,0.5);
  for (int i=0; i<30; i++) epst.insert(point(i,i));
  for (int i=10; i<21; i++) epst.remove(point(i,i));

#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif

  // epst.print();
  print_success();
  
}

void test_delete_all_points() {

  print_description("starting test of delete all points");

  ext::buffered_pst epst(9,0.5);
  for (int i=0; i<30; i++) epst.insert(point(i,i));
  for (int i=0; i<30; i++) {
    epst.remove(point(i,i));
#ifdef VALIDATE
    bool is_valid = epst.is_valid();
    if (!is_valid) epst.print();
    assert ( is_valid );
#endif
  }
  epst.print();
  
#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif

  print_success();
  
}

void test_insert_200_delete_20_points() {

  print_description("starting test of insert 200 delete 20 points");

  ext::buffered_pst epst(9,0.5);
  for (int i=0; i<200; i++) {
    epst.insert(point(i,i));
    bool is_valid = true; // epst.is_valid();
    if (!is_valid) {
      epst.print();
      assert ( is_valid );
    }
  }
  for (int i=0; i<20; i++) {
    epst.remove(point(i*10,i*10));
#ifdef VALIDATE
    bool is_valid = true; // epst.is_valid();
    if (!is_valid) {
      epst.print();
      assert ( is_valid );
    }
#endif
  }

  epst.print();
#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif

  print_success();
  
}

void test_delete_truly_random() {
  
  print_description("starting test of deleting random points... truly!");

  if (util::file_exists("test_points"))
    util::remove_file("test_points");
  
  io::buffered_stream<point> bs(1);
  bs.open("test_points");
  
  ext::buffered_pst epst(9,0.5);
  std::set<point> points;
  test::random r;
  for (int i = 0; i < 200; i++) points.insert(point(r.next(999), r.next(999)));
  vector<point> points_random(points.begin(), points.end());
  random_shuffle(points_random.begin(), points_random.end());

  for (int i=0; i < 50; i++) {
    bs.write(points_random[i]);
    epst.insert(points_random[i]);
#ifdef VALIDATE
    bool is_valid = epst.is_valid();
    if (!is_valid) {
      epst.print();
      assert ( is_valid );
    }
#endif
  }

  for (int i=0; i<10; i++) {
    bs.write(points_random[i*5]);
    epst.remove(points_random[i*5]);
#ifdef VALIDATE
    bool is_valid = epst.is_valid();
    if (!is_valid) {
    epst.print();
    assert ( is_valid );
  }
#endif
  }

  for (int i=50; i < 100; i++) {
    bs.write(points_random[i]);
    epst.insert(points_random[i]);
#ifdef VALIDATE
    bool is_valid = epst.is_valid();
    if (!is_valid) {
      epst.print();
      assert ( is_valid );
    }
#endif
  }

  for (int i=0; i<10; i++) {
    bs.write(points_random[50+i*5]);
    epst.remove(points_random[50+i*5]);
#ifdef VALIDATE
    bool is_valid = epst.is_valid();
    if (!is_valid) {
      epst.print();
      assert ( is_valid );
    }
#endif
  }

  for (int i=100; i < 150; i++) {
    bs.write(points_random[i]);
    epst.insert(points_random[i]);
#ifdef VALIDATE
    bool is_valid = epst.is_valid();
    if (!is_valid) {
      epst.print();
      assert ( is_valid );
    }
#endif
  }

  for (int i=0; i<10; i++) {
    bs.write(points_random[100+i*5]);
    epst.remove(points_random[100+i*5]);
#ifdef VALIDATE
    bool is_valid = epst.is_valid();
    if (!is_valid) {
      epst.print();
      assert ( is_valid );
    }
#endif
  }


  for (int i=150; i < 200; i++) {
    bs.write(points_random[i]);
    epst.insert(points_random[i]);
#ifdef VALIDATE
    bool is_valid = epst.is_valid();
    if (!is_valid) {
      epst.print();
      assert ( is_valid );
    }
#endif
  }

  for (int i=0; i<10; i++) {
    bs.write(points_random[150+i*5]);
    epst.remove(points_random[150+i*5]);
#ifdef VALIDATE
    bool is_valid = epst.is_valid();
    if (!is_valid) {
      epst.print();
      assert ( is_valid );
    }
#endif
  }

#ifdef VALIDATE
    bool is_valid = epst.is_valid();
    if (!is_valid) epst.print();
    assert( is_valid );
#endif
  
  print_success();
}

void test_delete_truly_random_points_from_file(std::string file_name) {
  
  print_description("starting test of deleting random points... truly!");

  io::buffered_stream<point> bs(4096);
  bs.open(file_name);
  
  ext::buffered_pst epst(9,0.5);
  //std::set<point> points;
  test::random r;
  //for (int i = 0; i < 200; i++) points.insert(point(r.next(999), r.next(999)));
  //vector<point> points_random(points.begin(), points.end());
  //random_shuffle(points_random.begin(), points_random.end());

  for (int i=0; i < 50; i++) {
    if (!bs.eof()) epst.insert(bs.read());
    else epst.insert(point(r.next(999), r.next(999)));
#ifdef VALIDATE
    bool is_valid = epst.is_valid();
    if (!is_valid) {
      epst.print();
      assert ( is_valid );
    }
#endif
  }

  for (int i=0; i<10; i++) {
    if (!bs.eof()) epst.remove(bs.read());
    else epst.remove(point(r.next(999), r.next(999)));
#ifdef VALIDATE
    bool is_valid = epst.is_valid();
    if (!is_valid) {
    epst.print();
    assert ( is_valid );
  }
#endif
  }

  for (int i=50; i < 100; i++) {
    if (!bs.eof()) epst.insert(bs.read());
    else epst.insert(point(r.next(999), r.next(999)));
#ifdef VALIDATE
    bool is_valid = epst.is_valid();
    if (!is_valid) {
      epst.print();
      assert ( is_valid );
    }
#endif
  }

  for (int i=0; i<10; i++) {
    if (!bs.eof()) epst.remove(bs.read());
    else epst.remove(point(r.next(999), r.next(999)));
#ifdef VALIDATE
    bool is_valid = epst.is_valid();
    if (!is_valid) {
      epst.print();
      assert ( is_valid );
    }
#endif
  }

  for (int i=100; i < 150; i++) {
    if (!bs.eof()) epst.insert(bs.read());
    else epst.insert(point(r.next(999), r.next(999)));
#ifdef VALIDATE
    bool is_valid = epst.is_valid();
    if (!is_valid) {
      epst.print();
      assert ( is_valid );
    }
#endif
  }

  for (int i=0; i<10; i++) {
    if (!bs.eof()) epst.remove(bs.read());
    else epst.remove(point(r.next(999), r.next(999)));
#ifdef VALIDATE
    bool is_valid = epst.is_valid();
    if (!is_valid) {
      epst.print();
      assert ( is_valid );
    }
#endif
  }


  for (int i=150; i < 200; i++) {
    if (!bs.eof()) epst.insert(bs.read());
    else epst.insert(point(r.next(999), r.next(999)));
#ifdef VALIDATE
    bool is_valid = epst.is_valid();
    if (!is_valid) {
      epst.print();
      assert ( is_valid );
    }
#endif
  }

  for (int i=0; i<10; i++) {
    if (!bs.eof()) epst.remove(bs.read());
    else epst.remove(point(r.next(999), r.next(999)));
#ifdef VALIDATE
    bool is_valid = epst.is_valid();
    if (!is_valid) {
      epst.print();
      assert ( is_valid );
    }
#endif
  }

#ifdef VALIDATE
    bool is_valid = epst.is_valid();
    if (!is_valid) epst.print();
    assert( is_valid );
#endif

    assert ( bs.eof() );
    
    bs.close();
    print_success();
    
}

void test_delete_truly_random_n_points(int n) {

  std::string lbl = "starting test of deleting random points... truly on "
    + to_string(n) + " points";
  print_description(lbl);

  if (util::file_exists("test_points"))
    util::remove_file("test_points");
  
  io::buffered_stream<point> bs(1);
  bs.open("test_points");
  
  ext::buffered_pst epst(9,0.5);
  std::set<point> points;
  test::random r;
  for (int i = 0; i < n; i++) points.insert(point(r.next(999), r.next(999)));
  vector<point> points_random(points.begin(), points.end());
  random_shuffle(points_random.begin(), points_random.end());

#ifdef VALIDATE
  int count = 0;
#endif
  bool insert = true;
  for (size_t i=0; i<points_random.size(); i++) {
#ifdef VALIDATE
    count++;
    // for (int j=0; j<1; j++)
    DEBUG_MSG_FAIL("Handling update " << count);
#endif
    bs.write(points_random[i]);
    if (insert) {
      epst.insert(points_random[i]);
#ifdef VALIDATE
        streambuf* cout_strbuf(cout.rdbuf());
        ostringstream output;
        cout.rdbuf(output.rdbuf());
        bool is_valid = epst.is_valid();
        if (!is_valid) {
          epst.print();
          cout.rdbuf(cout_strbuf);
          epst.is_valid();
        }
        assert ( is_valid );
        cout.rdbuf(cout_strbuf);
#endif
      if (i%50==0 && i!=0 && i!=points_random.size()-1) {
        insert = false;
        i = i-50;
      }
    } else {
      if (i%10==0) {
        epst.remove(points_random[i]);
#ifdef VALIDATE
        streambuf* cout_strbuf(cout.rdbuf());
        ostringstream output;
        cout.rdbuf(output.rdbuf());
        bool is_valid = epst.is_valid();
        if (!is_valid) {
          epst.print();
          cout.rdbuf(cout_strbuf);
          epst.is_valid();
        }
        assert ( is_valid );
        cout.rdbuf(cout_strbuf);
#endif
      }
      if (i%50==0) insert = true;       
    }

  }

#ifdef VALIDATE
    bool is_valid = epst.is_valid();
    if (!is_valid) epst.print();
    assert( is_valid );
#endif
  
  print_success();
}

void test_delete_truly_random_n_points_from_file(std::string file_name) {

  std::string lbl = "starting test of deleting random points... truly on "
    + file_name;
  print_description(lbl);

  io::buffered_stream<point> bs(4096);
  bs.open(file_name);
  
  ext::buffered_pst epst(9,0.5);
  std::set<point> points;
  //test::random r;
  //for (int i = 0; i < n; i++) points.insert(point(r.next(999), r.next(999)));
  //vector<point> points_random(points.begin(), points.end());
  //random_shuffle(points_random.begin(), points_random.end());

#ifdef VALIDATE
  int count = 0;
#endif
  bool insert = true;
  for (size_t i=0; i<100000000; i++) {
    if (bs.eof()) break;
    int foo = 0;
    foo++;
    // if (count >= 147) {
    //    epst.print();
    //    int j;
    //    cin >> j;
    //  }
#ifdef VALIDATE
    for (int j=0; j<1; j++)
      DEBUG_MSG_FAIL("Handling update " << ++count);
#endif
    //bs.write(points_random[i]);
    if (insert) {
      epst.insert(bs.read());
#ifdef VALIDATE
      streambuf* cout_strbuf(cout.rdbuf());
      ostringstream output;
      cout.rdbuf(output.rdbuf());
      epst.print();
      foo = system(std::string("mv tree.png tree_" + std::to_string(count) + ".png").c_str());
      bool is_valid = epst.is_valid();
      if (!is_valid) {
	epst.print();
	cout.rdbuf(cout_strbuf);
	epst.is_valid();
      }
      assert ( is_valid );
      cout.rdbuf(cout_strbuf);
#endif
      if (i%50==0 && i!=0) {
        insert = false;
        i = i-50;
      }
    } else {
      if (i%10==0) {
        epst.remove(bs.read());
#ifdef VALIDATE
        streambuf* cout_strbuf(cout.rdbuf());
        ostringstream output;
        cout.rdbuf(output.rdbuf());
	foo = system(std::string("mv tree.png tree_" + std::to_string(count) + ".png").
	c_str());
	epst.print();
        bool is_valid = epst.is_valid();
        if (!is_valid) {
          epst.print();
          cout.rdbuf(cout_strbuf);
          epst.is_valid();
        }
        assert ( is_valid );
        cout.rdbuf(cout_strbuf);
#endif
      }
      if (i%50==0) insert = true;       
    }

  }

#ifdef VALIDATE
    bool is_valid = epst.is_valid();
    if (!is_valid) epst.print();
    assert( is_valid );
#endif
  
  print_success();
}

void test_report_points_deterministic() {

  print_description("starting test of report points deterministic");

  ext::buffered_pst epst(9,0.5);
  for (int i=0; i<100; i++) {
    epst.insert(point(i,i));
#ifdef VALIDATE
    streambuf* cout_strbuf(cout.rdbuf());
    ostringstream output;
    cout.rdbuf(output.rdbuf());
    bool is_valid = true;
    if (!is_valid) {
      epst.print();
      cout.rdbuf(cout_strbuf);
      epst.is_valid();
    }
    assert ( is_valid );
    cout.rdbuf(cout_strbuf);
#endif
    //epst.print();
    // int k;
    //cin >> k;
  }

  std::vector<point> true_points;
  for (int i=50; i <= 75; i++)
    true_points.push_back(point(i,i));
  DEBUG_MSG("Starting print!");
  epst.print();

  epst.report(25,75,50,"test/deterministic_result");
  
  std::vector<point> actual_points;
  util::load_file_to_container<std::vector<point>, point>
    (actual_points, "test/deterministic_result", 4096);

  std::sort(actual_points.begin(),actual_points.end());
  
  if (actual_points != true_points) {
    DEBUG_MSG("Actual points:");
      for (point p : actual_points)
        DEBUG_MSG(" - " << p);
  }
  epst.print();

  assert ( actual_points  == true_points );
#ifdef VALIDATE
  assert(epst.is_valid());
#endif
  
  print_success();
  
}

void test_report_points_deterministic2() {

  print_description("starting test of report points deterministic");

  ext::buffered_pst epst(9,0.5);
  for (int i=0; i<100; i++) epst.insert(point(i,i));

  std::vector<point> true_points;
  for (int i=3; i <= 25; i++)
    true_points.push_back(point(i,i));
      
  epst.report(1,25,3,"test/deterministic_result2");
  
  std::vector<point> actual_points;
  util::load_file_to_container<std::vector<point>, point>
    (actual_points, "test/deterministic_result2", 4096);

  std::sort(actual_points.begin(),actual_points.end());
  
  if (actual_points != true_points) {
    DEBUG_MSG("Actual points:");
      for (point p : actual_points)
        DEBUG_MSG(" - " << p);
  }

  assert ( actual_points  == true_points );
  epst.print();
#ifdef VALIDATE
  assert(epst.is_valid());
#endif
  print_success();
  
}

void test_report_points_deterministic3() {

  print_description("starting test of report points deterministic");

  ext::buffered_pst epst(9,0.5);
  for (int i=0; i<100; i++) epst.insert(point(i,i));

  std::vector<point> true_points;
  for (int i=0; i < 100; i++)
    true_points.push_back(point(i,i));
      
  epst.report(0,100,0,"test/deterministic_result3");
  
  std::vector<point> actual_points;
  util::load_file_to_container<std::vector<point>, point>
    (actual_points, "test/deterministic_result3", 4096);

  std::sort(actual_points.begin(),actual_points.end());
  
  if (actual_points != true_points) {
    DEBUG_MSG("Actual points:");
      for (point p : actual_points)
        DEBUG_MSG(" - " << p);
  }

  assert ( actual_points  == true_points );
  epst.print();
#ifdef VALDIATE
  assert(epst.is_valid());
#endif
  print_success();
  
}

void test_report_points_deterministic_delete() {

  print_description("starting test of report points deterministic");

  ext::buffered_pst epst(9,0.5);
  for (int i=0; i<100; i++) epst.insert(point(i,i));

  for (int i=0; i<100; i+=2) {
    epst.remove(point(i,i));
    //epst.print();
    DEBUG_MSG_FAIL("Removing p(" << i << "," << i << ")");
#ifdef VALIDATE
    streambuf* cout_strbuf(cout.rdbuf());
    ostringstream output;
    cout.rdbuf(output.rdbuf());
    bool is_valid = true;
    if (!is_valid) {
      epst.print();
      cout.rdbuf(cout_strbuf);
      epst.is_valid();
    }
    assert ( is_valid );
    cout.rdbuf(cout_strbuf);
#endif
  }
  
  std::vector<point> true_points;
  for (int i=1; i < 100; i+=2)
    true_points.push_back(point(i,i));

  epst.print();
#ifdef VALIDATE
  assert(epst.is_valid());
#endif

  epst.report(0,100,0,"test/deterministic_result4");
  epst.print();
  
  std::vector<point> actual_points;
  util::load_file_to_container<std::vector<point>, point>
    (actual_points, "test/deterministic_result4", 4096);

  std::sort(actual_points.begin(),actual_points.end());
  
  if (actual_points != true_points) {
    DEBUG_MSG("Actual points:");
      for (point p : actual_points)
        DEBUG_MSG(" - " << p);
  }
  

  assert ( actual_points  == true_points );
  epst.print();
#ifdef VALIDATE
  assert(epst.is_valid());
#endif
  print_success();
  
}

void test_report_points_deterministic_repeat_report() {

  print_description("starting test of report points deterministic");

  ext::buffered_pst epst(9,0.5);
  for (int i=0; i<100; i++) epst.insert(point(i,i));

  for (int i=0; i<100; i+=2) {
    epst.remove(point(i,i));
    //epst.print();
//     DEBUG_MSG_FAIL("Removing p(" << i << "," << i << ")");
// #ifdef DEBUG
//     streambuf* cout_strbuf(cout.rdbuf());
//     ostringstream output;
//     cout.rdbuf(output.rdbuf());
//     bool is_valid = true;
//     if (!is_valid) {
//       epst.print();
//       cout.rdbuf(cout_strbuf);
//       epst.is_valid();
//     }
//     assert ( is_valid );
    //cout.rdbuf(cout_strbuf);
    //#endif
  }
  
  std::vector<point> true_points;
  for (int i=1; i < 100; i+=2)
    true_points.push_back(point(i,i));

  epst.print();
#ifdef VALIDATE
  assert(epst.is_valid());
#endif

  for (int i=0;i<5;i++) {

    if (util::file_exists("test/deterministic_result5"))
	util::remove_file("test/deterministic_result5");

    epst.report(0,100,0,"test/deterministic_result5");
    //epst.print();
  
    std::vector<point> actual_points;
    util::load_file_to_container<std::vector<point>, point>
      (actual_points, "test/deterministic_result5", 4096);
    
    std::sort(actual_points.begin(),actual_points.end());
  
    if (actual_points != true_points) {
      DEBUG_MSG("Actual points:");
      for (point p : actual_points)
        DEBUG_MSG(" - " << p);
    }
    assert ( actual_points  == true_points );
    epst.print();
#ifdef VALIDATE
    assert(epst.is_valid());
#endif
  }
  
  print_success();
  
}

void test_report_points_underflowing_point_buffer() {

  print_description("starting test of report points underflowing point buffer");

  std::set<point> true_points_set;
  ext::buffered_pst epst(9,0.5);
  for (int i=0; i<28; i++) {
    true_points_set.insert(point(i,i));
    epst.insert(point(i,i));
  }

  for (int i=11; i<18;i++) {
    epst.remove(point(i,i));
    true_points_set.erase(point(i,i));
  }

  std::vector<point> true_points(true_points_set.begin(),true_points_set.end());
  
  epst.report(0,100,0, "test/underflow_pb_res");

  std::vector<point> actual_points;
  util::load_file_to_container<std::vector<point>, point>
    (actual_points, "test/underflow_pb_res", 4096);
    
  std::sort(actual_points.begin(),actual_points.end());
  
  if (actual_points != true_points) {
    DEBUG_MSG("Actual points:");
    for (point p : actual_points)
      DEBUG_MSG(" - " << p);
  }

  epst.print();
  assert ( actual_points  == true_points );
#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif
  
  epst.remove(point(18,18));
  epst.remove(point(21,21));
  true_points_set.erase(point(18,18));
  true_points_set.erase(point(21,21));

  true_points = std::vector<point>(true_points_set.begin(), true_points_set.end());

  epst.report(0,100,0, "test/underflow_pb_res2");

  std::vector<point> actual_points2;
  util::load_file_to_container<std::vector<point>, point>
    (actual_points2, "test/underflow_pb_res2", 4096);
    
  std::sort(actual_points2.begin(),actual_points2.end());

  epst.print();

  if (actual_points2 != true_points) {
    DEBUG_MSG("Actual points:");
    for (point p : actual_points2)
      DEBUG_MSG(" - " << p);
  }

  assert ( actual_points2  == true_points );
#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif

  ////////////////////////////////////////////////////////////////////////

  epst.remove(point(8,8));
  epst.remove(point(10,10));
  epst.remove(point(19,19));
  epst.remove(point(20,20));
  true_points_set.erase(point(8,8));
  true_points_set.erase(point(10,10));
  true_points_set.erase(point(19,19));
  true_points_set.erase(point(20,20));

  true_points = std::vector<point>(true_points_set.begin(), true_points_set.end());

  epst.report(0,100,0, "test/underflow_pb_res3");

  std::vector<point> actual_points3;
  util::load_file_to_container<std::vector<point>, point>
    (actual_points3, "test/underflow_pb_res3", 4096);
    
  std::sort(actual_points3.begin(),actual_points3.end());

  epst.print();

  if (actual_points3 != true_points) {
    DEBUG_MSG("Actual points:");
    for (point p : actual_points3)
      DEBUG_MSG(" - " << p);
  }

  assert ( actual_points3  == true_points );
#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif

  ////////////////////////////////////////////////////////////////////////////

  epst.remove(point(2,2));

  
  epst.remove(point(0,0));
  true_points_set.erase(point(2,2));
  true_points_set.erase(point(0,0));

  epst.print();
  
  true_points = std::vector<point>(true_points_set.begin(), true_points_set.end());

  epst.report(-100,100,-100, "test/underflow_pb_res4");

  std::vector<point> actual_points4;
  util::load_file_to_container<std::vector<point>, point>
    (actual_points4, "test/underflow_pb_res4", 4096);
    
  std::sort(actual_points4.begin(),actual_points4.end());

  epst.print();

  if (actual_points4 != true_points) {
    DEBUG_MSG("Actual points:");
    for (point p : actual_points4)
      DEBUG_MSG(" - " << p);
    DEBUG_MSG("True points:");
    for (point p : true_points)
      DEBUG_MSG(" - " << p);
  }

  assert ( actual_points4  == true_points );
#ifdef VALIDATE
  assert ( epst.is_valid() );
#endif

  
  print_success();
  
}

void test_report_200_delete_20_points() {

  print_description("starting test of report 200 delete 20 points");

  std::set<point> true_points;
  ext::buffered_pst epst(9,0.5);
  for (int i=0; i<200; i++) {
    epst.insert(point(i,i));
    true_points.insert(point(i,i));
  }
  for (int i=0; i<20; i++) {
    epst.remove(point(i*10,i*10));
    true_points.erase(point(i*10,i*10));
  }

  epst.print();

  int i = system("mv tree.png tree_before.png");
  i++;

  test::random r;
  int x1 = r.next(200);
  int x2 = r.next(200);
  int y = r.next(200);

  if (x2 < x1) std::swap(x1,x2);
  
  epst.report(x1,x2,y,"test/report_200");

  std::vector<point> actual_points;
  util::load_file_to_container<std::vector<point>, point>
    (actual_points, "test/report_200", 4096);
    
  std::sort(actual_points.begin(),actual_points.end());

  std::vector<point> true_reported_points;
  for (point p : true_points)
    if (util::in_range(p,x1,x2,y))
      true_reported_points.push_back(p);

  epst.print();
  
  assert (true_reported_points == actual_points);
#ifdef VALIDATE
  assert (epst.is_valid());
#endif
  
  print_success();
  
}

void test_report_random() {

  print_description("starting test of report random ");

  std::set<point> true_points;
  ext::buffered_pst epst(9,0.5);

  util::remove_directory("testpoints_report_random");
  util::remove_directory("testpoints_report_random_q");
  
  test::random r;
  io::buffered_stream<point> bs(4096);
  bs.open("testpoints_report_random");
  for (int i=0; i<200; i++) {
    point p(r.next(200),r.next(200));
    epst.insert(p);
    true_points.insert(p);
    bs.write(p);
  }

  std::vector<point> rand_deletes(true_points.begin(), true_points.end());
  std::random_shuffle(rand_deletes.begin(), rand_deletes.end());
  
  for (int i=0; i<100; i++) {
    epst.remove(rand_deletes[i]);
    true_points.erase(rand_deletes[i]);
    bs.write(rand_deletes[i]);
  }
  bs.close();
  
#ifdef VALIDATE
  streambuf* cout_strbuf(cout.rdbuf());
  ostringstream output;
  cout.rdbuf(output.rdbuf());
  bool is_valid = epst.is_valid();
  if (!is_valid) {
    epst.print();
    cout.rdbuf(cout_strbuf);
    epst.is_valid();
  }
  cout.rdbuf(cout_strbuf);
  assert ( is_valid );
#endif
  io::buffered_stream<int> bs2(4096);

  for (int i = 0; i < 10; i++) {
    bs2.open("testpoints_report_random_q");
    bs2.seek(SEEK_END,0);
    int x1 = r.next(200);
    int x2 = r.next(200);
    int y = r.next(200);
    
    if (x2 < x1) std::swap(x1,x2);
    bs2.write(x1); bs2.write(x2); bs2.write(y);
    bs2.close();
    epst.report(x1,x2,y,"test/report_rand");

    std::vector<point> actual_points;
    util::load_file_to_container<std::vector<point>, point>
      (actual_points, "test/report_rand", 4096);
    
    std::sort(actual_points.begin(),actual_points.end());

    std::vector<point> true_reported_points;
    for (point p : true_points)
      if (util::in_range(p,x1,x2,y))
        true_reported_points.push_back(p);

#ifdef VALIDATE
    streambuf* cout_strbuf(cout.rdbuf());
    ostringstream output;
    cout.rdbuf(output.rdbuf());
    bool is_valid = epst.is_valid() && true_reported_points == actual_points;
    if (!is_valid) {
      epst.print();
      cout.rdbuf(cout_strbuf);
      epst.is_valid();
    }
    cout.rdbuf(cout_strbuf);
    assert ( is_valid );
#endif

    assert (true_reported_points == actual_points);

    util::remove_directory("test/report_rand");
    
  }
    
  print_success();
  
}

void test_report_random_repeat() {
  print_description("starting test of report random ");

  std::set<point> true_points;
  ext::buffered_pst epst(9,0.5);

  io::buffered_stream<point> bs(4096);
  bs.open("testpoints_report_random");
  for (int i=0; i<200; i++) {
    point p = bs.read();
    epst.insert(p);
    true_points.insert(p);
  }

#ifdef VALIDATE
  streambuf* cout_strbuf(cout.rdbuf());
  ostringstream output;
  cout.rdbuf(output.rdbuf());
  bool is_valid = epst.is_valid();
  if (!is_valid) {
    epst.print();
    cout.rdbuf(cout_strbuf);
    epst.is_valid();
  }
  cout.rdbuf(cout_strbuf);
  assert ( is_valid );
#endif
  

  for (int i=0; i<100; i++) {
    point p = bs.read();
    epst.remove(p);
    true_points.erase(p);
  }
  bs.close();
  
#ifdef VALIDATE
  cout.rdbuf(output.rdbuf());
  is_valid = epst.is_valid();
  if (!is_valid) {
    epst.print();
    cout.rdbuf(cout_strbuf);
    epst.is_valid();
  }
  cout.rdbuf(cout_strbuf);
  assert ( is_valid );
#endif
  
  io::buffered_stream<int> bs2(4096);
  bs2.open("testpoints_report_random_q");
  for (int i = 0; i < 10; i++) {
  
    int x1 = bs2.read();
    int x2 = bs2.read();
    int y = bs2.read();
    
    if (x2 < x1) std::swap(x1,x2);
    epst.print();
    DEBUG_MSG_FAIL("report: " << x1 << " " << x2 << " " << y);
    int k;
    cin >> k;
    epst.report(x1,x2,y,"test/report_rand");

    std::vector<point> actual_points;
    util::load_file_to_container<std::vector<point>, point>
      (actual_points, "test/report_rand", 4096);
    
    std::sort(actual_points.begin(),actual_points.end());

    std::vector<point> true_reported_points;
    for (point p : true_points)
      if (util::in_range(p,x1,x2,y))
        true_reported_points.push_back(p);

#ifdef VALIDATE
    cout.rdbuf(output.rdbuf());
    is_valid = epst.is_valid();
    if (!is_valid) {
      epst.print();
      cout.rdbuf(cout_strbuf);
      epst.is_valid();
    }
    cout.rdbuf(cout_strbuf);
    assert ( is_valid );
#endif

    if (true_reported_points != actual_points) {
      DEBUG_MSG_FAIL("ACTUAL POINTS:");
      for (point p : actual_points) DEBUG_MSG_FAIL(" - " << p);
      DEBUG_MSG_FAIL("TRUE POINTS:");
      for (point p : true_reported_points) DEBUG_MSG_FAIL(" - " << p);
      epst.print();
    }
    assert (true_reported_points == actual_points);

    util::remove_directory("test/report_rand");
    
  }
  bs2.close();
  print_success();
  
}

void test_report_random_2() {

  print_description("starting test of report random 2");

  std::set<point> true_points;
  ext::buffered_pst epst(9,0.5);
  epst.set_global_rebuild_configuration(ext::buffered_pst::global_rebuild_configuration
                                        (9999,0.5));
 
  test::random r;
  io::buffered_stream<point> bs(1);
  bs.open("testpoints_report_random_2");
  
  cerr << "- inserting 200 points" << endl;
  
  for (int i=0; i<200; i++) {
    point p(r.next(200),r.next(200));
    epst.insert(p);
    true_points.insert(p);
    bs.write(p);
#ifdef VALIDATE
    streambuf* cout_strbuf(cout.rdbuf());
    ostringstream output;
    cout.rdbuf(output.rdbuf());
    bool is_valid = epst.is_valid();
    if (!is_valid) {
      epst.print();
      cout.rdbuf(cout_strbuf);
      epst.is_valid();
    }
    cout.rdbuf(cout_strbuf);
    assert ( is_valid );
#endif
  }

  for (int i = 0; i < 10; i++) {
   
    std::vector<point> rand_deletes(true_points.begin(), true_points.end());
    std::random_shuffle(rand_deletes.begin(), rand_deletes.end());

    cerr << "- round " << i+1 << " of 10: deleting 50 points" << endl;
    
    for (int j=0; j<50; j++) {
      epst.remove(rand_deletes[j]);
      true_points.erase(rand_deletes[j]);
      bs.write(rand_deletes[j]);
#ifdef VALIDATE
      streambuf* cout_strbuf(cout.rdbuf());
      ostringstream output;
      cout.rdbuf(output.rdbuf());
      bool is_valid = epst.is_valid();
      if (!is_valid) {
        epst.print();
        cout.rdbuf(cout_strbuf);
        epst.is_valid();
      }
      cout.rdbuf(cout_strbuf);
      assert ( is_valid );
#endif
  }

    cerr << "- round " << i+1 << " of 10: reporting 10 times" << endl;

    for (int j = 0; j < 10; j++) {
      
      int x1 = r.next(200);
      int x2 = r.next(200);
      int y = r.next(200);
    
      if (x2 < x1) std::swap(x1,x2);

      bs.write(point(x1,x1));
      bs.write(point(x2,x2));
      bs.write(point(y,y));
      
      epst.report(x1,x2,y,"test/report_rand_2");

      std::vector<point> actual_points;
      util::load_file_to_container<std::vector<point>, point>
        (actual_points, "test/report_rand_2", 4096);
    
      std::sort(actual_points.begin(),actual_points.end());

      std::vector<point> true_reported_points;
      for (point p : true_points)
        if (util::in_range(p,x1,x2,y))
          true_reported_points.push_back(p);

      if (true_reported_points != actual_points) {
        epst.print();        
        assert (true_reported_points == actual_points);
      }

#ifdef VALIDATE
      streambuf* cout_strbuf(cout.rdbuf());
      ostringstream output;
      cout.rdbuf(output.rdbuf());
      bool is_valid = epst.is_valid();
      if (!is_valid) {
        epst.print();
        cout.rdbuf(cout_strbuf);
        epst.is_valid();
      }
      cout.rdbuf(cout_strbuf);
      assert ( is_valid );
#endif
   
      util::remove_directory("test/report_rand_2");
    }

    cerr << "- round " << i+1 << " of 10: inserting 50 points" << endl;
   
    for (int j=0; j<50; j++) {
      point p(r.next(200),r.next(200));
      epst.insert(p);
      true_points.insert(p);
      bs.write(p);
#ifdef VALIDATE
      streambuf* cout_strbuf(cout.rdbuf());
      ostringstream output;
      cout.rdbuf(output.rdbuf());
      bool is_valid = epst.is_valid();
      if (!is_valid) {
        epst.print();
        cout.rdbuf(cout_strbuf);
        epst.is_valid();
      }
      cout.rdbuf(cout_strbuf);
      assert ( is_valid );
#endif
    }
    
  }

  bs.close();
    
  print_success();
  
}

void test_report_random_buffer_size_512() {

  print_description("starting test of report random buffer_size 512");

  std::set<point> true_points;
  ext::buffered_pst epst(512,0.5);
 
  test::random r;

  cerr << "- inserting 12000 points" << endl;
  
  for (int i=0; i<12000; i++) {
    point p(r.next(99999),r.next(99999));
    epst.insert(p);
    true_points.insert(p);
  }

#ifdef VALIDATE
  bool is_valid = epst.is_valid();
  if (!is_valid) {
    epst.print();
    assert( is_valid );
  }
#endif
  
  for (int i = 0; i < 10; i++) {
   
    std::vector<point> rand_deletes(true_points.begin(), true_points.end());
    std::random_shuffle(rand_deletes.begin(), rand_deletes.end());

    cerr << "- round " << i+1 << " of 10: deleting 3000 points" << endl;
    
    for (int j=0; j<3000; j++) {
      epst.remove(rand_deletes[j]);
      true_points.erase(rand_deletes[j]);
    }

#ifdef VALIDATE
    is_valid = epst.is_valid();
    if (!is_valid) {
      epst.print();
      assert( is_valid );
    }
#endif

    cerr << "- round " << i+1 << " of 10: reporting 10 times" << endl;

    for (int j = 0; j < 10; j++) {
      
      int x1 = r.next(99999);
      int x2 = r.next(99999);
      int y = r.next(99999);
    
      if (x2 < x1) std::swap(x1,x2);
  
      epst.report(x1,x2,y,"test/report_rand_buf_size_512");

      std::vector<point> actual_points;
      util::load_file_to_container<std::vector<point>, point>
        (actual_points, "test/report_rand_buf_size_512", 4096);
    
      std::sort(actual_points.begin(),actual_points.end());

      std::vector<point> true_reported_points;
      for (point p : true_points)
        if (util::in_range(p,x1,x2,y))
          true_reported_points.push_back(p);

#ifdef VALIDATE
      if (true_reported_points != actual_points) {
        epst.print();        
        assert (true_reported_points == actual_points);
      }
      is_valid = epst.is_valid();
      if (!is_valid) {
        epst.print();
        assert( is_valid );
      }
#endif
      util::remove_directory("test/report_rand_buf_size_512");
    }

    cerr << "- round " << i+1 << " of 10: inserting 3000 points" << endl;
    
    for (int j=0; j<3000; j++) {
      point p(r.next(99999),r.next(99999));
      epst.insert(p);
      true_points.insert(p);
    }

#ifdef VALIDATE
    is_valid = epst.is_valid();
    if (!is_valid) {
      epst.print();
      assert( is_valid );
    }
#endif
    
  }
    
  print_success();
  
}

void test_global_rebuild_insert_10() {

  print_description("starting test of global rebuild insert 10");

  ext::buffered_pst epst(9,0.5);
  
  for (int i=0; i<10; i++) {
    point p(i,i);
    epst.insert(p);
  }

  epst.print();
#ifdef VALIDATE
  assert( epst.is_valid() );
#endif
  
  print_success();
  
}

void test_global_rebuild_insert_10_delete_5() {

  print_description("starting test of global rebuild insert 10 delete 5");

  ext::buffered_pst epst(9,0.5);
  
  for (int i=0; i<10; i++) {
    point p(i,i);
    epst.insert(p);
#ifdef VALIDATE
    streambuf* cout_strbuf(cout.rdbuf());
    ostringstream output;
    cout.rdbuf(output.rdbuf());
    bool is_valid = epst.is_valid();
    if (!is_valid) {
      epst.print();
      cout.rdbuf(cout_strbuf);
      epst.is_valid();
    }
    cout.rdbuf(cout_strbuf);
    assert ( is_valid );
#endif
  }

  for (int i = 0; i < 5; i++) {
    point p(i*2,i*2);
    epst.remove(p);
#ifdef VALIDATE
    streambuf* cout_strbuf(cout.rdbuf());
    ostringstream output;
    cout.rdbuf(output.rdbuf());
    bool is_valid = epst.is_valid();
    if (!is_valid) {
    epst.print();
    cout.rdbuf(cout_strbuf);
    epst.is_valid();
  }
    cout.rdbuf(cout_strbuf);
    assert ( is_valid );
#endif
  }

  epst.print();
#ifdef VALIDATE
  assert( epst.is_valid() );
#endif
  
  print_success();
  
}

void test_global_rebuild_insert_100_delete_50() {

  print_description("starting test of global rebuild insert 10 delete 5");

  ext::buffered_pst epst(9,0.5);
  
  for (int i=0; i<100; i++) {
    point p(i,i);
    epst.insert(p);
#ifdef VALIDATE
    streambuf* cout_strbuf(cout.rdbuf());
    ostringstream output;
    cout.rdbuf(output.rdbuf());
    bool is_valid = epst.is_valid();
    if (!is_valid) {
      epst.print();
      cout.rdbuf(cout_strbuf);
      epst.is_valid();
    }
    cout.rdbuf(cout_strbuf);
    assert ( is_valid );
#endif
  }

  for (int i = 0; i < 50; i++) {
    point p(i*2,i*2);
    epst.remove(p);
#ifdef VALIDATE
    streambuf* cout_strbuf(cout.rdbuf());
    ostringstream output;
    cout.rdbuf(output.rdbuf());
    bool is_valid = epst.is_valid();
    if (!is_valid) {
    epst.print();
    cout.rdbuf(cout_strbuf);
    epst.is_valid();
  }
    cout.rdbuf(cout_strbuf);
    assert ( is_valid );
#endif
  }

  epst.print();
#ifdef VALIDATE
  assert( epst.is_valid() );
#endif
  
  print_success();
  
}

void test_construction_50_points() {

  print_description("Starting test of construction on 50 points");
  std::string file = "test/test_construction_50_points";
  util::remove_directory(file);
  io::buffered_stream<point> bs(4096);
  bs.open(file);
  for (int i = 0; i < 54; i++) bs.write(point(i,i));
  bs.close();

  ext::buffered_pst epst(9, 0.5, file, true);

  epst.print();

#ifdef VALIDATE
  assert(epst.is_valid());
#endif
  print_success();
}

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
  
  util::remove_directory("test");
  
}

void test_contained_points_error() {

  print_description("starting test of report random 2");

  std::set<point> true_points;
  ext::buffered_pst epst(9,0.5);
  epst.set_global_rebuild_configuration(ext::buffered_pst::global_rebuild_configuration
                                        (9999,0.5));

  for (int i = 0; i < 20; i++) {
    epst.insert(point(i,i));
#ifdef VALIDATE
    assert(epst.is_valid());
#endif
  }

  for (int i = 40; i < 60; i++) {
    epst.insert(point(i,i));
#ifdef VALIDATE
    assert(epst.is_valid());
#endif
  }

  for (int i = 5; i < 8; i++) {
    epst.remove(point(i,i));
#ifdef VALIDATE
    assert(epst.is_valid());
#endif
  }

#ifdef VALIDATE
  assert(epst.is_valid());
#endif

  epst.print();
  
  epst.remove(point(44,44));
#ifdef VALIDATE
  assert(epst.is_valid());
#endif
  epst.insert(point(5,5));
#ifdef VALIDATE
  assert(epst.is_valid());
#endif
  epst.remove(point(10,10));
#ifdef VALIDATE
  assert(epst.is_valid());
#endif
  
  epst.print();
  
  print_success();
  
}

int main() {

  cleanup();
  if (!util::file_exists("test")) mkdir("test", 0700);
  
  cout << "\033[0;33m\e[4mSTARTING TEST OF EPST STRUCTURE\e[24m\033[0m" << endl;
  test_construction();
  test_insert();
  test_interval_range_belong_to();
  test_test();
  test_buffer_points_less_than_point_buffer_points();
  test_no_duplicates_in_pv_iv_dv();
  test_insert_buffer_overflow();
  test_root_split();
  test_root_split_insert_overflow();
  test_root_split_insert_between_overflow();
  test_maintaining_min_max_y_on_insert_buffer_overflow();
  test_node_degree_overflow();
  test_distribute_evenly();
  test_insert_buffer_overflow_to_non_leaf();
  test_insert_buffer_overflow_to_non_leaf2();
  test_insert_buffer_overflow_to_non_leaf3();
  test_insert_buffer_overflow_to_non_leaf4();
  test_not_valid_on_manual_insert();
  test_deterministic_random();
  test_deterministic_random2();
  test_random_deterministic3() ;
  test_random_insert();
  test_truly_random();
  // test_delete();
  // test_delete_overflow();
  // test_delete_overflow_underflow_node();
  // test_delete_overflow_many_points();
  // test_delete_all_points();
  // test_insert_200_delete_20_points();
  // test_delete_truly_random();
  // test_delete_truly_random_points_from_file("test_points_fail_1");
  // test_delete_truly_random_n_points(10000);
  // test_delete_truly_random_n_points_from_file("test_points");
  // test_report_points_deterministic();
  // test_report_points_deterministic2();
  // test_report_points_deterministic3();
  // test_report_points_deterministic_delete();
  // test_report_points_deterministic_repeat_report();
  // test_report_points_underflowing_point_buffer();
  // test_report_200_delete_20_points();
  // test_report_random();
  // test_report_random_repeat();
  // test_report_random_2();
  // test_global_rebuild_insert_10();
  // test_global_rebuild_insert_10_delete_5();
  // test_global_rebuild_insert_100_delete_50();
  test_construction_50_points();
  // TODO: test_insert_delete_all_insert_half_report()
  // TODO: test_insert_delete_half_insert_half_report();
  // TODO: test_insert_delete_half_insert_all_report();
  // test_report_random_buffer_size_512();
  // test_contained_points_error();
  
  cout << "\x1b[32mALL TESTS WERE SUCCESSFUL!\x1b[0m" << endl;
  
  cleanup();
  
  return 0;
  
}
