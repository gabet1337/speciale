#include <iostream>
#include "../common/debug.hpp"
#include "../common/point.hpp"
#include "../common/test_lib.hpp"
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

vector<point> get_gerth_points() {
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

  return points;
}

void test_constructor() {
  cout << "starting test_constructor ";
  ext::child_structure cs(0, 2, 0.5, get_test_points());
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_constructor2() {
  cout << "starting test_constructor2 ";
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
  vector<point> points = get_gerth_points();
  sort(points.begin(), points.end());
  ext::child_structure cs(8, 4, 0.5, points);
  assert( cs.valid_memory() );
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_collapse2() {
  cout << "starting test of correct collapse with uneven points ";
  vector<point> points = get_gerth_points();
  sort(points.begin(), points.end());
  points.pop_back();
  points.pop_back();
  ext::child_structure cs(12, 4, 0.5, points);
  assert( cs.valid_memory() );
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_collapse_left() {
  cout << "starting test of collapsing left ";
  vector<point> points;
  for (int i = 0; i < 32; i++) points.push_back(point(i,i));
  ext::child_structure cs(9, 4, 0.5, points);
  assert(cs.valid_memory());
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_collapse_right() {
  cout << "starting test of collapsing right ";

  vector<point> points;
  for (int i = 0; i < 32; i++) points.push_back(point(i,31-i));
  ext::child_structure cs(10, 4, 0.5, points);
  assert(cs.valid_memory());
  
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_collapse_left_right() {
  cout << "starting test of collapsing left and right and all the way ";

  vector<point> points;
  for (int i = 0; i < 16; i++) points.push_back(point(i,i));
  for (int i = 16; i < 32; i++) points.push_back(point(48-i,i));
  sort(points.begin(), points.end());
  ext::child_structure cs(11, 4, 0.5, points);
  assert(cs.valid_memory());
  
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;


}

void test_flush_catalog() {
  cout << "starting test of flushing catalog ";

  vector<point> points = get_gerth_points();
  sort(points.begin(), points.end());
  
  ext::child_structure* cs = new ext::child_structure(13, 4, 0.5, points);
  assert(cs->valid_memory());

  delete cs;

  cs = new ext::child_structure(13);
  assert(cs->valid_memory());
  
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_overflow_I() {
  cout << "starting test of overflow I buffer ";
  vector<point> points;
  ext::child_structure cs(14,2,0.5,points);
  assert(cs.valid_memory());

  //insert B+1 points
  for (int i = 0; i < 3; i++)
    cs.insert(point(i,i));

  assert(cs.valid_memory());

  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_overflow_I2() {
  cout << "starting test of overflow I buffer 2 ";
  vector<point> points;
  ext::child_structure cs(15,2,0.5,points);
  assert(cs.valid_memory());

  //insert (B+1)*2 points
  for (int i = 0; i < 6; i++)
    cs.insert(point(i,i));

  assert(cs.valid_memory());

  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_overflow_I3() {
  cout << "starting test of overflow I buffer 3 ";
  vector<point> points;
  ext::child_structure *cs = new ext::child_structure(17,2,0.5,points);
  assert(cs->valid_memory());

  //insert (B+1)*2 points
  for (int i = 0; i < 6; i++)
    cs->insert(point(i,i));

  assert(cs->valid_memory());

  delete cs;

  cs = new ext::child_structure(17);

  assert(cs->valid_memory());

  //insert (B+1)*2 points
  for (int i = 6; i < 13; i++)
    cs->insert(point(i,i));

  assert(cs->valid_memory());

  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_overflow_D() {
  cout << "starting test of overflow D buffer ";
  vector<point> points;
  ext::child_structure cs(16,2,0.5,points);
  assert(cs.valid_memory());

  //remove (1+B)*2 points
  for (int i = 0; i < 6; i++)
    cs.remove(point(i,i));

  assert(cs.valid_memory());

  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_overflow_D2() {
  cout << "starting test of overflow D buffer 2 ";
  vector<point> points;
  ext::child_structure cs(18,2,0.5,points);
  assert(cs.valid_memory());

  //insert (B+1)*2 points
  for (int i = 0; i < 6; i++)
    cs.remove(point(i,i));

  assert(cs.valid_memory());

  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_overflow_D3() {
  cout << "starting test of overflow D buffer 3 ";
  vector<point> points;
  ext::child_structure *cs = new ext::child_structure(19,2,0.5,points);
  assert(cs->valid_memory());

  //insert (B+1)*2 points
  for (int i = 0; i < 6; i++)
    cs->remove(point(i,i));

  assert(cs->valid_memory());

  delete cs;

  cs = new ext::child_structure(17);

  assert(cs->valid_memory());

  //insert (B+1)*2 points
  for (int i = 6; i < 13; i++)
    cs->remove(point(i,i));

  assert(cs->valid_memory());

  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_overflow_I_and_D() {
  cout << "starting test of overflowing I and D ";

  vector<point> points;
  points.push_back(point(0,0));
  points.push_back(point(1,1));
  ext::child_structure *cs = new ext::child_structure(20,2,0.5,points);
  assert(cs->valid_memory());

  for (int i = 0; i < 2; i++) cs->insert(point(i,i));
  for (int i = 0; i < 2; i++) cs->remove(point(i,i));
  cs->remove(point(2,2));
  assert(cs->valid_memory());

  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_overflow_I_and_D_2() {
  cout << "starting test of overflowing I and D 2 ";

  vector<point> points;
  points.push_back(point(0,0));
  points.push_back(point(1,1));
  ext::child_structure *cs = new ext::child_structure(21,2,0.5,points);
  assert(cs->valid_memory());

  for (int i = 0; i < 2; i++) cs->remove(point(i,i));
  for (int i = 0; i < 2; i++) cs->insert(point(i,i));
  cs->insert(point(2,2));
  assert(cs->valid_memory());

  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_report() {
  cout << "starting report test of gerths points ";
  vector<point> points = get_gerth_points();
  sort(points.begin(), points.end());
  ext::child_structure cs(22,2,0.5,points);
  assert(cs.valid_memory());
  assert( cs.report(10,27,12).size() == 7 );
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_report_2() {
  cout << "starting report test of gerths points ";
  vector<point> points = get_gerth_points();
  sort(points.begin(), points.end());
  ext::child_structure cs(23,2,0.5,points);
  assert(cs.valid_memory());
  assert( cs.report(13,13,13).at(0) == point(13,13) );
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_report_3() {
  cout << "starting report test of gerths points ";
  vector<point> points = get_gerth_points();
  sort(points.begin(), points.end());
  ext::child_structure cs(24,2,0.5,points);
  assert(cs.valid_memory());
  std::vector<point> result = cs.report(0,32,0);
  assert ( (result == points) && "wrong result");
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_report_4() {
  
  cout << "starting report test of gerths points ";
  vector<point> points = get_gerth_points();
  sort(points.begin(), points.end());
  ext::child_structure cs(25,2,0.5,points);
  assert(cs.valid_memory());
  
  std::vector<point> result = cs.report(0,32,20);
  assert ( (result.size() == 4) && "wrong result");

  result = cs.report(1,4,1);
  assert ( (result.size() == 4) && "wrong result");

  result = cs.report(-8,-4,-1000);
  assert ( (result.size() == 0) && "wrong result");
  
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
  
}

void test_report_insert_delete_buffer_not_empty() {
  
  cout << "starting report insert delete not empty ";
  vector<point> points = get_gerth_points();
  sort(points.begin(), points.end());
  ext::child_structure cs(26,2,0.5,points);
  assert(cs.valid_memory());

  cs.insert(point(12,12));
  cs.insert(point(24,15));
  cs.insert(point(23,17));
  
  std::vector<point> result = cs.report(0,32,0);
  assert ( (result.size() == 35) && "wrong result");

  cs.remove(point(15,15));
  result = cs.report(0,32,0);
  assert ( (result.size() == 34) && "wrong result");
  
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
  
}

void test_insert_delete_40K_points() {

  cout << "starting report insert delete 40K points ";
#undef DEBUG  
  ext::child_structure cs(27,1000,0.5,std::vector<point>());

  for (int i = 0; i < 40000; i++)
    cs.insert(point(i,i));

  assert ( cs.report(10000,20000,10001).size() == 10000);

  for (int i = 0; i < 40000; i++)
    cs.remove(point(i,i));

  assert ( cs.report(10000,20000,10001).size() == 0);

#define DEBUG
  
  cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
}

void test_random_points() {

  test::random r;
  std::set<point> points;

  for (int i=0; i < 10000; i++)
    points.insert(point(r.next(100),r.next(100)));

  ext::child_structure cs(28,13,0.5,std::vector<point>(points.begin(),points.end()));

  for (int i=0; i < 40; i++) {

    int x1 = r.next(100);
    int x2 = r.next(100);
    int y = r.next(100);

    cout << "starting random test on query [" << x1 << "," << x2 << "] X [" << y << ",\u221E] ";
    
    std::vector<point> result = cs.report(x1,x2,y);
    std::vector<point> actual_res;
    std::copy_if (points.begin(), points.end(), std::back_inserter(actual_res),
		  [&x1,&x2,&y](point p){ return  x1 <= p.x && p.x <= x2 && p.y >= y; });
    std::sort(result.begin(), result.end());
    std::sort(actual_res.begin(), actual_res.end());

    if (!(result == actual_res)) {
      cout << "Wrong query result" << endl;

      cout << "Result:" << endl;
      for (auto p : result)
	cout << " - " << p << endl;

      cout << "Actual_Result:" << endl;
      for (auto p : actual_res)
	cout << " - " << p << endl;
      
      std::vector<point> difference;
      std::set_difference(actual_res.begin(),actual_res.end(),
			  result.begin(),result.end(),
			  std::back_inserter(difference));
      cout << "Difference: " << endl;
      for (auto p : difference)
	cout << " - point " << p << endl;
    }
    
    assert ( (result == actual_res) && "Wrong query result");
    cout << " found " << result.size() << " points ";
    cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
  }
  
}

void test_random_points_interleaved() {

  test::random r;
  std::set<point> points;
  std::set<point> del_points;
  
  int num_points = 10000;
  
  for (int i=0; i < num_points; i++) {
    point p = point(r.next(100),r.next(100));
    points.insert(p);
    if (i%100 == 0)
      del_points.insert(p);
  }

  ext::child_structure cs(29,13,0.5,std::vector<point>(points.begin(),points.end()));

  for (auto p : del_points) {
    cs.remove(p);
    points.erase(points.find(p));
  }

  for (int i=0; i < 40; i++) {
  
    int x1 = r.next(100);
    int x2 = r.next(100);
    int y = r.next(100);

    cout << "starting random test interleaved on query [" << x1 << "," << x2 << "] X [" << y << ",\u221E] ";
    
    std::vector<point> result = cs.report(x1,x2,y);
    std::vector<point> actual_res;
    std::copy_if (points.begin(), points.end(), std::back_inserter(actual_res),
		  [&x1,&x2,&y](point p){ return  x1 <= p.x && p.x <= x2 && p.y >= y; });
    std::sort(result.begin(), result.end());
    std::sort(actual_res.begin(), actual_res.end());

    if (!(result == actual_res)) {
      cout << "Wrong query result" << endl;

      cout << "Result:" << endl;
      for (auto p : result)
	cout << " - " << p << endl;

      cout << "Actual_Result:" << endl;
      for (auto p : actual_res)
	cout << " - " << p << endl;
      
      std::vector<point> difference;
      std::set_difference(actual_res.begin(),actual_res.end(),
			  result.begin(),result.end(),
			  std::back_inserter(difference));
      cout << "Difference: " << endl;
      for (auto p : difference)
	cout << " - point " << p << endl;
    
      assert ( (result == actual_res) && "Wrong query result");
      cout << " found " << result.size() << " points ";
    }

    cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
    
  }
  
}

void test_random_points_with_flush() {

  test::random r;
  std::set<point> points;

  for (int i=0; i < 10000; i++)
    points.insert(point(r.next(100),r.next(100)));

  ext::child_structure* cs =
    new ext::child_structure(30,13,0.5,std::vector<point>(points.begin(),points.end()));

  delete cs;

  cs = new ext::child_structure(30);
  
  for (int i=0; i < 40; i++) {

    int x1 = r.next(100);
    int x2 = r.next(100);
    int y = r.next(100);

    cout << "starting random test on query [" << x1 << "," << x2 << "] X [" << y << ",\u221E] ";
    
    std::vector<point> result = cs->report(x1,x2,y);
    std::vector<point> actual_res;
    std::copy_if (points.begin(), points.end(), std::back_inserter(actual_res),
		  [&x1,&x2,&y](point p){ return  x1 <= p.x && p.x <= x2 && p.y >= y; });
    std::sort(result.begin(), result.end());
    std::sort(actual_res.begin(), actual_res.end());

    if (!(result == actual_res)) {
      cout << "Wrong query result" << endl;

      cout << "Result:" << endl;
      for (auto p : result)
	cout << " - " << p << endl;

      cout << "Actual_Result:" << endl;
      for (auto p : actual_res)
	cout << " - " << p << endl;
      
      std::vector<point> difference;
      std::set_difference(actual_res.begin(),actual_res.end(),
			  result.begin(),result.end(),
			  std::back_inserter(difference));
      cout << "Difference: " << endl;
      for (auto p : difference)
	cout << " - point " << p << endl;
    }
    
    assert ( (result == actual_res) && "Wrong query result");
    cout << " found " << result.size() << " points ";
    cout << "\x1b[32mSUCCESS!\x1b[0m" << endl;
  }
  
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
  lol = system("rm -rf c_9");
  lol = system("rm -rf c_10");
  lol = system("rm -rf c_11");
  lol = system("rm -rf c_12");
  lol = system("rm -rf c_13");
  lol = system("rm -rf c_14");
  lol = system("rm -rf c_15");
  lol = system("rm -rf c_16");
  lol = system("rm -rf c_17");
  lol = system("rm -rf c_18");
  lol = system("rm -rf c_19");
  lol = system("rm -rf c_20");
  lol = system("rm -rf c_21");
  lol = system("rm -rf c_22");
  lol = system("rm -rf c_23");
  lol = system("rm -rf c_24");
  lol = system("rm -rf c_25");
  lol = system("rm -rf c_26");
  lol = system("rm -rf c_27");
  lol = system("rm -rf c_28");
  lol = system("rm -rf c_29");
  lol = system("rm -rf c_30");
  
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
  test_collapse2();
  test_collapse_left();
  test_collapse_right();
  test_collapse_left_right();
  test_flush_catalog();
  test_overflow_I();
  test_overflow_I2();
  test_overflow_I3();
  test_overflow_D();
  test_overflow_D2();
  test_overflow_D3();
  test_overflow_I_and_D();
  test_overflow_I_and_D_2();
  test_report();
  test_report_2();
  test_report_3();
  test_report_4();
  test_report_insert_delete_buffer_not_empty();
  test_insert_delete_40K_points();
  test_random_points();
  test_random_points_interleaved();
  test_random_points_with_flush();
  clean_up();
  
  cout << "\x1b[32mALL TESTS WERE SUCCESSFUL!\x1b[0m" << endl;
  return 0;
}

            
