#include <iostream>
#include <assert.h>
#include <fstream>
#include "stream.hpp"
#include "../common/point.hpp"
#include "../common/debug.hpp"
#include <string>
using namespace std;

void delete_file(string file) {
  string command = "rm -f stream/testfiles/"+file;
  if (system(command.c_str()) != 0)
    perror(("Unable to delete file"+file).c_str());
}

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
  delete_file("test_read_write3.txt");
  io::buffered_stream<char> bs(10);
  
  bs.open("stream/testfiles/test_read_write3.txt");
  string true_string = "short";
  for (char c : true_string) bs.write(c);
  bs.close();
  string actual_string;
  bs.open("stream/testfiles/test_read_write3.txt");
  while (!bs.eof()) actual_string += bs.read();
  DEBUG_MSG(actual_string << " " << true_string);
  assert( (actual_string == true_string) && "Unequal strings");

  for (char c : true_string) bs.write(c);
  bs.close();
  
  bs.open("stream/testfiles/test_read_write3.txt");
  actual_string = "";
  while (!bs.eof()) actual_string+=bs.read();
  true_string.append(true_string);
  DEBUG_MSG(actual_string << " " << true_string);
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

  delete_file("test_points.txt");
  
  io::buffered_stream<point> bs(1);
  bs.open("stream/testfiles/test_points.txt");
  bs.write(point(1,2));
  bs.write(point(3,2));
  bs.close();
  bs.open("stream/testfiles/test_points.txt");
  point p1 = bs.read(), p2 = bs.read();
  assert ( (point(1,2) == p1 && point(3,2) == p2) && "Not the correct point");
  bs.close();
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_modify_first_point() {

  cout << "starting test_modify_first_point ";

  delete_file("test_modify_point.dat");

  io::buffered_stream<point> bs(2);
  bs.open("stream/testfiles/test_modify_point.dat");
  bs.write(point(89,56));
  bs.write(point(458,54));
  bs.close();

  bs.open("stream/testfiles/test_modify_point.dat");
  assert ( (point(89,56) == bs.read() && point(458,54) == bs.read()) && "Not the correct point");
  bs.close();

  bs.open("stream/testfiles/test_modify_point.dat");
  bs.write(point(1,2));
  bs.close();

  bs.open("stream/testfiles/test_modify_point.dat");
  assert ( (point(1,2) == bs.read() && point(458,54) == bs.read()) && "Not the correct point");
  bs.close();

  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_modify_second_point() {

  cout << "starting test_modify_second_point ";

  delete_file("test_modify_point.dat");

  io::buffered_stream<point> bs(2);
  bs.open("stream/testfiles/test_modify_point.dat");
  bs.write(point(89,56));
  bs.write(point(458,54));
  bs.close();

  bs.open("stream/testfiles/test_modify_point.dat");
  assert ( (point(89,56) == bs.read() && point(458,54) == bs.read()) && "Not the correct point");
  bs.close();

  bs.open("stream/testfiles/test_modify_point.dat");
  bs.read();
  bs.write(point(1,2));
  bs.close();

  bs.open("stream/testfiles/test_modify_point.dat");
  assert ( (point(89,56) == bs.read() && point(1,2) == bs.read()) && "Not the correct point");
  bs.close();

  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_seek_read_point_outside_buffer_range() {

  // Read point outside buffer range
  // 1. Write 8 points.
  // 2. Seek to start of file and read point.
  // 3. Seek to middle of file and read point.

  cout << "starting test_seek_read_point_outside_buffer_range ";

  delete_file("test_seek_read_point_outside_buffer_range.dat");

  // 1. Write 8 points.
  io::buffered_stream<point> bs(2);
  bs.open("stream/testfiles/test_seek_read_point_outside_buffer_range.dat");
  bs.write(point(89,56));
  bs.write(point(458,54));
  bs.write(point(48,41));
  bs.write(point(8,23));
  bs.write(point(356,213));
  bs.write(point(123,56));
  bs.write(point(98,234));
  bs.write(point(123,45));

  // 2. Seek to start of file and read point
  bs.seek(0,SEEK_SET);
  assert( (point(89,56) == bs.read()) && "Invalid point read");

  // 3. Seek to middle of file and read point
  bs.seek(32,SEEK_SET);
  assert( (point(356,213) == bs.read()) && "Invalid point read");
  bs.close();

  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
  
}

void test_seek_read_point_in_buffer_range() {
  
  // Read point in buffer range
  // 1. Write 8 points.
  // 2. Seek to start of file and read point.
  // 3. Seek to middle of file and read point.

  cout << "starting test_seek_read_point_inside_buffer_range ";

  delete_file("test_seek_read_point_inside_buffer_range.dat");

  // 1. Write 8 points.
  io::buffered_stream<point> bs(4);
  bs.open("stream/testfiles/test_seek_read_point_inside_buffer_range.dat");
  bs.write(point(89,56));
  bs.write(point(458,54));
  bs.write(point(48,41));
  bs.write(point(8,23));
  bs.write(point(356,213));
  bs.write(point(123,56));
  bs.write(point(98,234));
  bs.write(point(123,45));

  // 2. Seek to start of file and read point
  bs.seek(8,SEEK_SET);
  assert( (point(458,54) == bs.read()) && "Invalid point read");

  // 3. Seek to middle of file and read point
  bs.seek(24,SEEK_SET);
  assert( (point(8,23) == bs.read()) && "Invalid point read");
  bs.close();

  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
  
}

void test_seek_modify_point_in_buffer_range() {
  
  // Modify point in buffer range
  // 1. Write 8 points.
  // 2. Read second point.
  // 3. Modify second point.
  // 4. Read second point.

  cout << "starting test_seek_modify_point_inside_buffer_range ";

  delete_file("test_seek_modify_point_inside_buffer_range.dat");

  // 1. Write 8 points.
  io::buffered_stream<point> bs(4);
  bs.open("stream/testfiles/test_seek_modify_point_inside_buffer_range.dat");
  bs.write(point(89,56));
  bs.write(point(458,54));
  bs.write(point(48,41));
  bs.write(point(8,23));
  bs.write(point(356,213));
  bs.write(point(123,56));
  bs.write(point(98,234));
  bs.write(point(123,45));

  // 2. Seek to start of file and read point
  bs.seek(8,SEEK_SET);
  assert( (point(458,54) == bs.read()) && "Invalid point read");

  // 3. Seek to middle of file and read point
  bs.seek(8,SEEK_SET);
  bs.write(point(777,888));

  bs.seek(8,SEEK_SET);
  assert( (point(777,888) == bs.read()) && "Invalid point read");
  bs.close();

  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
  
}

void test_seek_modify_point_outside_buffer_range() {

  // Modify point outside of buffer range
  // 1. Write 8 points.
  // 2. Read second point.
  // 3. Modify second point.
  // 4. Seek outside of buffer. Read point.
  // 5. Read second point.

  cout << "starting test_seek_modify_point_outside_buffer_range ";

  delete_file("test_seek_modify_point_outside_buffer_range.dat");

  // 1. Write 8 points.
  io::buffered_stream<point> bs(2);
  bs.open("stream/testfiles/test_seek_modify_point_outside_buffer_range.dat");
  bs.write(point(89,56)); //0
  bs.write(point(458,54)); //8
  bs.write(point(48,41)); //16
  bs.write(point(8,23)); //24
  bs.write(point(356,213));//32
  bs.write(point(123,56));//40
  bs.write(point(98,234));//48
  bs.write(point(123,45));//56

  // 2. Seek to start of file and read point
  bs.seek(8,SEEK_SET);
  assert( (point(458,54) == bs.read()) && "Invalid point read");

  // 3. Seek to middle of file and read point
  bs.seek(8,SEEK_SET);
  bs.write(point(777,888));

  bs.seek(48,SEEK_SET);
  point p = bs.read();
  DEBUG_MSG(p);
  assert( (point(98,234) == p) && "Invalid point read");

  bs.seek(8,SEEK_SET);
  assert( (point(777,888) == bs.read()) && "Invalid point read");
  
  bs.close();

  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
  
}

void test_split_file_in_halve() {

  // split file in two halfes:
  // 1. Write 8 points.
  // 2. Seek to middle.
  // 3. Write remaining points to new file.
  // 4. Truncate original file.
  
  cout << "starting test_split_file_in_halve ";

  delete_file("test_split_file.dat");
  delete_file("test_split_file_split1.dat");
  
  // 1. Write 8 points.
  io::buffered_stream<point> bs(2);
  bs.open("stream/testfiles/test_split_file.dat");
  bs.write(point(89,56));//0
  bs.write(point(458,54));//8
  bs.write(point(48,41));//16
  bs.write(point(8,23));//24
  bs.write(point(356,213));//32
  bs.write(point(123,56));
  bs.write(point(98,234));
  bs.write(point(123,45));
  bs.close();

  // 2. Seek to middle
  bs.open("stream/testfiles/test_split_file.dat");
  bs.seek(32,SEEK_SET);

  // 3. Write remaining points to new file.
  io::buffered_stream<point> os(2);
  os.open("stream/testfiles/test_split_file_split1.dat");
  while (!bs.eof()) os.write(bs.read());
  os.close();

  // 4. Truncate original file
  bs.seek(32,SEEK_SET);
  bs.truncate();
  bs.close(); 

  

  // Assert files split into halves.
  bs.open("stream/testfiles/test_split_file.dat");
  assert ( (point(89,56) == bs.read() && point(458,54) == bs.read() && point(48,41) == bs.read() && point(8,23) == bs.read()) && "Not the correct point");
  assert ( (bs.eof()) && "File should be eof");
  bs.close();

  os.open("stream/testfiles/test_split_file_split1.dat");
  assert ( (point(356,213) == os.read() && point(123,56) == os.read() && point(98,234) == os.read() && point(123,45) == os.read()) && "Not the correct point");
  assert ( (os.eof()) && "File should be eof");
  os.close();
  
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
  
}

void test_read_entire_file_test_eof() {

  // Read entire file
  // 1. Write 8 points.
  // 2. Read all points. Check eof.
  
  cout << "starting test_read_entire_file ";

  delete_file("test_read_entire_file.dat");

  // 1. Write 8 points.
  io::buffered_stream<point> bs(2);
  bs.open("stream/testfiles/test_read_entire_file.dat");
  bs.write(point(89,56));
  bs.write(point(458,54));
  bs.write(point(48,41));
  bs.write(point(8,23));
  bs.write(point(356,213));
  bs.write(point(123,56));
  bs.write(point(98,234));
  bs.write(point(123,45));

  assert( (bs.eof() == 1) && "eof should be 1" );

  bs.seek(0,SEEK_SET);
  
  assert ( (bs.eof() == 0) && "eof should be 0" );

  // 2. Read all points
  int i = 0;
  while (!bs.eof()) {
    if (i == 0) assert ( (point(89,56) == bs.read()) && "Point not as expected");
    if (i == 1) assert ( (point(458,54) == bs.read()) && "Point not as expected");
    if (i == 2) assert ( (point(48,41) == bs.read()) && "Point not as expected");
    if (i == 3) assert ( (point(8,23) == bs.read()) && "Point not as expected");
    if (i == 4) assert ( (point(356,213) == bs.read()) && "Point not as expected");
    if (i == 5) assert ( (point(123,56) == bs.read()) && "Point not as expected");
    if (i == 6) assert ( (point(98,234) == bs.read()) && "Point not as expected");
    if (i == 7) assert ( (point(123,45) == bs.read()) && "Point not as expected");
    assert ( (i <= 7) && "eof should have been set");
    i++;
  }
  
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
  
}

void test_read_beyond_file() {

  // Read beyond file:
  // 1. Write 8 points.
  // 2. Read all points + 1
  
  cout << "starting test_read_beyond_file ";

  delete_file("test_read_beyond_file.dat");

  // 1. Write 8 points.
  io::buffered_stream<point> bs(2);
  bs.open("stream/testfiles/test_read_beyond_file.dat");
  bs.write(point(89,56));
  bs.write(point(458,54));
  bs.write(point(48,41));
  bs.write(point(8,23));
  bs.write(point(356,213));
  bs.write(point(123,56));
  bs.write(point(98,234));
  bs.write(point(123,45));

  // 2. Read all points
  while (!bs.eof()) bs.read();

  // We should see stream exit on ENOTTY. Delete test when fixed.
  // assert ( (point(999,999) == bs.read()) && "Should exit with errno = ENOTTY");
  
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
  
}

void test_file_size() {

  cout << "starting test_file_size ";

  delete_file("test_size.txt");
  
  io::buffered_stream<point> bs(1);
  bs.open("stream/testfiles/test_size.txt");
  bs.write(point(1,2));
  bs.write(point(3,2));
  bs.close();

  bs.open("stream/testfiles/test_size.txt");
  assert ( (bs.size() == 2) && "Not the right size");
  bs.close();
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_large_buffer() {
  cout << "starting test large buffer size ";
  delete_file("test_large_buffer.txt");
  io::buffered_stream<point> bs(4096);
  bs.open("stream/testfiles/test_large_buffer.txt");
  
  for (int i = 0; i < 10000; i++) {
    bs.write(point(i,i));
    assert( (bs.size() == (size_t)(i+1)) && "Incorrect size");
  }
  assert( (bs.size() == 10000) && "Incorrect size of buffer");

  bs.write(point(10,12));
  assert( (bs.size() != 10000) && "Incorrect size of buffer");
  bs.close();
  bs.open("stream/testfiles/test_large_buffer.txt");
  for (int i = 0; i < 10000; i++) {
    point p = bs.read();
    assert ( (p == point(i,i)) && "Incorrect element in buffer");
  }

  bs.close();
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_append() {
  cout << "starting test_append ";
  io::buffered_stream<char> bs(4096);
  delete_file("test_append.txt");
  bs.open("stream/testfiles/test_append.txt");
  bs.write('H');
  bs.write('E');
  bs.write('L');
  bs.write('L');
  bs.write('O');
  bs.close();

  bs.open("stream/testfiles/test_append.txt");
  bs.seek(0, SEEK_END);
  bs.write(' ');
  bs.write('W');
  bs.write('O');
  bs.write('R');
  bs.write('L');
  bs.write('D');  
  bs.close();

  string actual, true_string = "HELLO WORLD";
  bs.open("stream/testfiles/test_append.txt");
  while (!bs.eof()) actual+=bs.read();
  assert( (actual == true_string) && "Incorrect appendage");
  assert( (bs.size() == true_string.size()) && "Incorrect size of file");
  bs.close();
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_make_folder() {
  cout << "starting test_make_folder ";
  io::buffered_stream<char> bs(4096);
  bs.open("nofolder/test.txt");

  bs.close();
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

int main() {

  cout << "\033[0;33m\e[4mSTARTING TEST OF STREAM\e[24m\033[0m" << endl;
  
  test_open_close();
  test_read();
  test_write();
  test_interleave_read_write();
  test_read_point();
  test_file_size();
  test_read_write2();
  test_modify_first_point();
  test_modify_second_point();
  test_large_buffer();
  test_seek_read_point_outside_buffer_range();
  
  test_read_entire_file_test_eof();
  test_seek_read_point_in_buffer_range();
  test_seek_modify_point_in_buffer_range();
  test_seek_modify_point_outside_buffer_range();
  test_read_beyond_file();
  test_split_file_in_halve();
  test_append();
  //test_make_folder();
  cout << "\x1b[32mALL TESTS WERE SUCCESSFUL!\x1b[0m" << endl;

  return 0;
}
