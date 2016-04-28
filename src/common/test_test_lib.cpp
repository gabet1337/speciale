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
  test::drop_cache();
  util::remove_file("lol");
  util::remove_file("lol2");
  test::proc_io pi;
  io::buffered_stream<int> lol(1), lol2(2);
  lol.open("lol");
  lol2.open("lol2");
  for (int i = 0; i < 700; i++) lol.write(i);
  lol.close();
  pi.restart();
  for (int i = 0; i < 700; i++) lol2.write(i);

  lol2.close();
  std::cout << pi.total_ios() << std::endl;
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

void test_beeper() {
  test::beeper bp;
  while (true)
    bp.mario();
}

int main() {
  cout << "\033[0;33m\e[4mSTARTING TEST OF TEST LIB\e[24m\033[0m" << endl;

  //test_1_line();
  test_proc_io();
  //test_available_line();
  //test_beeper();
  

  cout << "\x1b[32mALL TESTS WERE SUCCESSFUL!\x1b[0m" << endl;

  return 0;
}
