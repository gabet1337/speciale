#include <iostream>
#include <assert.h>
#include "dynamic_pst.hpp"
#include "../common/utilities.hpp"
#include "../common/pst_interface.hpp"
#include "../stream/stream.hpp"
#include <iomanip>
using namespace std;

typedef internal::DynamicPrioritySearchTree dpst;

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

  dpst.remove(point(10,10));
  dpst.remove(point(15,15));
  dpst.remove(point(20,20));
  
  dpst.print();
  
  print_success();
}


int main() {
  cout << "\033[0;33m\e[4mSTARTING TEST OF INTERNAL DYNAMIC PST\e[24m\033[0m" << endl;
  //test_insert();
  //test_report();
  test_remove_report();
  cout << "\x1b[32mALL TESTS WERE SUCCESSFUL!\x1b[0m" << endl;

  return 0;
}
