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
#include <assert.h>
#include <iterator>
#include <error.h>
#include <queue>
#include <map>
#include <iostream>
#include <fstream>

#define INF 1000000000
namespace ext {
  int next_id = 1;
#ifdef DEBUG
  std::set<point> CONTAINED_POINTS;
#endif
  class buffered_pst {
  public:
    buffered_pst(size_t buffer_size, double epsilon);
    ~buffered_pst();
    void insert(const point &p);
    void remove(const point &p);
    std::vector<point> report(int x1, int x2, int y);
    void print();
#ifdef DEBUG
    bool is_valid();
#endif
  private:
    class buffered_pst_node {
    public:
      buffered_pst_node(int id, int parent_id, size_t buffer_size,
			size_t B_epsilon, buffered_pst_node* root);
      buffered_pst_node(int id, size_t buffer_size, buffered_pst_node* root);
      ~buffered_pst_node();
      int id;
      void remove(const point &p);
      void add_child(const range &r);
      void insert_into_point_buffer(const point &p);
      void load_point_buffer();
      void load_insert_buffer();
      void load_delete_buffer();
      void load_info_file();
      void load_ranges();
      void load_all();
      void flush_point_buffer();
      void flush_insert_buffer();
      void flush_delete_buffer();
      void flush_info_file();
      void flush_ranges();
      void flush_all();
      template <class Container>
      void insert_into_point_buffer(const Container &points);
      void insert_into_insert_buffer(const point &p);
      void handle_split();
      void split_leaf();
      template <class Container>
      void insert_into_insert_buffer(const Container &points);
      bool is_leaf();
      bool is_root();
      
      std::set<point> insert_buffer, delete_buffer, point_buffer;
      internal::rb_tree<range> ranges;
#ifdef DEBUG
      bool is_valid();
#endif
    private:
      std::pair<int,std::set<point> > find_child(const std::set<point> &buffer);
      bool insert_buffer_overflow();
      bool delete_buffer_overflow();
      bool point_buffer_overflow();
      bool point_buffer_underflow();
      bool is_virtual_leaf();
      void handle_insert_buffer_overflow();
      void handle_point_buffer_overflow();
      void handle_underflowing_point_buffer();
      std::string get_point_buffer_file_name(int id);
      std::string get_insert_buffer_file_name(int id);
      std::string get_delete_buffer_file_name(int id);
      std::string get_info_file_file_name(int id);
      std::string get_ranges_file_name(int id);
      std::string get_directory(int id);
      int parent_id;
      size_t buffer_size;
      size_t B_epsilon;
      double epsilon;
      buffered_pst_node* root;
      bool is_point_buffer_loaded;
      bool is_insert_buffer_loaded;
      bool is_delete_buffer_loaded;
      bool is_info_file_loaded;
      bool is_ranges_loaded;
      //ext::child_structure child_structure;
    };
    size_t buffer_size;
    int fanout;
    double epsilon;
    buffered_pst_node* root;
  };

  // BUFFERED_PST_NODE /////////////////////////////////////////////////////
  buffered_pst::buffered_pst_node::buffered_pst_node(int id, int parent_id, size_t _buffer_size, size_t B_epsilon, buffered_pst_node* root) : buffer_size(_buffer_size)  {
#ifdef DEBUG
    assert( util::file_exists(get_point_buffer_file_name(id)) == false);
    assert( util::file_exists(get_insert_buffer_file_name(id)) == false);
    assert( util::file_exists(get_delete_buffer_file_name(id)) == false);
    assert( util::file_exists(get_info_file_file_name(id)) == false);
#endif
    this->id = id;
    this->epsilon = epsilon;
    this->parent_id = parent_id;
    if ( is_root() ) this->root = this;
    else this->root = root;
    is_insert_buffer_loaded = true;
    is_delete_buffer_loaded = true;
    is_point_buffer_loaded = true;
    is_ranges_loaded = true;
    is_info_file_loaded = true;
    this->B_epsilon = B_epsilon;
    DEBUG_MSG("Constructed pst_node with id " << id << " and buffer_size: " << buffer_size
              << " and B_epsilon " << B_epsilon << " and parent_id " << parent_id);
  }

  buffered_pst::buffered_pst_node::buffered_pst_node(int id, size_t buffer_size,
						     buffered_pst_node* root) {
    DEBUG_MSG("Opening existing node " << id);
    this->id = id;
    this->root = root;
    this->buffer_size = buffer_size;
    is_insert_buffer_loaded = false;
    is_delete_buffer_loaded = false;
    is_point_buffer_loaded = false;
    is_ranges_loaded = false;
    is_info_file_loaded = false;
  }

  buffered_pst::buffered_pst_node::~buffered_pst_node() {
    DEBUG_MSG("DESTRUCTING NODE " << id);
  }

  /**************************************************************************************
    LOAD AND FLUSH
  ***************************************************************************************/ 
  
  void buffered_pst::buffered_pst_node::load_point_buffer() {
    if (is_root()) return;
    assert(!is_point_buffer_loaded);
    DEBUG_MSG("Loading point buffer for node " << id);
    util::load_file_to_container<std::set<point>, point>
      (point_buffer, get_point_buffer_file_name(id), buffer_size);
    is_point_buffer_loaded = true;
    DEBUG_MSG("Finished loading point buffer");
  }

  void buffered_pst::buffered_pst_node::load_insert_buffer() {
    if (is_root()) return;
    assert(!is_insert_buffer_loaded);
    DEBUG_MSG("Loading insert buffer for node " << id);
    util::load_file_to_container<std::set<point>, point>
      (insert_buffer, get_insert_buffer_file_name(id), buffer_size);
    is_insert_buffer_loaded = true;
  }

  void buffered_pst::buffered_pst_node::load_ranges() {
    if (is_root()) return;
    assert(!is_ranges_loaded);
    DEBUG_MSG("Loading ranges for node " << id);
    util::load_file_to_container<internal::rb_tree<range>, range>
      (ranges, get_ranges_file_name(id), buffer_size);
    is_ranges_loaded = true;
  }

