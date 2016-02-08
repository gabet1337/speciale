#include <iostream>
#include <assert.h>
#include <fstream>
#include "stream.hpp"
#include "../common/point.hpp"
#include <string>
using namespace std;

void test_open_close() {
  io::buffered_stream<char> bs(1);
  bs.open("stream/testfiles/test1.txt");
  bs.close();
}

void test_read() {
  io::buffered_stream<char> bs(1);
  bs.open("stream/testfiles/test1.txt");

  string true_string = "Hell";
  string actual_string;
  for (int i = 0; i < 4; i++) actual_string += bs.read();
  assert( (true_string == actual_string) && "Stream didn't read correct");
}

void test_write() {
  io::buffered_stream<char> bs(1);
  bs.open("stream/testfiles/test_out.txt");
  string true_string = "Hello World!";
  for (char c : true_string) bs.write(c);
  bs.close();

  ifstream file;
  file.open("stream/testfiles/test_out.txt");
  string actual_string;
  while (file >> actual_string);
  file.close();
  assert( (actual_string == true_string) && "Stream didn't write correctly");

}

void test_read_point() {
  io::buffered_stream<point> bs(1);
  // bs.open(
}

int main() {
  test_open_close();
  test_read();
  cout << "\x1b[32mALL TESTS WERE SUCCESSFUL!\x1b[0m" << endl;

  return 0;
}
