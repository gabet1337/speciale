#include <iostream>
#include "../common/debug.hpp"
#include "../common/point.hpp"
#include "child_structure.hpp"
#include <assert.h>
#include <vector>
#include <set>
#include "../internal/rb_tree.hpp"
#include <iterator>
#include <queue>

using namespace std;

vector<point> get_test_points() {
  vector<point> points;
  points.push_back(point(1,2));
  points.push_back(point(4,54));
  points.push_back(point(23,43));
  return points;
}

void test_constructor() {
  cout << "starting test_constructor ";
  cout << sizeof(size_t) << " " << sizeof(double) <<  " " << sizeof(point) <<endl;
  ext::child_structure cs(0, 2, 0.5, get_test_points());

  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_constructor2() {
  cout << "starting test_constructor2 ";
  cout << sizeof(size_t) << " " << sizeof(double) <<  " " << sizeof(point) <<endl;
  vector<point> points;
  for (int i = 0; i < 5; i++) points.push_back(point(i,i));
  ext::child_structure cs(7, 2, 0.5, points);

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


void test_set_structure() {
  cout << "starting test of internal::rb_tree ";

  internal::rb_tree<int> s;
  
  s.insert(-10000000);
  for (int i = 1; i <= 10; i++) s.insert(i);
  s.insert(10000000);
  assert( (s.predecessor(5) == 4));
  s.erase(5);
  assert( s.predecessor(5) == 4 && "Wrong element found");
  assert( s.successor(5) == 6);
  assert( s.belong_to(5) == 4);
  assert( s.belong_to(4) == 4);

  typedef pair<int,int> ii;
  internal::rb_tree<ii> s2;
  for (int i = 0; i < 10; i++) s2.insert(ii(i,i));

  assert( s2.predecessor(ii(5,0)) == ii(4,4) );

  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_pq() {
  cout << "starting test of pq ";
  typedef pair<point,int> pi;
  auto comp = [](pi p1, pi p2) { return p1.first.y > p2.first.y;};
  priority_queue<pi, vector<pi>, decltype(comp)> pq(comp);
  pq.push(pi(point(1,2),1));
  pq.push(pi(point(3,1),2));
  pq.push(pi(point(4,0),3));

  assert ( !(pq.top().first == point(3,1)));
  assert( pq.top().first == point(4,0));

  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_collapse() {
  cout << "starting test of correct collapse ";
  vector<point> points;
  points.push_back(point(1,1));
  points.push_back(point(4,1));
  points.push_back(point(9,1));
  points.push_back(point(22,1));
  points.push_back(point(26,2));
  points.push_back(point(30,3));
  points.push_back(point(32,1));
  points.push_back(point(24,5));
  points.push_back(point(20,7));
  points.push_back(point(7,2));
  points.push_back(point(3,6));
  points.push_back(point(5,8));
  points.push_back(point(12,8));
  points.push_back(point(16,4));
  points.push_back(point(23,8));
  points.push_back(point(27,9));
  points.push_back(point(31,8));
  points.push_back(point(18,10));
  points.push_back(point(10,10));
  points.push_back(point(14,11));
  points.push_back(point(25,12));
  points.push_back(point(13,13));
  points.push_back(point(15,15));
  points.push_back(point(21,14));
  points.push_back(point(29,16));
  points.push_back(point(11,17));
  points.push_back(point(8,19));
  points.push_back(point(19,17));
  points.push_back(point(28,21));
  points.push_back(point(2,20));
  points.push_back(point(6,22));
  points.push_back(point(17,23));
  sort(points.begin(), points.end());
  ext::child_structure cs(8, 4, 0.5, points);
  assert( cs.valid_memory() );
}

void clean_up() {
  int lol = system("rm -rf c_0");
  lol =system("rm -rf c_1");
  lol = system("rm -rf c_2");
  lol = system("rm -rf c_3");
  lol = system("rm -rf c_4");
  lol = system("rm -rf c_5");
  lol = system("rm -rf c_6");
  lol = system("rm -rf c_7");
  lol = system("rm -rf c_8");
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
  test_set_structure();
  test_pq();
  test_constructor();
  test_constructor2();
  test_collapse();
  clean_up();
  
  cout << "\x1b[32mALL TESTS WERE SUCCESSFUL!\x1b[0m" << endl;
  return 0;
}

            
