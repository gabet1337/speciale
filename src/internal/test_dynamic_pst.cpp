#include <iostream>
#include <assert.h>
#include "dynamic_pst.hpp"
#include "../common/utilities.hpp"
#include "../common/pst_interface.hpp"
#include "../stream/stream.hpp"
#include "../common/test_lib.hpp"

#include <iomanip>
using namespace std;

typedef internal::dynamic_pst dpst;

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
  print_description("Starting to test insert in internal dynamic pst");

  dpst dpst;
  dpst.insert(point(10,10));
  dpst.insert(point(12,12));
  dpst.insert(point(9,9));
  dpst.insert(point(15,15));
  dpst.insert(point(2,2));
  dpst.insert(point(14,14));
  dpst.insert(point(20,20));
  
  dpst.print();
  
  print_success();
}

void test_report() {
  print_description("Starting to test report of internal dynamic pst");

  dpst dpst;

  dpst.insert(point(10,10));
  dpst.insert(point(12,12));
  dpst.insert(point(9,9));
  dpst.insert(point(15,15));
  dpst.insert(point(2,2));
  dpst.insert(point(14,14));
  dpst.insert(point(20,20));

  dpst.remove(point(10,10));
  
  dpst.print();

  dpst.report(9, 15, 9, "test/dpst_result");

  io::buffered_stream<point> bs(4096);
  bs.open("test/dpst_result");
  while (!bs.eof())
    std::cout << bs.read() << std::endl;
  bs.close();
  util::remove_file("test/dpst_result");

  print_success();
}

void test_remove_report() {
  print_description("Starting to test remove in internal dynamic pst");

  dpst dpst;
  dpst.insert(point(10,10));
  dpst.insert(point(12,12));
  dpst.insert(point(9,9));
  dpst.insert(point(15,15));
  dpst.insert(point(2,2));
  dpst.insert(point(14,14));
  dpst.insert(point(20,20));

  //dpst.remove(point(10,10));
  //dpst.remove(point(15,15));
  //dpst.remove(point(20,20));
  
  dpst.print();
  
  print_success();
}

void test_construct() {
  print_description("Starting to test linear construct");

  std::vector<point> points;
  points.push_back(point(1,1));
  points.push_back(point(2,2));
  points.push_back(point(3,3));
  points.push_back(point(4,4));
  points.push_back(point(5,5));
  points.push_back(point(6,6));
  points.push_back(point(7,7));
  points.push_back(point(8,8));
  points.push_back(point(9,9));
  points.push_back(point(10,10));
  
  dpst dpst;
  dpst.construct_sorted(points);
  dpst.global_rebuild();
  
  //dpst.remove(point(10,10));
  //dpst.remove(point(15,15));
  //dpst.remove(point(20,20));
  
  dpst.print();
  
  print_success();
}

void test_report_random_1gb(size_t buffer_size, double epsilon) {

  print_description("starting test of report random 1gb");

  dpst epst;
  epst.set_global_rebuild_configuration(dpst::global_rebuild_configuration(INF,0.5));
  
  test::random r;
  
  io::buffered_stream<point> bs(4096);
  bs.open("test_data_report_random_1gb");
  
  cerr << "- inserting " << 93*1024*1024 << " points" << endl;
  
  for (size_t i = 0; i < 8*1024*1024; i++) { // insert 744 MB (93)
    epst.insert(bs.read());
    //res.insert(p);
    if (i > 0 && i % (128*1024) == 0) {
      cout << " - inserted " << i / (128*1024) << " MB" << endl;
    }
  }

  for (int i = 0; i < 1; i++) {
   
    cerr << "- round " << i+1 << " of 1: deleting " << 23*1024*1024 << " points" << endl;
    
    for (int j = 0; j < 4*1024*1024; j++) { // remove 128 MB
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
  
    for (int j = 0; j < 4*1024*1024; j++) { // insert 128 MB (16)
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

void cleanup() {
  util::remove_file("test/dpst_result");
  util::remove_file("test/report_rand_1gb");
}

int main() {
  cout << "\033[0;33m\e[4mSTARTING TEST OF INTERNAL DYNAMIC PST\e[24m\033[0m" << endl;
  cleanup();
  //test_insert();
  //test_remove_report();
  //test_report();
  test_report_random_1gb(4096, 0.5);

  //test_construct();
  cout << "\x1b[32mALL TESTS WERE SUCCESSFUL!\x1b[0m" << endl;

  return 0;
}