  void buffered_pst::buffered_pst_node::load_delete_buffer() {
    if (is_root()) return;
    assert(!is_delete_buffer_loaded);
    DEBUG_MSG("Loading delete buffer for node " << id);
    util::load_file_to_container<std::set<point>, point>
      (delete_buffer, get_delete_buffer_file_name(id), buffer_size);
    is_delete_buffer_loaded = true;
  }

  void buffered_pst::buffered_pst_node::load_info_file() {
    if (is_root()) return;
    assert(!is_info_file_loaded);
    DEBUG_MSG("Loading info file for node " << id);
    std::vector<size_t> info_file;
    util::load_file_to_container<std::vector<size_t>, size_t>
      (info_file, get_info_file_file_name(id), 512);
    this->buffer_size = info_file[0];
    this->B_epsilon = info_file[1];
    this->parent_id = (int)info_file[2];
    is_info_file_loaded = true;
  }

  void buffered_pst::buffered_pst_node::load_all() {
    load_point_buffer();
    load_insert_buffer();
    load_delete_buffer();
    load_info_file();
    load_ranges();
  }
  
  void buffered_pst::buffered_pst_node::flush_point_buffer() {
    if (is_root()) return;
    DEBUG_MSG("Flushing point buffer for node " << id);
    if (!util::file_exists(get_directory(id))) mkdir(get_directory(id).c_str(), 0700);
    util::flush_container_to_file<std::set<point>::iterator,point>
      (point_buffer.begin(),point_buffer.end(), get_point_buffer_file_name(id), buffer_size);
    point_buffer.clear();
    is_point_buffer_loaded = false;
  }

  void buffered_pst::buffered_pst_node::flush_insert_buffer() {
    if (is_root()) return;
    DEBUG_MSG("Flushing insert buffer for node " << id);
    if (!util::file_exists(get_directory(id))) mkdir(get_directory(id).c_str(), 0700);
    util::flush_container_to_file<std::set<point>::iterator,point>
      (insert_buffer.begin(),insert_buffer.end(),
       get_insert_buffer_file_name(id), buffer_size);
    insert_buffer.clear();
    is_insert_buffer_loaded = false;
  }

  void buffered_pst::buffered_pst_node::flush_ranges() {
    if (is_root()) return;
    DEBUG_MSG("Flushing ranges for node " << id);
    if (!util::file_exists(get_directory(id))) mkdir(get_directory(id).c_str(), 0700);
    util::flush_container_to_file<std::set<range>::iterator, range>
      (ranges.begin(), ranges.end(), get_ranges_file_name(id), buffer_size);
    ranges.clear();
    is_ranges_loaded = false;
  }

  void buffered_pst::buffered_pst_node::flush_delete_buffer() {
    if (is_root()) return;
    DEBUG_MSG("Flushing delete buffer for node " << id);
    if (!util::file_exists(get_directory(id))) mkdir(get_directory(id).c_str(), 0700);
    util::flush_container_to_file<std::set<point>::iterator,point>
      (delete_buffer.begin(),delete_buffer.end(),
       get_delete_buffer_file_name(id), buffer_size);
    delete_buffer.clear();
    is_delete_buffer_loaded = false;
  }

  void buffered_pst::buffered_pst_node::flush_info_file() {
    if (is_root()) return;
    DEBUG_MSG("Flushing info file for node " << id);
    if (!util::file_exists(get_directory(id))) mkdir(get_directory(id).c_str(), 0700);
    std::vector<size_t> info_file;
    info_file.push_back(buffer_size);
    info_file.push_back(B_epsilon);
    info_file.push_back((size_t)parent_id);
    
    util::flush_container_to_file<std::vector<size_t>::iterator, size_t>
      (info_file.begin(), info_file.end(), get_info_file_file_name(id), buffer_size);

    is_info_file_loaded = false;
  }

  void buffered_pst::buffered_pst_node::flush_all() {
    if (is_point_buffer_loaded) flush_point_buffer();
    if (is_insert_buffer_loaded) flush_insert_buffer();
    if (is_delete_buffer_loaded) flush_delete_buffer();
    if (is_info_file_loaded) flush_info_file();
    if (is_ranges_loaded) flush_ranges();
  }

  std::string buffered_pst::buffered_pst_node::get_point_buffer_file_name(int id) {
    return get_directory(id) + "/point_buffer";
  }

  std::string buffered_pst::buffered_pst_node::get_insert_buffer_file_name(int id) {
    return get_directory(id) + "/insert_buffer";
  }

  std::string buffered_pst::buffered_pst_node::get_delete_buffer_file_name(int id) {
    return get_directory(id) + "/delete_buffer";
  }

  std::string buffered_pst::buffered_pst_node::get_info_file_file_name(int id) {
    return get_directory(id) + "/info_file";
  }

  std::string buffered_pst::buffered_pst_node::get_ranges_file_name(int id) {
    return get_directory(id) + "/ranges";
  }

  std::string buffered_pst::buffered_pst_node::get_directory(int id) {
    return std::to_string(id);
  }


  /**************************************************************************************
    MAIN DATA STRUCTURE
  ***************************************************************************************/ 
  
  void buffered_pst::buffered_pst_node::insert_into_insert_buffer(const point &p) {
    std::vector<point> points;
    points.push_back(p);
    insert_into_insert_buffer(points);
  }

  template <class Container>
  void buffered_pst::buffered_pst_node::insert_into_insert_buffer(const Container &points) {
    assert(is_insert_buffer_loaded);
    DEBUG_MSG("inserting point(s) into insert buffer");
#ifdef DEBUG
    for (point p : points)
      DEBUG_MSG(" - " << p);
#endif

    insert_buffer.insert(points.begin(), points.end());
    
    if (insert_buffer_overflow()) handle_insert_buffer_overflow();
  }
  
  void buffered_pst::buffered_pst_node::insert_into_point_buffer(const point &p) {
    std::vector<point> points;
    points.push_back(p);
    insert_into_point_buffer(points);
  }

