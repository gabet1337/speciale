#ifndef BUFFERED_PST_HPP
#define BUFFERED_PST_HPP
#include "../common/point.hpp"
#include "../common/debug.hpp"
#include "../common/utilities.hpp"
#include "../internal/rb_tree.hpp"
#include "range.hpp"
#include "child_structure.hpp"
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <iterator>
#define INF 1000000000
namespace ext {

  class buffered_pst {
  public:
    buffered_pst(size_t buffer_size, double epsilon);
    ~buffered_pst();
    void insert(const point &p);
    void remove(const point &p);
    std::vector<point> report(int x1, int x2, int y);
#ifdef DEBUG
    bool is_valid();
#endif
  private:
    class buffered_pst_node {
    public:
      buffered_pst_node(int id, size_t buffer_size, double epsilon);
      ~buffered_pst_node();
      void insert(const point &p);
      void remove(const point &p);
      std::set<point> insert_buffer, delete_buffer, point_buffer;
#ifdef DEBUG
      bool is_valid();
#endif
    private:
      struct leaf_ranges {
        internal::rb_tree<range> ranges;
        leaf_ranges() {}
        void add_range(const point &min, int min_y, 
      };
      bool insert_buffer_overflow();
      bool delete_buffer_overflow();
      bool point_buffer_overflow();
      bool point_buffer_underflow();
      void handle_insert_buffer_overflow();
      void handle_point_buffer_overflow();
      std::string get_point_buffer_file_name(int id);
      std::string get_directory(int id);
      bool is_leaf();
      bool b_is_leaf;
      bool is_root();
      int id;
      size_t buffer_size;
      size_t B_epsilon;
      double epsilon;
      //ext::child_structure child_structure;
    };
    size_t buffer_size;

    int fanout;
    double epsilon;
    buffered_pst_node* root;
  };

  // BUFFERED_PST_NODE /////////////////////////////////////////////////////
  buffered_pst::buffered_pst_node::buffered_pst_node(int id, size_t _buffer_size, double epsilon)
    : buffer_size(_buffer_size)  {
    this->id = id;
    this->epsilon = epsilon;
    B_epsilon = (size_t)pow((double)buffer_size, epsilon);
    b_is_leaf = true;
    DEBUG_MSG("Constructed pst_node with id " << id << " and buffer_size: " << buffer_size
              << " and B_epsilon " << B_epsilon);
  }

  std::string buffered_pst::buffered_pst_node::get_point_buffer_file_name(int id) {
    return get_directory(id) + "/point_buffer";
  }

  std::string buffered_pst::buffered_pst_node::get_directory(int id) {
    return std::to_string(id);
  }
  
  buffered_pst::buffered_pst_node::~buffered_pst_node() {

    if (!util::file_exists(get_directory(id))) mkdir(get_directory(id).c_str(), 0700);
    
    DEBUG_MSG("Flushing point buffer");
    util::flush_container_to_file<std::set<point>::iterator,point>
      (point_buffer.begin(),point_buffer.end(), get_point_buffer_file_name(id), buffer_size);

    // DEBUG_MSG("Flushing insert buffer");
    // flush_container_to_file<std::set<point>::iterator,point>
    //   (point_buffer.begin(),point_buffer.end(), get_point_buffer_file_name());
  }

  void buffered_pst::buffered_pst_node::insert(const point &p) {
    DEBUG_MSG("Inserting point " << p << " into node " << id);
   
    if (is_root() && is_leaf()) {
      point_buffer.insert(p);
      if (point_buffer_overflow()) {
	handle_point_buffer_overflow();
      }
    } else if ( is_leaf() ) {
      // TODO handle this correct.
      point_buffer.insert(p);
    } else {
      DEBUG_MSG("remove duplicates of p from Pr, Ir, Dr");
      point_buffer.erase(p);
      insert_buffer.erase(p);
      delete_buffer.erase(p);
      DEBUG_MSG("Check if put into Ir or Pr");
      point min_y = *std::min_element(point_buffer.begin(), point_buffer.end(),
				      [] (const point &p1, const point &p2) {
					return p1.y < p2.y;
				      });
      if (p.y < min_y.y) {
        DEBUG_MSG("Inserted " << p << " into insert buffer...");
        insert_buffer.insert(p);
      }
      else point_buffer.insert(p);
      if ( is_root() && point_buffer_overflow() ) {
	DEBUG_MSG("Pr overflow: Moving point with smallest y-value from Pr to Ir");
	point_buffer.erase(min_y);
	insert_buffer.insert(min_y);
      }
      if (insert_buffer_overflow()) handle_insert_buffer_overflow();
    }
  }

