#include <iostream>
#include <assert.h>
#include <fstream>
#include "stream.hpp"
#include "../common/point.hpp"
#include <string>
using namespace std;

void test_open_close() {

  cout << "starting test_open_close ";
  
  io::buffered_stream<char> bs(1);
  bs.open("stream/testfiles/test1.txt");
  bs.close();

  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_read() {

  cout << "starting test_read ";
  
  io::buffered_stream<char> bs(1);
  bs.open("stream/testfiles/test1.txt");
  string true_string = "Hell";
  string actual_string;
  for (int i = 0; i < 4; i++) actual_string += bs.read();
  bs.close();
  assert( (true_string == actual_string) && "Stream didn't read correct");
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_write() {

  cout << "starting test_write ";
  
  io::buffered_stream<char> bs(1);
  bs.open("stream/testfiles/test_out.txt");
  string true_string = "Hello World!";
  for (char c : true_string) bs.write(c);
  bs.close();

  ifstream file;
  file.open("stream/testfiles/test_out.txt");
  string actual_string;
  getline(file, actual_string);
  file.close();
  assert( (actual_string == true_string) && "Stream didn't write correctly");
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_read_write2() {
  cout << "starting test_read_write2 ";
  system("rm stream/testfiles/test_read_write3.txt");
  io::buffered_stream<char> bs(10);
  
  bs.open("stream/testfiles/test_read_write3.txt");
  string true_string = "short";
  for (char c : true_string) bs.write(c);
  bs.close();
  string actual_string;
  bs.open("stream/testfiles/test_read_write3.txt");
  while (!bs.eof()) actual_string += bs.read();
  assert( (actual_string == true_string) && "Unequal strings");

  for (char c : true_string) bs.write(c);
  bs.close();
  
  bs.open("stream/testfiles/test_read_write3.txt");
  actual_string = "";
  while (!bs.eof()) actual_string+=bs.read();
  true_string.append(true_string);
  assert( (actual_string == true_string) && "Unequal strings");

  for (char c : true_string) bs.write(c);
  
  bs.close();

  bs.open("stream/testfiles/test_read_write3.txt");
  actual_string = "";
  while (!bs.eof()) actual_string+=bs.read();
  true_string.append(true_string);
  assert( (actual_string == true_string) && "Unequal strings");
  bs.close();  
  
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}


void test_interleave_read_write() {

  cout << "starting test_read_write ";
  
  io::buffered_stream<char> bs(2);
  bs.open("stream/testfiles/test_interleaved.txt");
  string true_string = "Hello World";
  for (size_t i = 0; i < true_string.size(); i++)
    if (i % 2)
      assert ( (bs.read() == true_string[i]) && "Stream didn't read correctly");
    else bs.write(true_string[i]);
  bs.close();
  
  ifstream file;
  file.open("stream/testfiles/test_interleaved.txt");
  string actual_string;
  getline(file, actual_string);
  file.close();
  assert( (actual_string == true_string) && "Stream didn't write correctly");
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_read_point() {

  cout << "starting test_read_point ";

  system("rm stream/testfiles/test_points.txt");
  
  io::buffered_stream<point> bs(1);
  bs.open("stream/testfiles/test_points.txt");
  bs.write(point(1,2));
  bs.write(point(3,2));
  bs.close();
  bs.open("stream/testfiles/test_points.txt");
  assert ( (point(1,2) == bs.read() && point(3,2) == bs.read()) && "Not the correct point");
  bs.close();
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_file_size() {

  cout << "starting test_file_size ";

  system("rm stream/testfiles/test_size.txt");
  
  io::buffered_stream<point> bs(1);
  bs.open("stream/testfiles/test_size.txt");
  bs.write(point(1,2));
  bs.write(point(3,2));
  bs.close();

  bs.open("stream/testfiles/test_size.txt");
  assert ( (bs.size() == 16) && "Not the right size");
  bs.close();
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}


int main() {
  
  test_open_close();
  test_read();
  test_write();
  test_interleave_read_write();
  test_read_point();
  test_file_size();
  test_read_write2();

  cout << "\x1b[32mALL TESTS WERE SUCCESSFUL!\x1b[0m" << endl;

  return 0;
}