  template <class Container>
  void buffered_pst::buffered_pst_node::insert_into_point_buffer(const Container &points) {
    assert(is_point_buffer_loaded);
    DEBUG_MSG("inserting point(s) into point buffer: ");

#ifdef DEBUG
    for (point p : points) DEBUG_MSG(" - " << p);
#endif
    
    point_buffer.insert(points.begin(), points.end());
    
    if (point_buffer_overflow()) handle_point_buffer_overflow();
  }

  bool buffered_pst::buffered_pst_node::point_buffer_overflow() {
    assert(is_point_buffer_loaded);
    if ( is_root() && is_leaf()) return point_buffer.size() > buffer_size;
    if ( is_root() ) return point_buffer.size() > buffer_size;
    if ( is_leaf() ) return point_buffer.size() >= buffer_size/2;
    return point_buffer.size() > buffer_size;
  }

  bool buffered_pst::buffered_pst_node::point_buffer_underflow() {
    assert(is_point_buffer_loaded);
    assert(is_ranges_loaded);
    if ( is_leaf() ) return false;
    // if our subtree doesnt have elements then we are leaf.
    if (is_virtual_leaf()) return false;
    return point_buffer.size() < buffer_size/2;
  }

  bool buffered_pst::buffered_pst_node::is_virtual_leaf() {
    bool is_virtual_leaf = true;
    for (auto r : ranges) if (r.max_y != INF) is_virtual_leaf = false;
    return is_virtual_leaf;
  }

  bool buffered_pst::buffered_pst_node::insert_buffer_overflow() {
    assert(is_insert_buffer_loaded);
    return insert_buffer.size() > buffer_size;
  }
  
  bool buffered_pst::buffered_pst_node::delete_buffer_overflow() {
    assert(is_delete_buffer_loaded);
    return delete_buffer.size() > buffer_size/4;
  }

  bool buffered_pst::buffered_pst_node::is_leaf() {
    assert(is_ranges_loaded);
    return ranges.empty();
  }

  bool buffered_pst::buffered_pst_node::is_root() {
    return id == 0;
  }

  void buffered_pst::buffered_pst_node::handle_split() {

    load_all();
    assert(is_ranges_loaded);
    assert(is_point_buffer_loaded);
    assert(is_delete_buffer_loaded);
    assert(is_insert_buffer_loaded);
    
    DEBUG_MSG("Checking if we should split node " << id << " "  << ranges.size()
	      << " " << B_epsilon);
#ifdef DEBUG
    for (auto r : ranges)
      DEBUG_MSG(" - " << r);
#endif
    if (ranges.size() > B_epsilon) {
      buffered_pst_node* parent =
	parent_id == 0 ? root : new buffered_pst_node(parent_id,buffer_size,root);

      parent->load_all();
      
      DEBUG_MSG("Handle node degree overflow");
      buffered_pst_node left_split_node(next_id++,parent_id,buffer_size,B_epsilon,root);
      buffered_pst_node right_split_node(next_id++,parent_id,buffer_size,B_epsilon,root);

      DEBUG_MSG("Distributing children");
#ifdef DEBUG
      for (auto r : ranges)
	DEBUG_MSG(" - " << r);
#endif
      size_t idx = 0;
      for (auto r : ranges) {
	buffered_pst_node child(r.node_id,buffer_size,root);
	child.load_info_file();
	if (++idx > ranges.size()/2) {
	  right_split_node.add_child(r);
	  child.parent_id = right_split_node.id;
	} else {
	  left_split_node.add_child(r);
	  child.parent_id = left_split_node.id;
	}
	child.flush_info_file();
      }

      DEBUG_MSG("Deleting our range in parent");
      if (is_root()) {
        DEBUG_MSG("We are root");
	ranges.clear();
      } else {
	for (auto r : parent->ranges) {
	  if (r.node_id == id) {
	    parent->ranges.erase(r);
	    DEBUG_MSG("Found range " << r << ". Deleted from parents range.");
	    break;
	  }
	}
      }
      
      DEBUG_MSG("Inserting new ranges");
      range left_range = *(left_split_node.ranges.begin());
      range right_range = *(right_split_node.ranges.begin());
      DEBUG_MSG(" - left_range: " << left_range);
      DEBUG_MSG(" - right_range: " << right_range);
      range left_child_range = range(left_range.min, left_range.max_y, left_split_node.id);
      range right_child_range = range(right_range.min, right_range.max_y, right_split_node.id);
      parent->add_child(left_child_range);
      parent->add_child(right_child_range);
      
      std::set<point> left_point_buffer, right_point_buffer, left_insert_buffer,
	right_insert_buffer, left_delete_buffer, right_delete_buffer;

      DEBUG_MSG("Distributing points from point_buffer");
      for (point p : point_buffer) {
	if (parent->ranges.belong_to(range(p,-1,-1)) == left_range) {
	  DEBUG_MSG("point " << p << " went into left_split_node");
	  left_point_buffer.insert(p);
	} else {
	  DEBUG_MSG("point " << p << " went into right_split_node");
	  right_point_buffer.insert(p);
	}
      }

      DEBUG_MSG("Distributing points from insert_buffer");
      for (point p : insert_buffer) {
	if (parent->ranges.belong_to(range(p,-1,-1)) == left_range) {
	  DEBUG_MSG("point " << p << " went into left_split_node");
	  left_insert_buffer.insert(p);
	} else {
	  DEBUG_MSG("point " << p << " went into right_split_node");
	  right_insert_buffer.insert(p);
	}
      }

      DEBUG_MSG("Distributing points from delete_buffer");
      for (point p : delete_buffer) {
	if (parent->ranges.belong_to(range(p,-1,-1)) == left_range) {
	  DEBUG_MSG("point " << p << " went into left_split_node");
	  left_delete_buffer.insert(p);
	} else {
	  DEBUG_MSG("point " << p << " went into right_split_node");
	  right_delete_buffer.insert(p);
	}
      }

      left_split_node.point_buffer = left_point_buffer;
      left_split_node.insert_buffer = left_insert_buffer;
      left_split_node.delete_buffer = left_delete_buffer;
      right_split_node.point_buffer = right_point_buffer;
      right_split_node.insert_buffer = right_insert_buffer;
      right_split_node.delete_buffer = right_delete_buffer;

      point_buffer.clear();
      delete_buffer.clear();
      insert_buffer.clear();

      DEBUG_MSG("Rebuild ranges to fit max_y of child");
      int left_max_y = std::max_element(left_split_node.point_buffer.begin(),
					left_split_node.point_buffer.end(),
                                        comp_y)->y;
					// [](const point &p1, const point &p2) {
					//   return p1.y < p2.y || (p1.y == p2.y && p1.x < p2.x);
					// })->y;
      if (left_split_node.point_buffer.empty()) left_max_y = INF;
      int right_max_y = std::max_element(right_split_node.point_buffer.begin(),
					 right_split_node.point_buffer.end(),
					 [](const point &p1, const point &p2) {
					   return p1.y < p2.y || (p1.y == p2.y && p1.x < p2.x);
					 })->y;
      if (right_split_node.point_buffer.empty()) right_max_y = INF;

      // DEBUG_MSG("Check if point buffers has new min x value");
      // point left_min_x = left_child_range.min;
      // if (!left_split_node.point_buffer.empty())
      //   left_min_x = std::min(left_min_x, *left_split_node.point_buffer.begin());
      
      // point right_min_x = right_child_range.min;
      // if (!right_split_node.point_buffer.empty())
      //   right_min_x = std::min(right_min_x, *right_split_node.point_buffer.begin());


      parent->ranges.erase(left_child_range);
      parent->add_child(range(left_child_range.min, left_max_y, left_child_range.node_id)); 
      parent->ranges.erase(right_child_range);
      parent->add_child(range(right_child_range.min, right_max_y, right_child_range.node_id));

      left_split_node.flush_all();
      right_split_node.flush_all();
      
      // TODO: Check if flush_all is needed.
      flush_all();
      parent->flush_all();

      left_split_node.handle_underflowing_point_buffer();
      right_split_node.handle_underflowing_point_buffer();
      parent->handle_underflowing_point_buffer();
      
      if (parent_id != 0) delete parent;
      
    }
  }