  bool buffered_pst::buffered_pst_node::point_buffer_overflow() {
    if ( is_root() && is_leaf()) return point_buffer.size() > buffer_size;
    if ( is_root() ) return point_buffer.size() > buffer_size;
    if ( is_leaf() ) return point_buffer.size() >= buffer_size/2;
    return point_buffer.size() > buffer_size;
  }

  bool buffered_pst::buffered_pst_node::point_buffer_underflow() {
    if ( is_leaf() ) return false;
    return point_buffer.size() < buffer_size/2;
  }

  bool buffered_pst::buffered_pst_node::insert_buffer_overflow() {
    return insert_buffer.size() > buffer_size;
  }
  
  bool buffered_pst::buffered_pst_node::delete_buffer_overflow() {
    return delete_buffer.size() > buffer_size/4;
  }

  bool buffered_pst::buffered_pst_node::is_leaf() {
    return b_is_leaf;
  }

  bool buffered_pst::buffered_pst_node::is_root() {
    return id == 0;
  }

  void buffered_pst::buffered_pst_node::handle_point_buffer_overflow() {
    DEBUG_MSG("Starting to handle point buffer overflow");
    
    if ( is_leaf() && is_root() ) {
    
      std::vector<point> points(point_buffer.begin(), point_buffer.end());
      std::sort(points.begin(),points.end(),[](const point &p1, const point &p2)
		{return p1.y < p2.y || (p1.y == p2.y && p1.x < p2.x);});
      std::sort(points.begin(),points.begin()+points.size()/2);
	
      buffered_pst_node c1(1, buffer_size, epsilon);
      buffered_pst_node c2(2, buffer_size, epsilon);
	
      for (auto it=points.begin(); it != points.begin()+points.size()/4; it++) {
	c1.insert(*it);
      }

      for (auto it=points.begin()+points.size()/4; it != points.begin()+points.size()/2; it++) {
	c2.insert(*it);
      }

      point_buffer = std::set<point>(points.begin()+points.size()/2,points.end());

#ifdef DEBUG
      DEBUG_MSG("Points in point_buffer");
      for (auto p : point_buffer)
	DEBUG_MSG(p);
#endif

      b_is_leaf = false;
    }
  }

