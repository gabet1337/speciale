#include "libspatial_pst.hpp"
#include <iostream>
#include <assert.h>
#include "../common/definitions.hpp"
#include "../common/utilities.hpp"
#include "../common/test_lib.hpp"
#include "../stream/stream.hpp"
using namespace std;

typedef ext::libspatial_pst lsp;

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


void test_insert() {
  print_description("Starting to test insert in boost r tree");
  lsp r(1024);
  for (int i = 0; i < 10; i++)
    r.insert(point(i,i));
  print_success();
}

void test_report() {
  print_description("Starting to test report of boost in boost r tree");
  util::remove_file("test/libspatial_report");
  lsp r(1024,0);
  for (int i = 0; i < 10; i++)
    r.insert(point(i,i));
  r.report(3,6,4,"test/libspatial_report");
  std::vector<point> actual_result;
  std::vector<point> true_result { point(4,4), point(5,5), point(6,6) };
  io::buffered_stream<point> bs(1024);
  bs.open("test/libspatial_report");
  while (!bs.eof()) actual_result.push_back(bs.read());
  bs.close();
  assert(actual_result == true_result);
  print_success();
}

void test_remove_report() {
  print_description("Starting to test remove in boost r tree");
  util::remove_file("test/libspatial_report");
  lsp r(1024,0);
  for (int i = 0; i < 10; i++)
    r.insert(point(i,i));
  r.remove(point(5,5));
  r.report(3,6,4,"test/libspatial_report");
  std::vector<point> actual_result;
  std::vector<point> true_result { point(4,4), point(6,6) };
  io::buffered_stream<point> bs(1024);
  bs.open("test/libspatial_report");
  while (!bs.eof()) actual_result.push_back(bs.read());
  bs.close();
  assert(actual_result == true_result);
  print_success();
}

void test_report_random_1gb(size_t buffer_size, double epsilon) {

  print_description("starting test of report random 1gb");

  lsp epst(buffer_size, epsilon);
  test::random r;
  
  io::buffered_stream<point> bs(4096);
  bs.open("data/insert_experiment");
  
  cerr << "- inserting " << 93*1024*1024 << " points" << endl;
  
  for (size_t i = 0; i < 93*1024*1024; i++) { // insert 744 MB
    epst.insert(bs.read());
    //res.insert(p);
    if (i > 0 && i % (128*1024) == 0) {
      cout << " - inserted " << i / (128*1024) << " MB" << endl;
    }
  }

  for (int i = 0; i < 1; i++) {
   
    cerr << "- round " << i+1 << " of 1: deleting " << 23*1024*1024 << " points" << endl;
    
    for (int j = 0; j < 16*1024*1024; j++) { // remove 128 MB
      epst.remove(bs.read());
    }
    
    cerr << "- round " << i+1 << " of 1: reporting 1 times" << endl;

    for (int j = 0; j < 1; j++) {

      int x1 = r.next();
      int x2 = r.next();
      int y = r.next();
      
      if (x2 < x1) std::swap(x1,x2);

      epst.report(x1,x2,y,"test/report_rand_1gb");
      util::remove_directory("test/report_rand_1gb");
    }
    
    cerr << "- round " << i+1 << " of 1: inserting " << 23*1024*1024 << " points" << endl;
  
    for (int j = 0; j < 16*1024*1024; j++) { // insert 128 MB
      epst.insert(bs.read());
    }

    for (int j = 0; j < 1; j++) {

      int x1 = r.next();
      int x2 = r.next();
      int y = r.next();
      
      if (x2 < x1) std::swap(x1,x2);
      
      epst.report(x1,x2,y,"test/report_rand_1gb");
      util::remove_directory("test/report_rand_1gb");
      
    }
    
  }
  
  bs.close();

  print_success();

}

void test_duplicates() {

  lsp r(1024);

  for (int i = 0; i < 10; i++)
    r.insert(point(i,i));

  for (int i = 0; i < 10; i++)
    r.insert(point(i,i));
    
  r.report(-100,100,-100,"test/report_lpst_3");
  
  print_success();
  
}

int main() {
  cout << "\033[0;33m\e[4mSTARTING TEST OF LIBSPATIAL R-TREE\e[24m\033[0m" << endl;
  test_insert();
  test_report();
  test_remove_report();

  //test_report_random_1gb(4096, 0);

  test_duplicates();
  
  cout << "\x1b[32mALL TESTS WERE SUCCESSFUL!\x1b[0m" << endl;

  return 0;
}