  void buffered_pst::buffered_pst_node::split_leaf() {

     DEBUG_MSG("Started splitting of leaf");

     //load_point_buffer();
     assert(is_point_buffer_loaded);
   
    /*
      1.) Move last b/4 point_buffer points to new leaf.
      2.) Update max_y on existing interval
      3.) Insert new interval for new leaf with min, max_y, node_id
    */

    std::vector<point> move_points;
    std::set<point> keep_points;
    size_t idx = 0;
    int keep_max_y = -INF, move_max_y = -INF;
    point move_min = point(INF,INF);
    for (auto p : point_buffer) {
      if (++idx <= point_buffer.size()/2) {
	DEBUG_MSG("Keep point " << p);
	keep_points.insert(p);
	keep_max_y = std::max(keep_max_y,p.y);
	continue;
      }
      DEBUG_MSG("Move point " << p);
      move_max_y = std::max(move_max_y, p.y);
      move_min = std::min(move_min, p);
      move_points.push_back(p);
    }

    point_buffer = keep_points;

    buffered_pst_node* parent =
      parent_id == 0 ? root : new buffered_pst_node(parent_id,buffer_size,root);
    parent->load_ranges();
    
    range r = parent->ranges.belong_to(range(*(point_buffer.begin()),-1,-1));
    DEBUG_MSG("Existing interval has changed max_y from " << r.max_y << " to "
	      << keep_max_y);
    parent->ranges.erase(r);
    parent->ranges.insert(range(r.min,keep_max_y,r.node_id));

    int new_node_id = next_id++;
    DEBUG_MSG("Insert interval for new leaf " << move_min << " "
	      << move_max_y << " " << new_node_id << " into " << parent->id);
    
    buffered_pst_node new_child(new_node_id,parent_id,buffer_size,B_epsilon, root);
    
    parent->ranges.insert(range(move_min,move_max_y,new_node_id));

    new_child.insert_into_point_buffer(move_points);

#ifdef DEBUG
    DEBUG_MSG("Ranges in parent now contains:");
    for (auto r : parent->ranges)
      DEBUG_MSG(" - " << r);
#endif
    
    new_child.flush_all();
    flush_point_buffer();
    parent->flush_ranges();
    
    if ( parent_id != 0 ) delete parent;
  }
  
  void buffered_pst::buffered_pst_node::handle_point_buffer_overflow() {
    DEBUG_MSG("Starting to handle point buffer overflow");
    
    if ( is_leaf() && is_root() ) {
    
      std::vector<point> points(point_buffer.begin(), point_buffer.end());
      std::sort(points.begin(),points.end(),[](const point &p1, const point &p2)
		{return p1.y < p2.y || (p1.y == p2.y && p1.x < p2.x);});
      std::sort(points.begin(),points.begin()+points.size()/2);
      int child1_id = next_id++;
      int child2_id = next_id++;
      buffered_pst_node c1(child1_id, id, buffer_size, B_epsilon, root);
      buffered_pst_node c2(child2_id, id, buffer_size, B_epsilon, root);
      c1.insert_into_point_buffer(std::vector<point>(points.begin(),
                                                     points.begin()+points.size()/4));

      c2.insert_into_point_buffer(std::vector<point>(points.begin()+points.size()/4,
                                                     points.begin()+points.size()/2));
      
      point_buffer = std::set<point>(points.begin()+points.size()/2,points.end());

      DEBUG_MSG("Adding new child ranges to node " << id);
      add_child(range(*c1.point_buffer.begin(),
                      std::max_element(c1.point_buffer.begin(),
                                       c1.point_buffer.end(),
                                       [](const point &p1, const point &p2) {
                                         return p1.y < p2.y || (p1.y == p2.y && p1.x < p2.x);
                                       })->y, child1_id));

      add_child(range(*c2.point_buffer.begin(),
                      std::max_element(c2.point_buffer.begin(),
                                       c2.point_buffer.end(),
                                       [](const point &p1, const point &p2) {
                                         return p1.y < p2.y || (p1.y == p2.y && p1.x < p2.x);
                                       })->y, child2_id));

      //DEBUG_MSG("Rebuild child structure of node " << id);
      

#ifdef DEBUG
      DEBUG_MSG("Points in point_buffer of node " << id);
      for (auto p : point_buffer)
	DEBUG_MSG(p);
#endif
      
      c1.flush_all();
      c2.flush_all();
      
    } else if ( is_root() ) {
      DEBUG_MSG("Overflow in the point buffer of the root. Move min point to insert buffer");
      point min_y = *std::min_element(point_buffer.begin(), point_buffer.end(),
				      [] (const point &p1, const point &p2) {
					return p1.y < p2.y || (p1.y == p2.y && p1.x < p2.x);
				      });
      point_buffer.erase(min_y);
      insert_into_insert_buffer(min_y);
    } else if ( is_leaf() ) {
      DEBUG_MSG("Overflow in the point buffer of leaf " << id);
      split_leaf();
    } else {
      error(1, ECANCELED, "We should never overflow a point buffer in an internal node");
    }
  }

