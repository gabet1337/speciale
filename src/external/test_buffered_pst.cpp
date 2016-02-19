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

int main() {

  cout << "\033[0;33m\e[4mSTARTING TEST OF CHILD STRUCTURE\e[24m\033[0m" << endl;
  
  test_test();
  test_construction();
  test_insert();
  
  cout << "\x1b[32mALL TESTS WERE SUCCESSFUL!\x1b[0m" << endl;
  return 0;
  
}
