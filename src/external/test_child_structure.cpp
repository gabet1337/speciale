#include <iostream>
#include "../common/debug.hpp"
#include "../common/point.hpp"
#include "child_structure.hpp"
#include <assert.h>
#include <vector>

using namespace std;

vector<point> get_test_points() {
  vector<point> points;
  points.push_back(point(1,2));
  points.push_back(point(23,43));
  points.push_back(point(4,54));
  return points;
}

void test_constructor() {
  cout << "starting test_constructor ";
  cout << sizeof(size_t) << " " << sizeof(double) <<  " " << sizeof(point) <<endl;
  ext::child_structure cs(1337, 6, 0.5, get_test_points());

  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

#ifdef VALIDATE
void test_valid_after_constructor() {
  cout << "starting test of validity in memory ";
  ext::child_structure cs(1,4,0.5, get_test_points());
  assert((cs.valid_memory() == true) && "Structure not valid in memory");
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_valid_after_destructor() {
  cout << "starting test of validity on disk ";
  ext::child_structure* cs = new ext::child_structure(2,4,0.5, get_test_points());
  delete cs;
  cs = new ext::child_structure(2);
  assert( (cs->valid_disk() == true) && "Structure not valid on disk");
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}
#endif

void test_error_on_no_existing_child_structure() {
  cout << "starting test of error on no existing child structure ";

  ext::child_structure* cs = new ext::child_structure(3,4,0.5,get_test_points());
  delete cs;

  cs = new ext::child_structure(3,4,0.5, get_test_points());

  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_flushing_L() {

  cout << "starting test of flushing L ";
  
  ext::child_structure* cs = new ext::child_structure(4,4,0.5,get_test_points());
  delete cs;

  io::buffered_stream<point> L_file(4096);
  L_file.open("c_4/L");

  vector<point> empty_points, read_in, test_points = get_test_points();
  empty_points.push_back(point(999,-999));
  
  while (!L_file.eof()) read_in.push_back(L_file.read());

  assert ((empty_points != read_in) && " points should not be equal");
  assert ((read_in == test_points) && " points not equal");  

  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_flush_insert_point() {

  cout << "starting test of inserting point ";

  ext::child_structure* cs = new ext::child_structure(5,4,0.5,get_test_points());
  cs->insert(point(2,2));

  delete cs;

  io::buffered_stream<point> I_file(4096);
  I_file.open("c_5/I");

  assert ( (I_file.read() == point(2,2)) && "point no as expected");
  assert ( (I_file.size() == sizeof(point)) && "file size not as expected");
  
  I_file.close();
  
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
  
}

void test_flush_delete_point() {

  cout << "starting test of delete point ";

  ext::child_structure* cs = new ext::child_structure(6,4,0.5,get_test_points());
  cs->remove(point(2,2));

  delete cs;

  io::buffered_stream<point> D_file(4096);
  D_file.open("c_5/I");

  assert ( (D_file.read() == point(2,2)) && "point no as expected");
  assert ( (D_file.size() == sizeof(point)) && "file size not as expected");
  
  D_file.close();
  
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
  
}

void clean_up() {
  int lol =system("rm -rf c_1");
  lol = system("rm -rf c_2");
  lol = system("rm -rf c_3");
  lol = system("rm -rf c_4");
  lol = system("rm -rf c_5");
  lol = system("rm -rf c_6");
  lol++;
}

int main() {
  clean_up();
#ifdef VALIDATE
  test_valid_after_constructor();
  test_valid_after_destructor();
#endif
  //test_error_on_no_existing_child_structure();
  test_flushing_L();
  test_flush_insert_point();
  test_flush_delete_point();
  
  cout << "\x1b[32mALL TESTS WERE SUCCESSFUL!\x1b[0m" << endl;
  return 0;
}

            