  void buffered_pst::buffered_pst_node::add_child(const range &r) {
    DEBUG_MSG("Adding range " << r << " to node " << id);
    assert(is_ranges_loaded);
    ranges.insert(r);
  }

  std::pair<int, std::set<point> > buffered_pst::buffered_pst_node::find_child
  (const std::set<point> &buffer) {
    assert(is_ranges_loaded);
    DEBUG_MSG("Starting to find child");

#ifdef DEBUG
    DEBUG_MSG("Ranges to consider:");
    for (auto r : ranges)
      DEBUG_MSG(" - " << r);
#endif
    
    std::set<point> tmp,best;
    range best_range = ranges.belong_to(range(*buffer.begin(),-1,-1));
    range cur_range = ranges.belong_to(range(*buffer.begin(),-1,-1));
    for (point p : buffer) {
      range blt = ranges.belong_to(range(p,-1,-1));
      DEBUG_MSG("point " << p << " belongs to " << blt);
      if (blt != cur_range) {
        if (tmp.size() > best.size()) {
          best = tmp;
          best_range = cur_range;
        }
        cur_range = blt;
        tmp.clear();
      }
      tmp.insert(p);
    }
    if (tmp.size() > best.size()) {
      best = tmp;
      best_range = cur_range;
    }

    return {best_range.node_id, best};
  }

  void buffered_pst::buffered_pst_node::handle_insert_buffer_overflow() {
    DEBUG_MSG("Starting to handle insert buffer overflow");
    assert(is_insert_buffer_loaded);
    assert(is_point_buffer_loaded);
    assert(is_delete_buffer_loaded);
    assert(is_ranges_loaded);
    assert(is_info_file_loaded);
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
    auto child_info = find_child(insert_buffer);
    buffered_pst_node found_child(child_info.first,buffer_size,root);
    found_child.load_all();
      
    DEBUG_MSG("Create U to send to child");
    DEBUG_MSG(buffer_size/B_epsilon);
    std::set<point> U;
    range blt = ranges.belong_to(range(*(child_info.second.begin()),-1,-1));
    size_t idx = 0;
    int max_y = blt.max_y;
    point min = blt.min;
    for (auto p : child_info.second) {
      DEBUG_MSG(idx);
      if (++idx > buffer_size/B_epsilon) break;
      min = std::min(min,p);
      max_y = std::max(max_y, p.y);
      DEBUG_MSG("point " << p << " is in U");
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
	DEBUG_MSG("Removing " << p << " from delete buffer of found child");
	found_child.delete_buffer.erase(p);
      }
      if (found_child.point_buffer.find(p) != found_child.point_buffer.end()) { 
	DEBUG_MSG("Removing " << p << " from point buffer of found child");
	found_child.point_buffer.erase(p);
      }
    }
    
