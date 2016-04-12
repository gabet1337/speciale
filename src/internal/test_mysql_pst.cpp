#include <iostream>
#include <assert.h>
#include "mysql_pst.hpp"
#include "../common/utilities.hpp"
#include "../stream/stream.hpp"
#include <iomanip>
using namespace std;

typedef internal::mysql_pst mpst;

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
  print_description("Starting to test insert in mysql pst");
  mpst r;
  for (int i = 0; i < 10; i++)
    r.insert(point(i,i));
  print_success();
}

void test_report() {
  print_description("Starting to test report of mysql pst");
  util::remove_file("test/mysql_report");
  mpst r(1024,0);
  for (int i = 0; i < 10; i++)
    r.insert(point(i,i));
  r.report(3,6,4,"test/mysql_report");
  std::vector<point> actual_result;
  std::vector<point> true_result { point(4,4), point(5,5), point(6,6) };
  io::buffered_stream<point> bs(1024);
  bs.open("test/mysql_report");
  while (!bs.eof()) actual_result.push_back(bs.read());
  bs.close();
  assert(actual_result == true_result);
  print_success();
}

void test_remove_report() {
  print_description("Starting to test remove in mysql pst");
  util::remove_file("test/mysql_report");
  mpst r(1024,0);
  for (int i = 0; i < 10; i++)
    r.insert(point(i,i));
  r.remove(point(5,5));
  r.report(3,6,4,"test/mysql_report");
  std::vector<point> actual_result;
  std::vector<point> true_result { point(4,4), point(6,6) };
  io::buffered_stream<point> bs(1024);
  bs.open("test/mysql_report");
  while (!bs.eof()) actual_result.push_back(bs.read());
  bs.close();
  assert(actual_result == true_result);
  print_success();
}


int main() {
  cout << "\033[0;33m\e[4mSTARTING TEST OF MYSQL PST\e[24m\033[0m" << endl;
  test_insert();
  test_report();
  test_remove_report();
  cout << "\x1b[32mALL TESTS WERE SUCCESSFUL!\x1b[0m" << endl;

  return 0;
}
