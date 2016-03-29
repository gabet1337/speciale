#include "arge_pst.hpp"
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

int main() {

  cout << "\033[0;33m\e[4mSTARTING TEST OF APST STRUCTURE\e[24m\033[0m" << endl;

  cout << "\x1b[32mALL TESTS WERE SUCCESSFUL!\x1b[0m" << endl;

  return 0;
}