    if (found_child.is_leaf()) {
      
      if (*(U.begin()) < blt.min || max_y > blt.max_y) {
	DEBUG_MSG("Updating range " << min << " " << max_y << " for node_id: " << blt.node_id);
	ranges.erase(blt);
	ranges.insert(range(min,max_y,blt.node_id));
      }

#ifdef DEBUG
      DEBUG_MSG("Range now contains:");
      for (auto r : ranges)
	DEBUG_MSG(" - " << r);
#endif

      flush_all();
	
      DEBUG_MSG("found child was a leaf... sending U do Pc");
      found_child.insert_into_point_buffer(U);
      found_child.flush_all();
	
    } else {

      /*
	1.) Any point in U with y-value >= the min y-value in Pc is inserted into Pc and Cv
	and removed from U.
	2.) If Pc overflows, i.e. |Pc| > B then repeatedly move min y-value point from Pc
	to U until |Pc| = B.
	3.) Add remaining points in U to Ic which might overflow.
      */

      point min_y = *std::min_element(found_child.point_buffer.begin(),
				      found_child.point_buffer.end(),
				      [](const point &p1, const point &p2) {
					return p1.y < p2.y || (p1.x == p2.x && p1.y < p2.y);
				      });
      DEBUG_MSG("Found min_y in found_child " << found_child.id << " to be " << min_y);

      DEBUG_MSG("Distributing points according to min_y element");
      std::set<point> new_U;
      for (point p : U) {
        if (min_y.y <= p.y || (min_y.y == p.y && min_y.x <= p.x)) {
	  DEBUG_MSG("Point " << p << " went into found_childs point buffer");
	  found_child.point_buffer.insert(p);
	} else {
	  DEBUG_MSG("Point " << p << " stays in U");
	  new_U.insert(p);
	}
      }
      U = new_U;

      std::vector<point> sorted_point_buffer(found_child.point_buffer.begin(),
					     found_child.point_buffer.end());

      std::sort(sorted_point_buffer.begin(), sorted_point_buffer.end(),
		[](const point &p1, const point &p2) {
		  return p1.y < p2.y || (p1.x == p2.x && p1.y < p2.y);
		});
	
      if (found_child.point_buffer_overflow()) {
	DEBUG_MSG("Point buffer overflows in found_child " << found_child.id);
	DEBUG_MSG("Moving " << (found_child.point_buffer.size()-buffer_size)
		  << " points from point_buffer to U");
	size_t num_points_to_move = found_child.point_buffer.size()-buffer_size;
	for (size_t i = 0; i < num_points_to_move; i++) {
	  DEBUG_MSG(" - " << sorted_point_buffer[i] << " went into U");
	  found_child.point_buffer.erase(sorted_point_buffer[i]);
	  U.insert(sorted_point_buffer[i]);
	}	  
      }

      DEBUG_MSG("Updating range");
      range old_range;
      for (range r : ranges) {
	if (found_child.id == r.node_id) {
	  old_range = r;
	  break;
	}
      }

      range new_range = range(std::min(*found_child.point_buffer.begin(),old_range.min),
			      (sorted_point_buffer.end()-1)->y,
			      old_range.node_id);
	
      DEBUG_MSG("Updating range for found_child " << found_child.id << " from "
		<< old_range << " to " << new_range);
	
      ranges.erase(old_range);
      ranges.insert(new_range);
	
      DEBUG_MSG("Inserting U into found_child " << found_child.id);
	
#ifdef DEBUG
      for (point p : U)
	DEBUG_MSG(" - " << p);
#endif
      flush_all();
      
      found_child.insert_into_insert_buffer(U);
      found_child.flush_all();
    }
    handle_split();
  }

  void buffered_pst::buffered_pst_node::handle_underflowing_point_buffer() {
    DEBUG_MSG("Starting to handle underflowing point buffer in node " << id);
    load_point_buffer();
    load_ranges();
    assert(is_point_buffer_loaded);
    assert(is_ranges_loaded);

    if (is_virtual_leaf() && point_buffer.empty()) {
      load_insert_buffer();
      std::vector<point> temp(insert_buffer.begin(), insert_buffer.end());
      insert_buffer.clear();
      flush_insert_buffer();
      insert_into_point_buffer(temp);
    }
    
    if (!point_buffer_underflow()) {
      DEBUG_MSG("We are not underflowed");
      flush_point_buffer();
      return;
    } else flush_point_buffer();
    
    DEBUG_MSG("We are underflowed. Let's butt(tom) up check children");

    internal::rb_tree<range> new_ranges;
    // TODO: FLUSH RANGES!
    for (range r : ranges) {
      buffered_pst_node n(r.node_id,buffer_size,root);
      n.load_info_file();
      n.load_point_buffer();
      n.load_ranges();
      if (n.is_leaf() && n.point_buffer.empty()) {
	DEBUG_MSG("We have found an empty leaf. Delete it. " << n.id << " was deleted.");
	//continue;
      }
      n.flush_info_file();
      n.flush_point_buffer();
      n.flush_ranges();
      new_ranges.insert(r);
      //flush_ranges();
      n.handle_underflowing_point_buffer();
      //load_ranges();
    }
    ranges = new_ranges;

    DEBUG_MSG("We have filled our children!... Now ready to fill ourself");
    typedef std::pair<point, int> point_child_pair;
    auto comp = [](const point_child_pair &p1, const point_child_pair &p2) {
      return p1.first.y < p2.first.y || (p1.first.y == p2.first.y && p1.first.x < p2.first.x);
    };
    std::priority_queue<point_child_pair, std::vector<point_child_pair>,
			decltype(comp)> pq(comp);
    std::vector<buffered_pst_node> children;
    for (range r : ranges) {
      children.push_back(buffered_pst_node(r.node_id,buffer_size,root));
      children.back().load_point_buffer();
      children.back().load_ranges();
    }

#ifdef DEBUG
    for (auto c : children)
      DEBUG_MSG("We have loaded " << c.id);
#endif
    std::map<int,int> node_id_to_index;
    for (int i = 0; i < (int)children.size(); i++) {
      node_id_to_index[children[i].id] = i;
      for (auto p : children[i].point_buffer) {
	DEBUG_MSG("Added point: " << p << " to pq with c.id = " << children[i].id);
	pq.push(point_child_pair(p,i));
      }
    }

    //TODO: If point_buffer is empty. Delete from ranges.
    DEBUG_MSG("Grabbing the B/2 highest y-value points from children and deleting them");
    std::set<point> X;
    for (size_t i = 0; !pq.empty() && i < buffer_size/2; i++) {
      point_child_pair pcp = pq.top();
      pq.pop();
      DEBUG_MSG("Inserting " << pcp.first << " into X");
      X.insert(pcp.first);
      DEBUG_MSG("Removing " << pcp.first << " from " << children[pcp.second].id);
      children[pcp.second].point_buffer.erase(pcp.first);
    }

    DEBUG_MSG("Removing points in X cap Dv from X and Dv");
    load_delete_buffer();
    assert(is_delete_buffer_loaded);
    std::set<point> tmp_X;
    for (point p : X)
      if (delete_buffer.find(p) != delete_buffer.end()) {
	DEBUG_MSG("point " << p << " found in both X and Dv");
	delete_buffer.erase(p);
      } else {
	DEBUG_MSG("point " << p << " remains in X");
	tmp_X.insert(p);
      }
    X = tmp_X;

    DEBUG_MSG("Move more recent updates of p from Iv to X");
    load_insert_buffer();
    assert(is_insert_buffer_loaded);
    tmp_X.clear();
    for (point p : X) {
      if (insert_buffer.find(p) != insert_buffer.end()) {
	DEBUG_MSG("point " << p << " is in X and Iv. Move point from Iv to X");
	tmp_X.insert(*insert_buffer.find(p));
	insert_buffer.erase(*insert_buffer.find(p));
      } else {
	DEBUG_MSG("point " << p << " was not in Iv and remains.");
	tmp_X.insert(p);
      }
    }
    X = tmp_X;

    DEBUG_MSG("Satisfy heap ordering between Iv and Pv");
    std::vector<point> vp_temp(X.begin(), X.end());
    vp_temp.insert(vp_temp.end(), insert_buffer.begin(), insert_buffer.end());
    std::sort(vp_temp.begin(), vp_temp.end(),
	      [](const point &p1, const point &p2) {
		return p1.y > p2.y || (p1.y == p2.y && p1.x > p2.x);
	      });
    int X_size = X.size();
    X = std::set<point>(vp_temp.begin(), vp_temp.begin()+X_size);
    insert_buffer = std::set<point>(vp_temp.begin()+X_size, vp_temp.end());
#ifdef DEBUG
    DEBUG_MSG("X now contains:");
    for (auto p : X) DEBUG_MSG(" - " << p);
    DEBUG_MSG("Insert buffer now contains");
    for (auto p : insert_buffer) DEBUG_MSG(" - " << p);
#endif

    DEBUG_MSG("Rebuilding our ranges");
    new_ranges.clear();
    for (range r : ranges) {
      point max_y = *std::max_element(children[node_id_to_index[r.node_id]].point_buffer.begin(),
				      children[node_id_to_index[r.node_id]].point_buffer.end(),
				      [](const point &p1, const point &p2) {
					return p1.y < p2.y || (p1.y == p2.y && p1.x < p2.x);
				      });
      if (children[node_id_to_index[r.node_id]].is_leaf() &&
	  children[node_id_to_index[r.node_id]].point_buffer.empty()) {
	DEBUG_MSG("We have found an empty leaf. Delete " <<
		  children[node_id_to_index[r.node_id]].id);
	//continue;
      }
      if (children[node_id_to_index[r.node_id]].point_buffer.empty())
	new_ranges.insert(range(r.min, INF, r.node_id));
      else new_ranges.insert(range(r.min, max_y.y, r.node_id));
    }
    ranges = new_ranges;
    DEBUG_MSG("Our ranges now look LIKE this:");
#ifdef DEBUG
    for (range r : ranges) DEBUG_MSG(r);
#endif

    DEBUG_MSG("Finally point in X are inserted into Pv");
    load_point_buffer();
    // if (is_leaf()) {
    //   for (point p : insert_buffer) X.insert(p);
    //   insert_buffer.clear();
    //   delete_buffer.clear();
    // }
    insert_into_point_buffer(X);

    flush_point_buffer();
    flush_insert_buffer();
    flush_delete_buffer();
    
    for (auto c : children) {
      c.flush_point_buffer();
      c.flush_ranges();
    }
     
    DEBUG_MSG("Check if any children have underflowing point buffers as a result");
    // TODO: Fix them ranges!!
    for (range r : ranges) {
      buffered_pst_node n(r.node_id,buffer_size,root);
      n.handle_underflowing_point_buffer();
    }
    flush_ranges();
  }
  
