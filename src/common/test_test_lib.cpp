#include <iostream>
#include <assert.h>
#include "utilities.hpp"
#include "test_lib.hpp"
#include <iomanip>
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

void test_1_line() {

  test::gnuplot gp;
  // gp.add_line("test", gp.STYLE::ARGE, "test_data_file", 1, 3);
  // gp.add_line("test", gp.STYLE::ARGE, "test_data_file", 1, 3);
  gp.add_function("sin(x)", common::PST_VARIANT::ARGE, "sin(x)");
  cout << gp.generate_script() << endl;
  //gp.output_plot();
}

void test_proc_io() {
  test::proc_io pi;
  io::buffered_stream<int> lol(1);
  lol.open("lol");
  for (int i = 0; i < 1337; i++) lol.write(i);
  lol.close();
  cout << pi.total_ios() << endl;
}

void test_available_line() {
  test::gnuplot gp;
  cout << gp.get_available_style(common::PST_VARIANT::ARGE) << endl;
  cout << gp.get_available_style(common::PST_VARIANT::ARGE) << endl;
  cout << gp.get_available_style(common::PST_VARIANT::ARGE) << endl;
  cout << gp.get_available_style(common::PST_VARIANT::ARGE) << endl;
  cout << gp.get_available_style(common::PST_VARIANT::ARGE) << endl;
  cout << gp.get_available_style(common::PST_VARIANT::ARGE) << endl;
  cout << gp.get_available_style(common::PST_VARIANT::ARGE) << endl;
  cout << gp.get_available_style(common::PST_VARIANT::ARGE) << endl;
  cout << gp.get_available_style(common::PST_VARIANT::ARGE) << endl;
  cout << gp.get_available_style(common::PST_VARIANT::ARGE) << endl;
}

int main() {
  cout << "\033[0;33m\e[4mSTARTING TEST OF TEST LIB\e[24m\033[0m" << endl;

  //test_1_line();
  //test_proc_io();
  test_available_line();

  cout << "\x1b[32mALL TESTS WERE SUCCESSFUL!\x1b[0m" << endl;

  return 0;
}