  void buffered_pst::buffered_pst_node::handle_insert_buffer_overflow() {
    DEBUG_MSG("Starting to handle insert buffer overflow");
    /*
      - Find child to send U = B/Be elements to
      - Remove points in U from Iv, Ic, Dc, Pc and Cv (load child)
      - Any point in U with larger y-value than or equal to minimum value in Pc
        is inserted into Pc and Cv and removed from U
      - Handle recursive case if Pc overflows ( move points with smallest y-value from Pc to U)
      - if child is a leaf all points are inserted into Pc => might cause recursion as above
      - else add remaining points in U to Ic => might cause overflow
     */

    DEBUG_MSG("Locate child to send insertions to");
    //fake it:
    buffered_pst_node found_child(3, buffer_size, epsilon);

    DEBUG_MSG("Create U to send to child");
    std::set<point> U;
    size_t idx = 0;
    for (auto p : insert_buffer) {
      if (++idx > buffer_size/B_epsilon) break;
      U.insert(p);
    }

    DEBUG_MSG("Remove points in U from Iv, Ic, Dc, Pc, Cv");

    for (point p : U) {
      if (insert_buffer.find(p) != insert_buffer.end()) {
        DEBUG_MSG("Removing " << p << " from insert buffer");
        insert_buffer.erase(p);
      }
      if (found_child.insert_buffer.find(p) != found_child.insert_buffer.end()) { 
        DEBUG_MSG("Removing " << p << " from insert buffer of found child");
        found_child.insert_buffer.erase(p);
      }
      if (found_child.delete_buffer.find(p) != found_child.delete_buffer.end()) { 
        DEBUG_MSG("Removing " << p << " from insert buffer of found child");
        found_child.delete_buffer.erase(p);
      }
      if (found_child.point_buffer.find(p) != found_child.point_buffer.end()) { 
        DEBUG_MSG("Removing " << p << " from insert buffer of found child");
        found_child.point_buffer.erase(p);
      }
    }
    
    if (found_child.is_leaf()) {
      DEBUG_MSG("found child was a leaf... sending U do Pc");
      for (point p : U) found_child.insert(p);
    } else {

    }
  }

#ifdef DEBUG
  bool buffered_pst::buffered_pst_node::is_valid() {
    // TODO check all invariants. Recurse on children.
    if ( insert_buffer_overflow() ) {
      DEBUG_MSG("Insert buffer overflow in node " << id);
      return false;
    }
    if ( delete_buffer_overflow() ) {
      DEBUG_MSG("Delete buffer overflow in node " << id);
      return false;
    }
    if ( point_buffer_overflow() ) {
      DEBUG_MSG("Point buffer overflow in node " << id);
      return false;
    }
    if ( point_buffer_underflow() ) {
      DEBUG_MSG("Point buffer underflow in node " << id);
      return false;
    }


    // no duplicates in Pv, Iv, Dv
    std::vector<point> intersection;
    //insert \cap delete = OE
    std::set_intersection(insert_buffer.begin(), insert_buffer.end(),
                          delete_buffer.begin(), delete_buffer.end(),
                          std::back_inserter(intersection));
    if (!intersection.empty()) {
      DEBUG_MSG("Duplicate in insert buffer and delete buffer");
      for (point p : intersection)
        DEBUG_MSG(p);
      return false;
    }
    intersection.clear();
    //insert \cap point = OE
    std::set_intersection(insert_buffer.begin(), insert_buffer.end(),
                          point_buffer.begin(), point_buffer.end(),
                          std::back_inserter(intersection));
    if (!intersection.empty()) {
      DEBUG_MSG("Duplicate in insert buffer and point buffer");
      for (point p : intersection)
        DEBUG_MSG(p);
      return false;
    }
    intersection.clear();
    // delete \cap point = OE
    std::set_intersection(delete_buffer.begin(), delete_buffer.end(),
                          point_buffer.begin(), point_buffer.end(),
                          std::back_inserter(intersection));
    if (!intersection.empty()) {
      DEBUG_MSG("Duplicate in insert buffer and point buffer");
      for (point p : intersection)
        DEBUG_MSG(p);
      return false;
    }

    // Iv U Dv have y-value less than the points in Pv
    std::vector<point> union_iv_dv;
    std::set_union(insert_buffer.begin(), insert_buffer.end(),
                   delete_buffer.begin(), delete_buffer.end(),
                   std::back_inserter(union_iv_dv));
    int min_y = std::min_element(point_buffer.begin(), point_buffer.end(),
                                 [] (point p1, point p2) { return p1.y < p2.y; })->y;
    if (std::any_of(union_iv_dv.begin(), union_iv_dv.end(),
                    [&min_y] (point p) {
                      return p.y > min_y;
                    })) {
      DEBUG_MSG("A point in Iv or Dv has larger y value than a point in Pv");
      return false;
    }

    return true;
  }
#endif

  // BUFFERED_PST /////////////////////////////////////////////////////////
  buffered_pst::buffered_pst(size_t buffer_size, double epsilon) {
    this->buffer_size = buffer_size;
    this->epsilon = epsilon;
    fanout = (epsilon == 0.5) ? (int)sqrt(buffer_size) :
      (int)pow((double)buffer_size,epsilon);
    root = new buffered_pst_node(0,buffer_size,epsilon);
    DEBUG_MSG("Constructing buffered_pst with buffer_size: "
	      << buffer_size << " epsilon: " << epsilon << " fanout: " << fanout);
      
  }

  void buffered_pst::insert(const point &p) {
    DEBUG_MSG("Inserting point " << p << " into root");
    root->insert(p);
  }
  
  buffered_pst::~buffered_pst() {
    //TODO: should delete all files of children
  }

#ifdef DEBUG
  bool buffered_pst::is_valid() {
    return root->is_valid();
  }
#endif
  
};

#endif