#ifdef DEBUG
  bool buffered_pst::buffered_pst_node::is_valid() {

    DEBUG_MSG("STARTING IS VALID TEST");
    load_all();
    
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
      DEBUG_MSG("Point buffer underflow in node");
      return false;
    }

    // if ( is_leaf() && point_buffer.empty() && !is_root())  {
    //   DEBUG_MSG("We are an empty leaf... we should have been deleted " << id);
    //   return false;
    // }

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
    point min_y = *std::min_element(point_buffer.begin(), point_buffer.end(),
                                    [] (point p1, point p2) {
                                      return p1.y < p2.y || (p1.y == p2.y && p1.x < p2.x);
                                    });
    if (std::any_of(union_iv_dv.begin(), union_iv_dv.end(),
                    [&min_y] (point p) {
                      return min_y.y < p.y || (min_y.y == p.y && min_y.x < p.x);
                    })) {
      DEBUG_MSG("A point in Iv or Dv has larger y value than a point in Pv");
      return false;
    }

    for (auto it = ranges.begin(); it != ranges.end(); it++) {
      io::buffered_stream<point> bs(4096);
      bs.open(get_point_buffer_file_name(it->node_id));
      while (!bs.eof()) {
        point p = bs.read();
        if (ranges.belong_to(range(p,0,0)) != *it) {
          DEBUG_MSG("point " << p << " is in the wrong child " << it->node_id);
          DEBUG_MSG("child " << it->node_id << " is responsible for " << *it
                    << " but belong to " << ranges.belong_to(range(p,0,0)));
          return false;
        }
	if (p.y > it->max_y) {
	  DEBUG_MSG("point " << p << " has y-value larger than range max_y " << it->max_y);
	  return false;
	}
      }
      bs.close();
    }

    if (ranges.size() == 0 && is_leaf() != true) {
      DEBUG_MSG("We have children but is a leaf in node: " << id);
      return false;
    }

    if (ranges.size() != 0 && is_leaf() != false) {
      DEBUG_MSG("We dont have children but we are not a leaf! in node: " << id);
      return false;
    }

    // tests the heap order on point buffers
    DEBUG_MSG("Test the heap order on " << id);
    for (auto r : ranges) {
      DEBUG_MSG("Opening child " << r.node_id);
      buffered_pst_node child(r.node_id, buffer_size,root);
      child.load_point_buffer();
      if (std::any_of(child.point_buffer.begin(), child.point_buffer.end(),
                      [&min_y] (point p) {
                        return min_y.y < p.y || (min_y.y == p.y && min_y.x < p.x);
                      })) {
        DEBUG_MSG("A point in child has larger y value than a point in Pv");
        DEBUG_MSG("We have points: ");
        for (point p : point_buffer) DEBUG_MSG(" - " << p);
        DEBUG_MSG("Child has points:");
        for (point p : child.point_buffer) DEBUG_MSG(" - " << p);
        return false;
      }
    }

    // tests that all points supposed to be in the structure is there.
    // all points must reside in point buffers or insert buffers.
    if (is_root()) {
      std::set<point> collected_points;
      for (point p : point_buffer) collected_points.insert(p);
      for (point p : insert_buffer) collected_points.insert(p);
      std::queue<buffered_pst_node> q;
      for (range r : ranges) {
        q.push(buffered_pst_node(r.node_id, buffer_size, root));
      }

      while (!q.empty()) {
        buffered_pst_node bpn = q.front(); q.pop();
        bpn.load_all();
        for (point p : bpn.insert_buffer) collected_points.insert(p);
        for (point p : bpn.point_buffer) collected_points.insert(p);
        for (range r : bpn.ranges) q.push(buffered_pst_node(r.node_id, buffer_size, root));
      }
      if (collected_points != CONTAINED_POINTS) {
        DEBUG_MSG("CONTAINED_POINTS:");
        for (point p : CONTAINED_POINTS) DEBUG_MSG(" - " << p);
        DEBUG_MSG("collected_points:");
        for (point p : collected_points) DEBUG_MSG(" - " << p);
        DEBUG_MSG("The collected points in our structure is not equal to the inserted points");
        std::vector<point> diff;
        std::set_difference(CONTAINED_POINTS.begin(), CONTAINED_POINTS.end(),
                            collected_points.begin(), collected_points.end(),
                            std::back_inserter(diff));
        for (point p : diff) DEBUG_MSG(" - " << p);
        return false;
      }
    }

    //Test if all points in the subtree is in the correct x range
    for (auto r : ranges) {
      std::set<point> points_in_subtree;
      std::queue<buffered_pst_node> q;
      q.push(buffered_pst_node(r.node_id, buffer_size, root));
      while (!q.empty()) {
        buffered_pst_node bpn = q.front(); q.pop();
        bpn.load_all();
        for (point p : bpn.point_buffer) points_in_subtree.insert(p);
        for (point p : bpn.insert_buffer) points_in_subtree.insert(p);
        for (range ra : bpn.ranges) q.push(buffered_pst_node(ra.node_id, buffer_size, root));
      }
      for (point p : points_in_subtree) {
        if (ranges.belong_to(range(p,-1,-1)) != r) {
          DEBUG_MSG("Point " << p << " was found not belonging to this subtree but was found to belong to " << ranges.belong_to(range(p,-1,-1)));
          for (range ra : ranges) DEBUG_MSG(" - " << ra);
          return false;
        }
      }
    }

    // ALWAYS AT THE BUTT
    for (auto r : ranges) {
      DEBUG_MSG("Opening child " << r.node_id);
      buffered_pst_node child(r.node_id,buffer_size,root);
      if (child.is_valid() == false) {
	return false;
      }
    }
    

    flush_all();
    DEBUG_MSG("IS VALID IS DONE AND GOOD!");
    return true;
  }
