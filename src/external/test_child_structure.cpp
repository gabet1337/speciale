#include <iostream>
#include "../common/debug.hpp"
#include "../common/point.hpp"
#include "child_structure.hpp"
#include <assert.h>
using namespace std;

void test_constructor() {
  cout << "starting test_constructor ";
  cout << sizeof(size_t) << " " << sizeof(double) <<  " " << sizeof(point) <<endl;
  ext::child_structure cs(1337, 6, 0.5);

  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_valid_after_constructor() {
  cout << "starting test of validity in memory ";
  ext::child_structure cs(1,4,0.5);
  assert((cs.valid_memory() == true) && "Structure not valid in memory");
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_valid_after_destructor() {
  cout << "starting test of validity on disk ";
  ext::child_structure* cs = new ext::child_structure(2,4,0.5);
  delete cs;
  cs = new ext::child_structure(2);
  assert( (cs->valid_disk() == true) && "Structure not valid on disk");
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}


void clean_up() {
  int lol =system("rm -rf c_1");
  lol = system("rm -rf c_2");
  lol++;
}

int main() {
  test_valid_after_constructor();
  test_valid_after_destructor();

  clean_up();
  cout << "\x1b[32mALL TESTS WERE SUCCESSFUL!\x1b[0m" << endl;
  return 0;
}

            