#endif

  // BUFFERED_PST /////////////////////////////////////////////////////////
  buffered_pst::buffered_pst(size_t buffer_size, double epsilon) {
    this->buffer_size = buffer_size;
    this->epsilon = epsilon;
    fanout = (epsilon == 0.5) ? (int)sqrt(buffer_size) :
      (int)pow((double)buffer_size,epsilon);
    root = new buffered_pst_node(0,0,buffer_size,fanout,0);
    DEBUG_MSG("Constructing buffered_pst with buffer_size: "
	      << buffer_size << " epsilon: " << epsilon << " fanout: " << fanout);
    next_id = 1;
#ifdef DEBUG
    CONTAINED_POINTS.clear();
#endif
  }

  void buffered_pst::insert(const point &p) {
    DEBUG_MSG("Inserting point " << p << " into root");
#ifdef DEBUG
    CONTAINED_POINTS.insert(p);
#endif
    if (root->is_leaf()) {
      root->insert_into_point_buffer(p);
    } else {
      DEBUG_MSG("remove duplicates of p from Pr, Ir, Dr");
      root->point_buffer.erase(p);
      root->insert_buffer.erase(p);
      root->delete_buffer.erase(p);
      DEBUG_MSG("Check if put into Ir or Pr");
      point min_y = *std::min_element(root->point_buffer.begin(), root->point_buffer.end(),
				      [] (const point &p1, const point &p2) {
					return p1.y < p2.y || (p1.y == p2.y && p1.x < p2.x);
				      });
      if (p.y < min_y.y || (p.y == min_y.y && p.x < min_y.x)) root->insert_into_insert_buffer(p);
      else root->insert_into_point_buffer(p);
    }
  }

  void buffered_pst::print() {
    std::ofstream dot_file;
    dot_file.open("temp.dot");
    dot_file << "digraph {\n";
    dot_file << "0 [label=\"0\npb: ";
    for (point p : root->point_buffer) dot_file << p << ", ";
    dot_file << "\nib: ";
    for (point p : root->insert_buffer) dot_file << p << ", ";
    dot_file << "\"]\n";
    std::queue<buffered_pst_node> q;
    for (auto r : root->ranges) {
      q.push(buffered_pst_node(r.node_id, buffer_size, root));
      dot_file << "0 -> " << std::to_string(r.node_id) << "[label=\"" << r << "\"]\n";
    }
    while (!q.empty()) {
      buffered_pst_node bpn = q.front(); q.pop();
      bpn.load_all();
      dot_file << std::to_string(bpn.id) << " [label=\" " << std::to_string(bpn.id) << "\npb: ";
      for (point p : bpn.point_buffer) dot_file << p << ", ";
      dot_file << "\nib: ";
      for (point p : bpn.insert_buffer) dot_file << p << ", ";
      dot_file << "\"]\n";
      for (auto r : bpn.ranges) {
        q.push(buffered_pst_node(r.node_id, buffer_size, root));
        dot_file << std::to_string(bpn.id) << " -> " << std::to_string(r.node_id) << "[label=\"" << r << "\"]\n";
      }
    }
    dot_file << "}";
    dot_file.close();
    int r = system("dot -Tpng -o tree.png temp.dot");
    //r = system("eog tree.png");
    r++;
  }

  buffered_pst::~buffered_pst() {
    //TODO: should delete all files of children
    for (int i=0; i < next_id; i++) {
      DEBUG_MSG("Descructing file " << i);
      util::remove_directory(std::to_string(i));
    }
  }

#ifdef DEBUG
  bool buffered_pst::is_valid() {
    return root->is_valid();
  }
#endif
  
};

#endif
