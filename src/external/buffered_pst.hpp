#ifndef BUFFERED_PST_HPP
#define BUFFERED_PST_HPP
#include "../common/point.hpp"
#include "../common/debug.hpp"
#include "../common/utilities.hpp"
#include "../common/definitions.hpp"
#include "../internal/rb_tree.hpp"
#include "range.hpp"
#include "child_structure_interface.hpp"
#include "child_structure.hpp"
#include "child_structure_stub.hpp"
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
#include <stack>
#include <tuple>
#include <deque>

namespace ext {
  int next_id = 1;
#ifdef DEBUG
  std::set<point> CONTAINED_POINTS;
#endif
  class buffered_pst {
  public:
    buffered_pst(size_t buffer_size, double epsilon);
    buffered_pst(size_t buffer_size, double epsilon,
                 std::string file_name, bool is_sorted);
    ~buffered_pst();
    void insert(const point &p);
    void remove(const point &p);
    void report(int x1, int x2, int y, const std::string &output_file);
    void print();
    struct global_rebuild_configuration {
      global_rebuild_configuration()
        : start_rebuild_at(0), rebuild_factor(0.5) {}
      global_rebuild_configuration(size_t _start_rebuild_at, double _rebuild_factor)
        : start_rebuild_at(_start_rebuild_at), rebuild_factor(_rebuild_factor) {}
      ~global_rebuild_configuration() {};
      size_t start_rebuild_at;
      double rebuild_factor;
    };
    void set_global_rebuild_configuration(const global_rebuild_configuration &config);
#ifdef DEBUG
    bool is_valid();
#endif
  private:
    class buffered_pst_node {
    public:
      buffered_pst_node(int id, int parent_id, size_t buffer_size,
                        size_t B_epsilon,double epsilon, buffered_pst_node* root);
      buffered_pst_node(int id, size_t buffer_size,double epsilon, buffered_pst_node* root);
      ~buffered_pst_node();
      int id;
      void add_child(const range &r);
      void insert_into_point_buffer(const point &p);
      void load_point_buffer();
      void load_insert_buffer();
      void load_delete_buffer();
      void load_info_file();
      void load_ranges();
      void load_child_structure();
      void load_all();
      void flush_point_buffer();
      void flush_insert_buffer();
      void flush_delete_buffer();
      void flush_info_file();
      void flush_ranges();
      void flush_child_structure();
      void flush_all();
      template <class Container>
      void insert_into_point_buffer(const Container &points);
      void insert_into_delete_buffer(const point &p);
      template <class Container>
      void insert_into_delete_buffer(const Container &points);
      void insert_into_insert_buffer(const point &p);
      template <class Container>
      void insert_into_insert_buffer(const Container &points);
      bool is_leaf();
      bool is_root();
      bool is_point_buffer_loaded;
      bool is_insert_buffer_loaded;
      bool is_delete_buffer_loaded;
      bool is_info_file_loaded;
      bool is_ranges_loaded;
      bool is_child_structure_loaded;
      bool insert_buffer_overflow();
      bool delete_buffer_overflow();
      bool point_buffer_overflow();
      bool point_buffer_underflow();
      bool node_degree_overflow();
      bool is_virtual_leaf();
      std::set<point> insert_buffer, delete_buffer, point_buffer;
      ext::child_structure_interface *child_structure;
      internal::rb_tree<range> ranges;
      int parent_id;
#ifdef DEBUG
      bool is_valid();
#endif
    private:
      std::pair<int,std::set<point> > find_child(const std::set<point> &buffer);
      std::string get_point_buffer_file_name(int id);
      std::string get_insert_buffer_file_name(int id);
      std::string get_delete_buffer_file_name(int id);
      std::string get_info_file_file_name(int id);
      std::string get_ranges_file_name(int id);
      std::string get_directory(int id);
      size_t buffer_size;
      size_t B_epsilon;
      double epsilon;
      buffered_pst_node* root;
    };
    //methods of article
    void handle_point_buffer_overflow_in_root(buffered_pst_node* node);
    void handle_insert_buffer_overflow(buffered_pst_node* node, buffered_pst_node* child);
    void handle_insert_buffer_overflow_in_leaf_and_virtual_leaf
    (buffered_pst_node* leaf, buffered_pst_node* parent);
    void handle_delete_buffer_overflow(buffered_pst_node* node, buffered_pst_node* child);
    void handle_point_buffer_underflow(buffered_pst_node* parent,
                                       buffered_pst_node* node,
                                       const std::vector<buffered_pst_node*> &children);
    void handle_node_degree_overflow(buffered_pst_node* node, buffered_pst_node* parent,
                                     std::map<int, buffered_pst_node*> &children);
    /// HELPER METHODS
    void handle_events();
    void handle_global_rebuild();
    buffered_pst_node* copy_node(buffered_pst_node* node);
    buffered_pst_node* find_child(buffered_pst_node* node, const std::set<point> &buffer);
    void handle_delete_buffer_overflow_in_leaf_and_virtual_leaf(buffered_pst_node* node);
    void handle_point_buffer_overflow_of_leaf_root();
    void handle_point_buffer_underflow_in_children(buffered_pst_node* node);
    void handle_point_buffer_underflow_in_virtual_leaf(buffered_pst_node* node,
                                                       buffered_pst_node* parent);
    void split_leaf(buffered_pst_node* node, buffered_pst_node* parent);
    void handle_point_buffer_overflow_in_virtual_leaf(buffered_pst_node* parent,
                                                      buffered_pst_node* node,
                                                      std::map<int,buffered_pst_node*> &children);
    void flush_buffers_to_child(buffered_pst_node* node,
                                std::deque<buffered_pst_node*> &q,
                                std::deque<buffered_pst_node*> &fix_up_queue,
                                int x1, int x2, int y);
    void handle_invalid_buffer_events(std::deque<buffered_pst_node*> &q);
    void global_rebuild();
    void construct_sorted(const std::string &file_name);
    enum struct EVENT {
      insert_buffer_overflow,
      delete_buffer_overflow,
      point_buffer_overflow,
      node_degree_overflow,
      point_buffer_underflow,
      point_buffer_underflow_construct,
      point_buffer_underflow_full_children,
      split_leaf
    };
    enum struct STATE {
      normal,
      fix_up,
      global_rebuild,
      construct
    };
    std::string event_to_string(const EVENT &e);
    std::stack<std::pair<buffered_pst_node*, EVENT> > event_stack;
    global_rebuild_configuration global_rebuild_config;
    size_t buffer_size, operation_count, epoch_begin_point_count;
    int B_epsilon;
    double epsilon;
    buffered_pst_node* root;
    int epoch_begin;
    STATE state;
    int parent_to_stop_at;
  };

  // BUFFERED_PST_NODE /////////////////////////////////////////////////////
  buffered_pst::buffered_pst_node::buffered_pst_node(int id, int parent_id, size_t _buffer_size, size_t B_epsilon, double epsilon, buffered_pst_node* root) : buffer_size(_buffer_size)  {
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
    child_structure = new ext::child_structure(id, buffer_size, epsilon, std::vector<point>());
    is_child_structure_loaded = true;
    this->B_epsilon = B_epsilon;
    DEBUG_MSG("Constructed pst_node with id " << id << " and buffer_size: " << buffer_size
              << " and B_epsilon " << B_epsilon << " and parent_id " << parent_id);
  }

  buffered_pst::buffered_pst_node::buffered_pst_node(int id, size_t buffer_size,
                                                     double epsilon,
                                                     buffered_pst_node* root) {
    DEBUG_MSG("Opening existing node " << id);
    this->id = id;
    this->root = root;
    this->buffer_size = buffer_size;
    this->epsilon = epsilon;
    is_insert_buffer_loaded = false;
    is_delete_buffer_loaded = false;
    is_point_buffer_loaded = false;
    is_ranges_loaded = false;
    is_info_file_loaded = false;
    is_child_structure_loaded = false;
    child_structure = 0;
  }

  buffered_pst::buffered_pst_node::~buffered_pst_node() {
    DEBUG_MSG("DESTRUCTING NODE " << id);
    if (child_structure) delete child_structure;
    //assert(!is_child_structure_loaded);
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

  void buffered_pst::buffered_pst_node::load_child_structure() {
    if (is_root()) return;
    assert(!is_child_structure_loaded);
    DEBUG_MSG("Loading child structure for node " << id);
    assert(!child_structure);
    child_structure = new ext::child_structure(id);
    is_child_structure_loaded = true;
  }

  void buffered_pst::buffered_pst_node::load_all() {
    load_point_buffer();
    load_insert_buffer();
    load_delete_buffer();
    load_info_file();
    load_ranges();
    load_child_structure();
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
    DEBUG_MSG_FAIL("Flushing ranges for node " << id);
#ifdef DEBUG
    for (range r : ranges)
      DEBUG_MSG_FAIL(" - " << r);
#endif
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

  void buffered_pst::buffered_pst_node::flush_child_structure() {
    if (is_root()) return;
    DEBUG_MSG("Flushing child structure for node " << id);
    delete child_structure;
    child_structure = 0;
    is_child_structure_loaded = false;
  }

  void buffered_pst::buffered_pst_node::flush_all() {
    if (is_point_buffer_loaded) flush_point_buffer();
    if (is_insert_buffer_loaded) flush_insert_buffer();
    if (is_delete_buffer_loaded) flush_delete_buffer();
    if (is_info_file_loaded) flush_info_file();
    if (is_ranges_loaded) flush_ranges();
    if (is_child_structure_loaded) flush_child_structure();
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

    //if (insert_buffer_overflow()) handle_insert_buffer_overflow();
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

#ifdef DEBUG
    DEBUG_MSG_FAIL("Point buffer now contains the following points in node " << id);
    for (point p : point_buffer)
      DEBUG_MSG_FAIL(" - " << p);
#endif
    //if (point_buffer_overflow()) handle_point_buffer_overflow();
  }

  void buffered_pst::buffered_pst_node::insert_into_delete_buffer(const point &p) {
    std::vector<point> points;
    points.push_back(p);
    insert_into_delete_buffer(points);
  }

  template <class Container>
  void buffered_pst::buffered_pst_node::insert_into_delete_buffer(const Container &points) {
    assert(is_delete_buffer_loaded);
    DEBUG_MSG("inserting point(s) into delete buffer of node " << id);

#ifdef DEBUG
    for (auto p : points) DEBUG_MSG(" - " << p);
#endif

    delete_buffer.insert(points.begin(), points.end());

    //if (delete_buffer_overflow()) handle_delete_buffer_overflow();
  }

  bool buffered_pst::buffered_pst_node::point_buffer_overflow() {
    assert(is_point_buffer_loaded);
    assert(is_ranges_loaded);
    if ( is_leaf() && !is_root() ) return point_buffer.size() >= buffer_size/2;
    return point_buffer.size() > buffer_size;
  }

  bool buffered_pst::buffered_pst_node::point_buffer_underflow() {
    assert(is_point_buffer_loaded);
    assert(is_ranges_loaded);
    if ( is_leaf() || is_virtual_leaf() ) return false;
    return point_buffer.size() < buffer_size/2;
  }

  bool buffered_pst::buffered_pst_node::is_virtual_leaf() {
    assert(is_ranges_loaded);
    bool is_virtual_leaf = true;
    for (auto r : ranges) if (r.max_y != INF) is_virtual_leaf = false;
    return is_virtual_leaf;
  }

  bool buffered_pst::buffered_pst_node::insert_buffer_overflow() {
    assert(is_insert_buffer_loaded);
    assert(is_ranges_loaded);
    if (is_leaf() || is_virtual_leaf()) return !insert_buffer.empty();
    return insert_buffer.size() > buffer_size;
  }
  
  bool buffered_pst::buffered_pst_node::delete_buffer_overflow() {
    assert(is_delete_buffer_loaded);
    assert(is_ranges_loaded);
    if (is_leaf() || is_virtual_leaf()) return !delete_buffer.empty();
    return delete_buffer.size() > buffer_size/4;
  }

  bool buffered_pst::buffered_pst_node::node_degree_overflow() {
    assert(is_info_file_loaded);
    assert(is_ranges_loaded);
    return ranges.size() > B_epsilon;
  }

  bool buffered_pst::buffered_pst_node::is_leaf() {
    assert(is_ranges_loaded);
    return ranges.empty();
  }

  bool buffered_pst::buffered_pst_node::is_root() {
    return id == 0;
  }

  void buffered_pst::buffered_pst_node::add_child(const range &r) {
    DEBUG_MSG("Adding range " << r << " to node " << id);
    assert(is_ranges_loaded);
    ranges.insert(r);
  }


#ifdef DEBUG
  bool buffered_pst::buffered_pst_node::is_valid() {

    DEBUG_MSG("STARTING IS VALID TEST");
    load_all();
    
    // TODO check all invariants. Recurse on children.
    if ( insert_buffer_overflow() ) {
      DEBUG_MSG_FAIL("Insert buffer overflow in node " << id);
      return false;
    }
    if ( delete_buffer_overflow() ) {
      DEBUG_MSG_FAIL("Delete buffer overflow in node " << id);
      return false;
    }
    if ( point_buffer_overflow() ) {
      DEBUG_MSG_FAIL("Point buffer overflow in node " << id);
      return false;
    }
    if ( point_buffer_underflow() ) {
      DEBUG_MSG_FAIL("Point buffer underflow in node " << id);
      return false;
    }

    if ( ( is_leaf() || is_virtual_leaf() ) && (!delete_buffer.empty() )) {
      DEBUG_MSG_FAIL("We are a leaf or a virtual leaf without empty delete or insert buffer " << id);
      DEBUG_MSG("Delete buffer contains");
      for (point p : delete_buffer) DEBUG_MSG(" - " << p);
      DEBUG_MSG("Insert buffer contains");
      for (point p : insert_buffer) DEBUG_MSG(" - " << p);
      return false;
    }

    if (is_root()) {
      for (range r : ranges)
        if (r.node_id == 0) {
          DEBUG_MSG_FAIL("We have node id 0 range in root");
          for (range r : ranges) DEBUG_MSG_FAIL(" - " << r);
          return false;
        }
    }

    // if ( is_leaf() && point_buffer.empty() && !is_root())  {
    //   DEBUG_MSG_FAIL("We are an empty leaf... we should have been deleted " << id);
    //   return false;
    // }

    // no duplicates in Pv, Iv, Dv
    std::vector<point> intersection;
    //insert \cap delete = OE
    std::set_intersection(insert_buffer.begin(), insert_buffer.end(),
                          delete_buffer.begin(), delete_buffer.end(),
                          std::back_inserter(intersection));
    if (!intersection.empty()) {
      DEBUG_MSG_FAIL("Duplicate in insert buffer and delete buffer");
      for (point p : intersection)
        DEBUG_MSG_FAIL(p);
      return false;
    }
    intersection.clear();
    //insert \cap point = OE
    std::set_intersection(insert_buffer.begin(), insert_buffer.end(),
                          point_buffer.begin(), point_buffer.end(),
                          std::back_inserter(intersection));
    if (!intersection.empty()) {
      DEBUG_MSG_FAIL("Duplicate in insert buffer and point buffer");
      for (point p : intersection)
        DEBUG_MSG_FAIL(p);
      return false;
    }
    intersection.clear();
    // delete \cap point = OE
    std::set_intersection(delete_buffer.begin(), delete_buffer.end(),
                          point_buffer.begin(), point_buffer.end(),
                          std::back_inserter(intersection));
    if (!intersection.empty()) {
      DEBUG_MSG_FAIL("Duplicate in insert buffer and point buffer");
      for (point p : intersection)
        DEBUG_MSG_FAIL(p);
      return false;
    }

    // Iv U Dv have y-value less than the points in Pv
    std::vector<point> union_iv_dv;
    std::set_union(insert_buffer.begin(), insert_buffer.end(),
                   delete_buffer.begin(), delete_buffer.end(),
                   std::back_inserter(union_iv_dv));
    point min_y = *std::min_element(point_buffer.begin(), point_buffer.end(),
                                    comp_y);
    if (std::any_of(union_iv_dv.begin(), union_iv_dv.end(),
                    [&min_y] (point p) {
                      return min_y.y < p.y || (min_y.y == p.y && min_y.x < p.x);
                    })) {
      DEBUG_MSG_FAIL("A point in Iv or Dv has larger y value than a point in Pv on node " << id);
      DEBUG_MSG_FAIL("Points in Iv:");
      for (auto p : insert_buffer) DEBUG_MSG_FAIL(" - " << p);
      DEBUG_MSG_FAIL("Points in Dv:");
      for (auto p : delete_buffer) DEBUG_MSG_FAIL(" - " << p);
      DEBUG_MSG_FAIL("Points in Pv:");
      for (auto p : point_buffer) DEBUG_MSG_FAIL(" - " << p);
      return false;
    }

    // Point buffer should not have points that is also in delete buffer
    for (point p : point_buffer) {
      if (delete_buffer.find(p) != delete_buffer.end()) {
        DEBUG_MSG_FAIL("Point " << p << " is both in delete_buffer and point_buffer at node "
                       << id);
        return false;
      }
    }

    // Insert buffer should not have points that is also in delete buffer
    for (point p : insert_buffer) {
      if (delete_buffer.find(p) != delete_buffer.end()) {
        DEBUG_MSG_FAIL("Point " << p << " is both in delete_buffer and insert_buffer at node "
                       << id);
        return false;
      }
    }
    
    // tests ranges are correct w.r.t to points in point buffer
    for (auto it = ranges.begin(); it != ranges.end(); it++) {
      io::buffered_stream<point> bs(4096);
      bs.open(get_point_buffer_file_name(it->node_id));
      while (!bs.eof()) {
        point p = bs.read();
        if (ranges.belong_to(range(p,0,0)) != *it) {
          DEBUG_MSG_FAIL("point " << p << " is in the wrong child " << it->node_id);
          DEBUG_MSG_FAIL("child " << it->node_id << " is responsible for " << *it
                         << " but belong to " << ranges.belong_to(range(p,0,0)));
          return false;
        }
        if (p.y > it->max_y) {
          DEBUG_MSG_FAIL("point " << p << " has y-value larger than range max_y " << it->max_y);
          return false;
        }
      }
      bs.close();
    }

    if (ranges.size() == 0 && is_leaf() != true) {
      DEBUG_MSG_FAIL("We have children but is a leaf in node: " << id);
      return false;
    }

    if (ranges.size() != 0 && is_leaf() != false) {
      DEBUG_MSG_FAIL("We dont have children but we are not a leaf! in node: " << id);
      return false;
    }

    // tests the heap order on point buffers
    DEBUG_MSG("Test the heap order on " << id);
    for (auto r : ranges) {
      DEBUG_MSG("Opening child " << r.node_id);
      buffered_pst_node child(r.node_id, buffer_size,epsilon,root);
      child.load_point_buffer();
      if (std::any_of(child.point_buffer.begin(), child.point_buffer.end(),
                      [&min_y] (point p) {
                        return min_y.y < p.y || (min_y.y == p.y && min_y.x < p.x);
                      })) {
        DEBUG_MSG_FAIL("A point in child has larger y value than a point in Pv");
        DEBUG_MSG_FAIL("We have points: ");
        for (point p : point_buffer) DEBUG_MSG_FAIL(" - " << p);
        DEBUG_MSG_FAIL("Child has points:");
        for (point p : child.point_buffer) DEBUG_MSG_FAIL(" - " << p);
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
        q.push(buffered_pst_node(r.node_id, buffer_size,epsilon, root));
      }

      while (!q.empty()) {
        buffered_pst_node bpn = q.front(); q.pop();
        bpn.load_all();
        for (point p : bpn.insert_buffer) collected_points.insert(p);
        for (point p : bpn.point_buffer) collected_points.insert(p);
        for (range r : bpn.ranges) q.push(buffered_pst_node(r.node_id, buffer_size, epsilon, root));
      }
      if (collected_points != CONTAINED_POINTS) {
        DEBUG_MSG_FAIL("CONTAINED_POINTS:");
        for (point p : CONTAINED_POINTS) DEBUG_MSG_FAIL(" - " << p);
        DEBUG_MSG_FAIL("collected_points:");
        for (point p : collected_points) DEBUG_MSG_FAIL(" - " << p);
        DEBUG_MSG_FAIL("The collected points in our structure is not equal to the inserted points");
        std::vector<point> diff;
        std::set_difference(CONTAINED_POINTS.begin(), CONTAINED_POINTS.end(),
                            collected_points.begin(), collected_points.end(),
                            std::back_inserter(diff));
        for (point p : diff) DEBUG_MSG_FAIL(" - " << p);
        return false;
      }
    }

    //Test if all points in the subtree is in the correct x range
    for (auto r : ranges) {
      std::set<point> points_in_subtree;
      std::queue<buffered_pst_node> q;
      q.push(buffered_pst_node(r.node_id, buffer_size,epsilon, root));
      while (!q.empty()) {
        buffered_pst_node bpn = q.front(); q.pop();
        bpn.load_all();
        for (point p : bpn.point_buffer) points_in_subtree.insert(p);
        for (point p : bpn.insert_buffer) points_in_subtree.insert(p);
        for (range ra : bpn.ranges) q.push(buffered_pst_node(ra.node_id, buffer_size,epsilon, root));
      }
      for (point p : points_in_subtree) {
        if (ranges.belong_to(range(p,-1,-1)) != r) {
          DEBUG_MSG_FAIL("Point " << p << " was found not belonging to this subtree but was found to belong to " << ranges.belong_to(range(p,-1,-1)));
          for (range ra : ranges) DEBUG_MSG_FAIL(" - " << ra);
          return false;
        }
      }
    }

    // testing correct max y in child
    for (auto r : ranges) {
      DEBUG_MSG("Opening child " << r.node_id);
      buffered_pst_node child(r.node_id,buffer_size,epsilon,root);
      child.load_all();

      int max_y = std::max_element(child.point_buffer.begin(),
                                   child.point_buffer.end(),
                                   comp_y)->y;

      if (child.point_buffer.empty()) max_y = INF;
      
      if (max_y != r.max_y) {
        DEBUG_MSG_FAIL("max_y " << r.max_y << " in range of node " << id << " is not equal to "
                       << "maximum y " << max_y << " in point buffer of child " << r.node_id);
        return false;
      }
    }

    // Tests that the child structure holds the same elements as the children
    std::vector<point> child_structure_points = child_structure->get_points();
    std::vector<point> collected_points;
    for (auto r : ranges) {
      DEBUG_MSG("Opening child " << r.node_id);
      buffered_pst_node child(r.node_id, buffer_size, epsilon, root);
      child.load_point_buffer();
      for (point p : child.point_buffer) DEBUG_MSG(" - " << p);
      collected_points.insert(collected_points.end(), child.point_buffer.begin(),
                              child.point_buffer.end());
    }
    if (child_structure_points != collected_points) {
      DEBUG_MSG_FAIL("Child structure of node " << id << " does not have the same points as the children");
      DEBUG_MSG_FAIL("points in child_structure:");
      for (point p : child_structure_points) DEBUG_MSG_FAIL(" - " << p);
      DEBUG_MSG_FAIL("points in children:");
      for (point p : collected_points) DEBUG_MSG_FAIL(" - " << p);
      return false;
    }

    if (ranges.size() > B_epsilon) {
      DEBUG_MSG_FAIL("We have too many children in node " << id);
      for (range r : ranges) DEBUG_MSG_FAIL(" - " << r);
      return false;
    }
    
    // ALWAYS AT THE BUTT
    for (auto r : ranges) {
      DEBUG_MSG("Opening child " << r.node_id);
      buffered_pst_node child(r.node_id,buffer_size,epsilon,root);
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
    this->epoch_begin_point_count = 0;
    this->operation_count = 0;
    this->global_rebuild_config = global_rebuild_configuration();
    this->parent_to_stop_at = -1;
    this->state = STATE::normal;
    this->buffer_size = buffer_size;
    this->epsilon = epsilon;
    this->epoch_begin = 0;
    this->B_epsilon = (epsilon == 0.5) ? (int)sqrt(buffer_size) :
      (int)pow((double)buffer_size,epsilon);
    root = new buffered_pst_node(0,0,buffer_size, B_epsilon, epsilon, 0);
    DEBUG_MSG("Constructing buffered_pst with buffer_size: "
              << buffer_size << " epsilon: " << epsilon << " B_epsilon: " << B_epsilon);
    next_id = 1;
#ifdef DEBUG
    CONTAINED_POINTS.clear();
#endif
  }

  buffered_pst::buffered_pst(size_t buffer_size, double epsilon,
                             std::string file_name, bool is_sorted)
    : buffered_pst(buffer_size, epsilon)
  {
    if (is_sorted) construct_sorted(file_name);
    //else construct_unsorted(file_name);
  }

  void buffered_pst::insert(const point &p) {
    DEBUG_MSG("Inserting point " << p << " into root");
#ifdef DEBUG
    if (state == STATE::normal)
      CONTAINED_POINTS.insert(p);
#endif
    if (root->is_leaf()) {
      root->insert_into_point_buffer(p);
      if (state == STATE::normal || state == STATE::global_rebuild)
        event_stack.push({root, EVENT::point_buffer_overflow});
    } else {
      DEBUG_MSG("remove duplicates of p from Pr, Ir, Dr");
      if (root->point_buffer.erase(p)) {
        root->point_buffer.insert(p);
        return;
      }
      root->insert_buffer.erase(p);
      root->delete_buffer.erase(p);
      DEBUG_MSG("Check if put into Ir or Pr");
      point min_y = *std::min_element(root->point_buffer.begin(), root->point_buffer.end(), comp_y);
      if (p.y < min_y.y || (p.y == min_y.y && p.x < min_y.x)) {
        root->insert_into_insert_buffer(p);
        if (state == STATE::normal || state == STATE::global_rebuild)
          event_stack.push({root, EVENT::insert_buffer_overflow});
      } else {
        root->insert_into_point_buffer(p);
        if (state == STATE::normal || state == STATE::global_rebuild)
          event_stack.push({root, EVENT::point_buffer_overflow});
      }
    }
    handle_events();
    handle_global_rebuild();
  }

  void buffered_pst::handle_global_rebuild() {
    DEBUG_MSG("Starting to handle global rebuild");
    if (state == STATE::global_rebuild) return;
    operation_count++;
    DEBUG_MSG_FAIL("epoch_begin_point_count " << epoch_begin_point_count << " "
                   << "operation count " << operation_count);
    if (global_rebuild_config.start_rebuild_at >=
        epoch_begin_point_count + operation_count) return;
    if (operation_count >=
        (size_t)((double)epoch_begin_point_count * global_rebuild_config.rebuild_factor))
      global_rebuild();
  }
  
  void buffered_pst::remove(const point &p) {
    DEBUG_MSG("Removing point " << p << " in root");
#ifdef DEBUG
    if (state == STATE::normal) {
      if (root->point_buffer.find(p) != root->point_buffer.end()) CONTAINED_POINTS.erase(p);
      if (root->insert_buffer.find(p) != root->insert_buffer.end()) CONTAINED_POINTS.erase(p);
    }
#endif
    point min_y = *std::min_element(root->point_buffer.begin(),
                                    root->point_buffer.end(),comp_y);
    
    root->point_buffer.erase(p);
    root->insert_buffer.erase(p);
    root->delete_buffer.erase(p);
    
    if (state == STATE::normal || state == STATE::global_rebuild)
      event_stack.push({root, EVENT::point_buffer_underflow});
    
    if (!root->is_leaf() && (p.y < min_y.y || (p.y == min_y.y && p.x < min_y.x))) {
      DEBUG_MSG("Deletion " << p << " is smaller than min_y " << min_y
                << ". Inserting into deletion_buffer of root");
      root->insert_into_delete_buffer(p);
      if (state == STATE::normal || state == STATE::global_rebuild)
        event_stack.push({root, EVENT::delete_buffer_overflow});
    }

    handle_events();
    handle_global_rebuild();
    //root->handle_underflowing_point_buffer();
  }

  void buffered_pst::set_global_rebuild_configuration
  (const global_rebuild_configuration &config) {
    global_rebuild_config = config;
  }

  /*
    This is the main event loop
  */
  // FUN_HANDLE_EVENTS
  void buffered_pst::handle_events() {
    std::pair<buffered_pst_node*, EVENT> prev_event = {0, EVENT::point_buffer_overflow};


    while (!event_stack.empty()) {
      auto cur_event = event_stack.top();
      event_stack.pop();
      buffered_pst_node* node;
      EVENT event;

      std::tie(node, event) = cur_event;
      DEBUG_MSG("STARTING TO HANDLE EVENT: " << event_to_string(event) << " in node " <<
                node->id);
      switch (cur_event.second) {
        
      case EVENT::point_buffer_overflow:
        {
          node->load_ranges();
          node->load_point_buffer();
          if (!node->point_buffer_overflow()) {
            node->flush_ranges();
            node->flush_point_buffer();
            break;
          }

          if ( node->is_root() && node->is_leaf() )
            handle_point_buffer_overflow_of_leaf_root();
          else if ( node->is_root() )
            handle_point_buffer_overflow_in_root(node);
          else if ( node->is_leaf() ) {
            node->load_info_file();
            buffered_pst_node* parent = node->parent_id == 0
              ? root
              : new buffered_pst_node(node->parent_id,buffer_size,epsilon,root);
            parent->load_ranges();
            
            split_leaf(node, parent);
            
            parent->flush_ranges();
            if (!parent->is_root()) delete parent;
            node->flush_info_file();
          } else if ( node->is_virtual_leaf() ) {
            node->load_info_file();
            node->load_child_structure();
            buffered_pst_node* parent = node->parent_id == 0
              ? root
              : new buffered_pst_node(node->parent_id,buffer_size,epsilon,root);
            parent->load_child_structure();
            
            std::map<int, buffered_pst_node*> children;
            for (range r : node->ranges) {
              buffered_pst_node* child = new buffered_pst_node(r.node_id, buffer_size, epsilon, root);
              children[r.node_id] = child;
              child->load_point_buffer();
            }

            handle_point_buffer_overflow_in_virtual_leaf(parent, node, children);

            for (auto c : children) {
              c.second->flush_point_buffer();
              delete c.second;
            }
            
            parent->flush_child_structure();

            node->flush_child_structure();
            node->flush_info_file();
          } else assert(true==false);
          
          node->flush_ranges();
          node->flush_point_buffer();
        }
        break;
      case EVENT::insert_buffer_overflow:
        {
          node->load_insert_buffer();
          node->load_ranges();
          if ( !node->insert_buffer_overflow() ) {
            node->flush_insert_buffer();
            node->flush_ranges();
            break;
          }
          node->load_point_buffer();
          node->load_info_file();
          if ( node->is_leaf() || node->is_virtual_leaf() ) {
            buffered_pst_node* parent = node->parent_id == 0
              ? root
              : new buffered_pst_node(node->parent_id,buffer_size,epsilon,root);
            parent->load_child_structure();
            parent->load_ranges();
            handle_insert_buffer_overflow_in_leaf_and_virtual_leaf(node, parent);
            parent->flush_child_structure();
            parent->flush_ranges();
            if (!parent->is_root()) delete parent;
          } else {
            node->load_delete_buffer();
            node->load_child_structure();
            while (node->insert_buffer_overflow()) {
              buffered_pst_node* child = find_child(node, node->insert_buffer);
              child->load_all();
              handle_insert_buffer_overflow(node, child);
              child->flush_all();
              delete child;
            }
            node->flush_delete_buffer();
            node->flush_child_structure();
          }
          node->flush_info_file();
          node->flush_ranges();
          node->flush_insert_buffer();
          node->flush_point_buffer();
        }
        break;
      case EVENT::delete_buffer_overflow:
        {
          node->load_delete_buffer();
          node->load_ranges();
          
          if ( !node->delete_buffer_overflow() ) {
            node->flush_delete_buffer();
            node->flush_ranges();
            break;
          }
          
          if (node->is_leaf() || node->is_virtual_leaf()) {
            handle_delete_buffer_overflow_in_leaf_and_virtual_leaf(node);
          } else {
            node->load_point_buffer();
            node->load_insert_buffer();
            node->load_info_file();
            node->load_child_structure();
            while (node->delete_buffer_overflow()) {
              buffered_pst_node* child = find_child(node, node->delete_buffer);
              child->load_ranges();
              child->load_point_buffer();
              child->load_delete_buffer();
              child->load_insert_buffer();
              handle_delete_buffer_overflow(node, child);
              child->flush_ranges();
              child->flush_point_buffer();
              child->flush_delete_buffer();
              child->flush_insert_buffer();
              delete child;
            }
          }
          node->flush_all();
        }
        break;
      case EVENT::node_degree_overflow:
        {
          node->load_ranges();
          node->load_info_file();
          if ( !node->node_degree_overflow() ) {
            node->flush_ranges();
            node->flush_info_file();
            break;
          }
          node->load_point_buffer();
          node->load_insert_buffer();
          node->load_delete_buffer();
          node->load_child_structure();

          buffered_pst_node* parent = node->parent_id == 0
            ? root
            : new buffered_pst_node(node->parent_id,buffer_size,epsilon,root);
          parent->load_ranges();
          std::map<int, buffered_pst_node*> children;
          for (range r : node->ranges) {
            buffered_pst_node* child = new buffered_pst_node(r.node_id, buffer_size,
                                                             epsilon, root);
            children[r.node_id] = child;
            child->load_point_buffer();
            child->load_info_file();
          }
          handle_node_degree_overflow(node, parent, children);
          node->flush_all();
          parent->flush_ranges();
          if (!parent->is_root()) delete parent;
          for (auto c : children) {
            c.second->flush_point_buffer();
            c.second->flush_info_file();
            delete c.second;
          }
        }
        break;
      case EVENT::point_buffer_underflow_construct:
      case EVENT::point_buffer_underflow:
        {
          node->load_ranges();
          node->load_point_buffer();
          
          if ( event == EVENT::point_buffer_underflow &&
               !node->point_buffer_underflow() ) {
           node->flush_ranges();
           node->flush_point_buffer();
           break;
          }

          if ( node->is_leaf() || node->is_virtual_leaf() ) {
           
          } else {
            if (state == STATE::normal || state == STATE::fix_up
                || state == STATE::global_rebuild || state == STATE::construct) {
              event_stack.push({copy_node(node), EVENT::point_buffer_underflow_full_children});
              handle_point_buffer_underflow_in_children(node);
            }
          }
          node->flush_point_buffer();
          node->flush_ranges();
        }
        break;
      case EVENT::point_buffer_underflow_full_children:
        {
          node->load_all();
          buffered_pst_node* parent = node->parent_id == 0
            ? root
            : new buffered_pst_node(node->parent_id,buffer_size,epsilon,root);
          parent->load_child_structure();
          parent->load_ranges();
          
          std::vector<buffered_pst_node*> children;
          for (range r : node->ranges) {
            //TODO if max_y...
            buffered_pst_node* child =
              new buffered_pst_node(r.node_id, buffer_size,
                                    epsilon, root);
            child->load_point_buffer();
            child->load_ranges();
            children.push_back(child);
          }
          handle_point_buffer_underflow(parent, node, children);
          parent->flush_child_structure();
          parent->flush_ranges();
          if (!parent->is_root()) delete parent;
          for (auto c : children) {
            c->flush_point_buffer();
            c->flush_ranges();
            delete c;
          }
           if (state == STATE::normal || state == STATE::fix_up
               || state == STATE::global_rebuild || state == STATE::construct)
             handle_point_buffer_underflow_in_children(node);
          node->flush_all();
        }
        break;
      case EVENT::split_leaf:
        {
          node->load_point_buffer();
          node->load_info_file();
          buffered_pst_node* parent = node->parent_id == 0
            ? root
            : new buffered_pst_node(node->parent_id,buffer_size,epsilon,root);
          parent->load_ranges();
          split_leaf(node, parent);
          parent->flush_ranges();
          if (!parent->is_root()) delete parent;
          node->flush_point_buffer();
          node->flush_info_file();
        }
        break;
      default:
        DEBUG_MSG_FAIL("Illegal event! " << event_to_string(event));
        break;
      }
      if (!node->is_root()) delete node;
      prev_event = cur_event;
    }

  }

  std::string buffered_pst::event_to_string(const EVENT &e) {
    switch (e) {
    case EVENT::point_buffer_overflow:
      return "point buffer overflow";
    case EVENT::point_buffer_underflow:
      return "point buffer underflow";
    case EVENT::split_leaf:
      return "split leaf";
    case EVENT::insert_buffer_overflow:
      return "insert buffer overflow";
    case EVENT::delete_buffer_overflow:
      return "delete buffer overflow";
    case EVENT::node_degree_overflow:
      return "node degree overflow";
    case EVENT::point_buffer_underflow_full_children:
      return "point buffer underflow with full children";
    case EVENT::point_buffer_underflow_construct:
      return "point buffer underflow construct";
    default:
      return "ILLEGAL EVENT!!!";
    }
  }

  buffered_pst::buffered_pst_node* buffered_pst::copy_node(buffered_pst_node* node) {
    if (node->is_root()) return node;
    return new buffered_pst_node(node->id, buffer_size, epsilon, root);
  }

  /*
    Makes the initial split of the root before it has children
   */
  void buffered_pst::handle_point_buffer_overflow_of_leaf_root() {
    DEBUG_MSG("Starting to make the initial split of root");
    
    std::vector<point> points(root->point_buffer.begin(), root->point_buffer.end());
    std::sort(points.begin(),points.end(),comp_y);

    std::sort(points.begin(),points.begin()+points.size()/2);
    int child1_id = next_id++;
    int child2_id = next_id++;
    buffered_pst_node c1(child1_id, root->id, buffer_size, B_epsilon, epsilon, root);
    buffered_pst_node c2(child2_id, root->id, buffer_size, B_epsilon, epsilon, root);
    c1.insert_into_point_buffer(std::vector<point>(points.begin(),
                                                   points.begin()+points.size()/4));

    c2.insert_into_point_buffer(std::vector<point>(points.begin()+points.size()/4,
                                                   points.begin()+points.size()/2));
      
    root->point_buffer = std::set<point>(points.begin()+points.size()/2,points.end());

    DEBUG_MSG("Adding new child ranges to root " << root->id);
    root->add_child(range(*c1.point_buffer.begin(),
                          std::max_element(c1.point_buffer.begin(),
                                           c1.point_buffer.end(),
                                           comp_y)->y, child1_id));

    root->add_child(range(*c2.point_buffer.begin(),
                          std::max_element(c2.point_buffer.begin(),
                                           c2.point_buffer.end(),
                                           comp_y)->y, child2_id));

    DEBUG_MSG("Rebuild child structure of root in root leaf split" << root->id);
    for (auto it = points.begin(); it != points.begin()+points.size()/2; it++) {
      DEBUG_MSG("Inserting " << *it << " into child structure of root " << root->id);
      root->child_structure->insert(*it);
    }
      

#ifdef DEBUG
    DEBUG_MSG("Points in point_buffer of root " << root->id);
    for (auto p : root->point_buffer)
      DEBUG_MSG(p);
#endif
      
    c1.flush_all();
    c2.flush_all();

  }

  /*
    The node is split if it is a leaf or root without children
    If the node is the root the minimum element in the point buffer
    is moved to the insert buffer
    Invariant: Everything needed is loaded in advance in parent and child
   */
  void buffered_pst::handle_point_buffer_overflow_in_root(buffered_pst_node* node) {

    DEBUG_MSG("Starting to handle point buffer overflow");

#ifdef DEBUG
    assert(node->is_root());
#endif
      
    DEBUG_MSG("Overflow in the point buffer of the root. Move min point to insert buffer");
    point min_y = *std::min_element(node->point_buffer.begin(),
                                    node->point_buffer.end(),
                                    comp_y);
    node->point_buffer.erase(min_y);
    node->insert_into_insert_buffer(min_y);
   
    if (state == STATE::normal || state == STATE::fix_up
        || state == STATE::global_rebuild)
      event_stack.push({copy_node(node), EVENT::insert_buffer_overflow});
  }

  void buffered_pst::handle_point_buffer_overflow_in_virtual_leaf
  (buffered_pst_node* parent,
   buffered_pst_node* node,
   std::map<int,buffered_pst_node*> &children) {

    DEBUG_MSG("Point buffer overflow in virtual leaf " << node->id
              << ". Create U to send to children");

#ifdef DEBUG
    assert (node->is_point_buffer_loaded);
    assert (node->is_ranges_loaded);
    assert (node->is_child_structure_loaded);
    assert (parent->is_child_structure_loaded);
    for (auto c : children)
      assert (c.second->is_point_buffer_loaded);
    assert(node->point_buffer.size() > buffer_size);
#endif

    std::vector<point> sorted_point_buffer(node->point_buffer.begin(),
                                           node->point_buffer.end());
    std::sort(sorted_point_buffer.begin(), sorted_point_buffer.end(), comp_y);
    
    int points_to_move = std::max(node->point_buffer.size() / B_epsilon,
                                  node->point_buffer.size() - buffer_size);
    


    for (int i = 0; i < points_to_move; i++) {
      point p = sorted_point_buffer[i];
      parent->child_structure->remove(p);
      node->child_structure->insert(p);

      range r = node->ranges.belong_to(range(p, -1, -1));

      children[r.node_id]->point_buffer.insert(p);

      node->point_buffer.erase(p);
    }

    std::vector<range> new_ranges(node->ranges.begin(), node->ranges.end());
    for (size_t i = 0; i < new_ranges.size(); i++) {
      if (i == 0)
        new_ranges[i].min =
          std::min(new_ranges[i].min,
                   children[new_ranges[i].node_id]->point_buffer.empty() ? point(INF,INF) :
                   *std::min_element(children[new_ranges[i].node_id]->point_buffer.begin(),
                                     children[new_ranges[i].node_id]->point_buffer.end()));
      
      new_ranges[i].max_y = children[new_ranges[i].node_id]->point_buffer.empty() ? INF
        : std::max_element(children[new_ranges[i].node_id]->point_buffer.begin(),
                           children[new_ranges[i].node_id]->point_buffer.end(),
                           comp_y)->y;
    }
    node->ranges.clear();
    for (auto r : new_ranges) node->ranges.insert(r);
    
    for (auto c : children)
      if (state == STATE::fix_up || state == STATE::normal
          || state == STATE::global_rebuild)
        event_stack.push({copy_node(c.second), EVENT::point_buffer_overflow});
  }


  /*
    Handles insert buffer overflow in a leaf by moving
    all elements from insert buffer to point buffer
    and inserting the elements in the child structure of the parent
   */
  void buffered_pst::handle_insert_buffer_overflow_in_leaf_and_virtual_leaf
  (buffered_pst_node* leaf,
   buffered_pst_node* parent) {
    DEBUG_MSG("We, " << leaf->id << ", are a leaf. Move all elements of the insert buffer to point buffer");

#ifdef DEBUG
    assert(leaf->is_insert_buffer_loaded);
    assert(leaf->is_point_buffer_loaded);
    assert(parent->is_ranges_loaded);
    assert(parent->is_child_structure_loaded);
#endif
    
    std::vector<point> ib_temp(leaf->insert_buffer.begin(), leaf->insert_buffer.end());
    leaf->insert_buffer.clear();

    if ( !leaf->is_root() ) {
      for (point p : ib_temp) {
        parent->child_structure->insert(p);
      }
    }

    if (leaf->point_buffer.empty()) {
      int max_y = std::max_element(ib_temp.begin(), ib_temp.end(), comp_y)->y;
      if (ib_temp.empty()) max_y = INF;
      for (range r : parent->ranges) {
        if (r.node_id == leaf->id) {
          parent->ranges.erase(r);
          parent->ranges.insert(range(r.min, max_y, r.node_id));
          break;
        }
      }
    }

    leaf->insert_into_point_buffer(ib_temp);
    
    if (state == STATE::normal || state == STATE::fix_up
        || state == STATE::global_rebuild)
      event_stack.push({copy_node(leaf), EVENT::point_buffer_overflow});
  }

  /*
    - Find child to send U elements to of size enough to not overflow
    - Remove points in U from Iv, Ic, Dc, Pc and Cv (load child)
    - Any point in U with larger y-value than or equal to minimum value in Pc
    is inserted into Pc and Cv and removed from U
    - Handle recursive case if Pc overflows ( move points with smallest y-value from Pc to U)
    - if child is a leaf all points are inserted into Pc => might cause recursion as above
    - else add remaining points in U to Ic => might cause overflow
  */
  void buffered_pst::handle_insert_buffer_overflow(buffered_pst_node* node,
                                                   buffered_pst_node* child) {
    DEBUG_MSG("Starting to handle insert buffer overflow in " << node->id);
#ifdef DEBUG
    assert(node->is_insert_buffer_loaded);
    assert(node->is_point_buffer_loaded);
    assert(node->is_delete_buffer_loaded);
    assert(node->is_ranges_loaded);
    assert(node->is_info_file_loaded);
    assert(node->is_child_structure_loaded);
    assert(child->is_insert_buffer_loaded);
    assert(child->is_point_buffer_loaded);
    assert(child->is_delete_buffer_loaded);
    assert(child->is_ranges_loaded);
    assert(child->is_info_file_loaded);
    assert(child->is_child_structure_loaded);

    assert(node->insert_buffer.size() > buffer_size);
#endif
      
    DEBUG_MSG("Create U to send to child " << child->id);
    DEBUG_MSG(node->insert_buffer.size()/B_epsilon);

    std::set<point> U;
    range range_of_child(range(point(-1,-1), -1, -1));
    for (range r : node->ranges)
      if (r.node_id == child->id) {
        range_of_child = r;
        break;
      }
    size_t idx = 0;
    int max_y = range_of_child.max_y == INF ? -INF : range_of_child.max_y;
    point min = range_of_child.min;
    for (auto p : node->insert_buffer) {
      if (node->ranges.belong_to(range(p, -1, -1)) != range_of_child) continue;
      DEBUG_MSG(idx);

      if (++idx > std::max(node->insert_buffer.size()/B_epsilon,
                           node->insert_buffer.size()-buffer_size)) break;
      min = std::min(min,p);
      max_y = std::max(max_y, p.y);
      DEBUG_MSG("point " << p << " is in U");
      U.insert(p);
    }
  
    DEBUG_MSG("Remove points in U from Iv, Ic, Dc, Pc, Cv");

    std::set<point> new_U;
    for (point p : U) {
      if (node->insert_buffer.erase(p)) DEBUG_MSG("Removing " << p << " from insert buffer");

      if (child->insert_buffer.erase(p))
        DEBUG_MSG("Removing " << p << " from insert buffer of found child");

      if (child->delete_buffer.erase(p))
        DEBUG_MSG("Removing " << p << " from delete buffer of found child");

      if (child->point_buffer.erase(p)) { 
        DEBUG_MSG("Removing " << p << " from point buffer of found child");
        child->point_buffer.insert(p);
        continue;
      }
      DEBUG_MSG("Removing " << p << " from child structure from " << node->id);
      node->child_structure->remove(p);
      new_U.insert(p);
    }
    U = new_U;
    
    if (child->is_leaf() || child->is_virtual_leaf()) {
      
      if (*(U.begin()) < range_of_child.min || max_y > range_of_child.max_y || range_of_child.max_y == INF) {
        DEBUG_MSG("Updating range " << min << " " << max_y << " for node_id: " << range_of_child.node_id);
        node->ranges.erase(range_of_child);
        node->ranges.insert(range(min,max_y,range_of_child.node_id));
      }

#ifdef DEBUG
      DEBUG_MSG("Range now contains:");
      for (auto r : node->ranges)
        DEBUG_MSG(" - " << r);
#endif

      DEBUG_MSG("Inserting all points in our child structure... " << node->id);
      for (point p : U) {
        DEBUG_MSG(" - " << p);
        node->child_structure->insert(p);
      }

      DEBUG_MSG("found child was a leaf... sending U to Pc of node " << child->id);
      child->insert_into_point_buffer(U);

      if (state == STATE::normal || state == STATE::fix_up
          || state == STATE::global_rebuild) {
        event_stack.push({copy_node(child), EVENT::point_buffer_overflow});
        //event_stack.push({copy_node(child), EVENT::insert_buffer_overflow});
      }

    } else {

      /*
        1.) Any point in U with y-value >= the min y-value in Pc is inserted into Pc and Cv
        and removed from U.
        2.) If Pc overflows, i.e. |Pc| > B then repeatedly move min y-value point from Pc
        to U until |Pc| = B.
        3.) Add remaining points in U to Ic which might overflow.
      */

      point min_y = *std::min_element(child->point_buffer.begin(),
                                      child->point_buffer.end(),
                                      comp_y);
      DEBUG_MSG("Found min_y in found_child " << child->id << " to be " << min_y);

      DEBUG_MSG("Distributing points according to min_y element");
      std::set<point> new_U, temp_pb;
      for (point p : U) {
        if (min_y.y < p.y || (min_y.y == p.y && min_y.x < p.x)) {
          DEBUG_MSG("Point " << p << " went into found_childs point buffer");
          child->point_buffer.insert(p);
          DEBUG_MSG("Inserting " << p << " in child structure");
          node->child_structure->insert(p);
        } else {
          DEBUG_MSG("Point " << p << " stays in U");
          new_U.insert(p);
        }
      }
      U = new_U;

      std::vector<point> sorted_point_buffer(child->point_buffer.begin(),
                                             child->point_buffer.end());

      std::sort(sorted_point_buffer.begin(), sorted_point_buffer.end(), comp_y);
        
      if (child->point_buffer_overflow()) {
        DEBUG_MSG("Point buffer overflows in found_child " << child->id);
        DEBUG_MSG("Moving " << (child->point_buffer.size()-buffer_size)
                  << " points from point_buffer to U");
        size_t num_points_to_move = child->point_buffer.size() - buffer_size;
        for (size_t i = 0; i < num_points_to_move; i++) {
          DEBUG_MSG(" - " << sorted_point_buffer[i] << " went into U");
          child->point_buffer.erase(sorted_point_buffer[i]);
          DEBUG_MSG("Removing point " << sorted_point_buffer[i] << " from child structure");
          node->child_structure->remove(sorted_point_buffer[i]);
          U.insert(sorted_point_buffer[i]);
        }         
      }

      DEBUG_MSG("Updating range");
      range old_range;
      for (range r : node->ranges) {
        if (child->id == r.node_id) {
          old_range = r;
          break;
        }
      }

      range new_range = range(std::min(*child->point_buffer.begin(),old_range.min),
                              (sorted_point_buffer.end()-1)->y,
                              old_range.node_id);
        
      DEBUG_MSG("Updating range for found_child " << child->id << " from "
                << old_range << " to " << new_range);
        
      node->ranges.erase(old_range);
      node->ranges.insert(new_range);
        
      DEBUG_MSG("Inserting U into found_child " << child->id);
#ifdef DEBUG
      for (point p : U)
        DEBUG_MSG(" - " << p);
#endif
      
      child->insert_into_insert_buffer(U);

      if (state == STATE::normal || state == STATE::fix_up
          || state == STATE::global_rebuild)
        event_stack.push({copy_node(child), EVENT::insert_buffer_overflow});
    }
  }

  void buffered_pst::handle_delete_buffer_overflow_in_leaf_and_virtual_leaf(buffered_pst_node* node) {
    DEBUG_MSG("Delete buffer overflow in a leaf or virtual leaf: empty delete buffer in node " << node->id);
    // assert ( true == false );
#ifdef DEBUG
    assert (node->is_delete_buffer_loaded);
#endif
    node->delete_buffer.clear();
  }
  
  void buffered_pst::handle_delete_buffer_overflow(buffered_pst_node* node,
                                                   buffered_pst_node* child) {
    DEBUG_MSG("Starting to handle delete buffer overflow");
#ifdef DEBUG
    assert(node->is_insert_buffer_loaded);
    assert(node->is_point_buffer_loaded);
    assert(node->is_delete_buffer_loaded);
    assert(node->is_ranges_loaded);
    assert(node->is_info_file_loaded);
    assert(node->is_child_structure_loaded);
    
    assert(child->is_ranges_loaded);
    assert(child->is_point_buffer_loaded);
    assert(child->is_delete_buffer_loaded);
    assert(child->is_insert_buffer_loaded);

    assert(node->delete_buffer.size() > buffer_size/4);

#endif
      
    DEBUG_MSG("Create U to send to child");
    DEBUG_MSG(node->delete_buffer.size()/B_epsilon);

    range range_of_child(range(point(-1,-1),-1,-1));
    for (range r : node->ranges)
      if (child->id == r.node_id) {
        range_of_child = r;
        break;
      }

    size_t idx = 0;
    std::set<point> U;
    for (auto p : node->delete_buffer) {
      if (node->ranges.belong_to(range(p,-1,-1)) != range_of_child) continue;

      if (++idx > std::max(node->delete_buffer.size()/B_epsilon,
                           node->delete_buffer.size()-buffer_size/4)) break;
      DEBUG_MSG("point " << p << " is in U");
      U.insert(p);
    }

    point min_y = *std::min_element(child->point_buffer.begin(),
                                    child->point_buffer.end(),
                                    comp_y);
    DEBUG_MSG("Found min_y in found_child " << child->id << " to be " << min_y);
    
    DEBUG_MSG("Remove points in U from Dv, Ic, Dc, Pc, Cv");

    std::set<point> new_U;
    for (point p : U) {
      if (node->delete_buffer.erase(p)) {
        DEBUG_MSG("Removing " << p << " from delete buffer");
      }
      
      if (child->insert_buffer.erase(p)) {
        DEBUG_MSG("Removing " << p << " from insert buffer of found child");
#ifdef DEBUG
        CONTAINED_POINTS.erase(p);
#endif
      } else if (child->point_buffer.erase(p)) {
        DEBUG_MSG("Removing " << p << " from point buffer of found child");
#ifdef DEBUG
        CONTAINED_POINTS.erase(p);
#endif
        DEBUG_MSG("Removing " << p << " from child structure of found child");
        node->child_structure->remove(p);
      } else if (child->delete_buffer.erase(p)) {
        DEBUG_MSG("Removing " << p << " from delete buffer of found child");
        new_U.insert(p);
      } else new_U.insert(p);

    }
    U = new_U;
    
    int max_y = std::max_element(child->point_buffer.begin(),
                                 child->point_buffer.end(),
                                 comp_y)->y;

    if (child->point_buffer.empty()) max_y = INF;
      
    DEBUG_MSG("Rebuilding range max_y from " << range_of_child.max_y << " to "
              << max_y << " in delete_buffer_overflow() in node " << node->id);
    node->ranges.erase(range_of_child);
    node->ranges.insert(range(range_of_child.min,max_y,range_of_child.node_id));

#ifdef DEBUG
    DEBUG_MSG("Range now contains:");
    for (auto r : node->ranges)
      DEBUG_MSG(" - " << r);
#endif
      
    if ( !(child->is_leaf() || child->is_virtual_leaf()) ) {

      DEBUG_MSG("Removing points that cannot delete anything further down");
      std::set<point> new_U;
      for (point p : U) {
        DEBUG_MSG("Looking at point " << p);
        if (p.y < min_y.y || (min_y.y == p.y && p.x < min_y.x)) {
          DEBUG_MSG("Point " << p << " stays in U");
          new_U.insert(p);
        }
      }
      U = new_U;
      
      child->insert_into_delete_buffer(U);

      if (state == STATE::normal || state == STATE::fix_up
          || state == STATE::global_rebuild) {
        event_stack.push({copy_node(child), EVENT::point_buffer_underflow});
        event_stack.push({copy_node(child), EVENT::delete_buffer_overflow});
      }
    }
  }

  /*
    Invariant: point buffer is empty & we are a virtual leaf
   */
  void buffered_pst::handle_point_buffer_underflow_in_virtual_leaf(buffered_pst_node* node,
                                                                   buffered_pst_node* parent) {
    DEBUG_MSG("Starting to handle underflow in the point buffer of a virtual leaf");

#ifdef DEBUG
    assert(node->is_delete_buffer_loaded);
    assert(node->is_insert_buffer_loaded);
    assert(parent->is_child_structure_loaded);
#endif

    node->delete_buffer.clear();
    std::vector<point> temp(node->insert_buffer.begin(), node->insert_buffer.end());
    node->insert_buffer.clear();
    DEBUG_MSG("Moving points from insert buffer to point buffer in virtual leaf " << node->id);
    for (point p : temp) {
      DEBUG_MSG(" - " << p);
      parent->child_structure->insert(p);
    }
    node->insert_into_point_buffer(temp);
    if (state == STATE::normal || state == STATE::fix_up
        || state == STATE::global_rebuild)
      event_stack.push({copy_node(node),EVENT::insert_buffer_overflow});
  }

  void buffered_pst::handle_point_buffer_underflow_in_children(buffered_pst_node* node) {
    DEBUG_MSG("Pushing all children to the stack to handle underflowing point buffer");
#ifdef DEBUG
    assert(node->is_ranges_loaded);
#endif
    for (range r : node->ranges) {
      //if (it->max_y == INF) continue;
      // if (state == STATE::normal || state == STATE::fix_up
      //     || state == STATE::global_rebuild)
      event_stack.push({
          new buffered_pst_node(r.node_id, buffer_size, epsilon, root),
            EVENT::point_buffer_underflow});
    }
  }

  
  void buffered_pst::handle_point_buffer_underflow(buffered_pst_node* parent,
                                                   buffered_pst_node* node,
                                                   const std::vector<buffered_pst_node*>
                                                   &children) {
    DEBUG_MSG("Starting to handle underflowing point buffer in node " << node->id);
#ifdef DEBUG
    assert(node->is_point_buffer_loaded);
    assert(node->is_delete_buffer_loaded);
    assert(node->is_insert_buffer_loaded);
    assert(node->is_ranges_loaded);
    assert(node->is_child_structure_loaded);
    for (auto c : children) {
      assert(c->is_point_buffer_loaded);
      assert(c->is_ranges_loaded);
    }
    assert(parent->is_child_structure_loaded);
    assert(parent->is_ranges_loaded);
#endif

    typedef std::pair<point, int> point_child_pair;
    auto comp = [](const point_child_pair &p1, const point_child_pair &p2) {
      return p1.first.y < p2.first.y || (p1.first.y == p2.first.y && p1.first.x < p2.first.x);
    };
    std::priority_queue<point_child_pair, std::vector<point_child_pair>,
                        decltype(comp)> pq(comp);
 
    std::map<int,int> node_id_to_index;
    for (int i = 0; i < (int)children.size(); i++) {
      node_id_to_index[children[i]->id] = i;
      for (auto p : children[i]->point_buffer) {
        DEBUG_MSG("Added point: " << p << " to pq with c.id = " << children[i]->id);
        pq.push(point_child_pair(p,i));
      }
    }

    // TODO: should X.size() <= buffer_size/2 ??
    DEBUG_MSG("Grabbing the B/2 highest y-value points from children and deleting them");
    std::set<point> X;
    while (!pq.empty() && X.size() < buffer_size/2) {
      point_child_pair pcp = pq.top(); pq.pop();
      if ( node->delete_buffer.erase(pcp.first) ) {
        DEBUG_MSG("Point " << pcp.first << " was canceled by delete in node " << node->id);
#ifdef DEBUG
        CONTAINED_POINTS.erase(pcp.first);
#endif
      } else {
        auto it = node->insert_buffer.find(pcp.first);
        if ( it != node->insert_buffer.end() ) {
          DEBUG_MSG("Move more recent updates of p from Iv to X in node " << node->id);
          node->insert_buffer.erase(*it);
          X.insert(*it);
        } else {
          DEBUG_MSG("Inserting point " << pcp.first << " into X in node id " << node->id);
          X.insert(pcp.first);
        }
      }
      DEBUG_MSG("Removing " << pcp.first << " from " << children[pcp.second]->id);
      children[pcp.second]->point_buffer.erase(pcp.first);
      DEBUG_MSG("Removing " << pcp.first << " from child structure " << node->id);
      node->child_structure->remove(pcp.first);
    }

    DEBUG_MSG("Satisfy heap ordering between Iv and Pv");
    std::vector<point> vp_temp(X.begin(), X.end());
    vp_temp.insert(vp_temp.end(), node->insert_buffer.begin(), node->insert_buffer.end());
    std::sort(vp_temp.begin(), vp_temp.end(),
              [](const point &p1, const point &p2) {
                return p1.y > p2.y || (p1.y == p2.y && p1.x > p2.x);
              });
    int X_size = X.size();
    X = std::set<point>(vp_temp.begin(), vp_temp.begin()+X_size);
    node->insert_buffer = std::set<point>(vp_temp.begin()+X_size, vp_temp.end());

#ifdef DEBUG
    DEBUG_MSG("X now contains:");
    for (auto p : X) DEBUG_MSG(" - " << p);
    DEBUG_MSG("Insert buffer now contains");
    for (auto p : node->insert_buffer) DEBUG_MSG(" - " << p);
    DEBUG_MSG("Point buffer now contains");
    for (auto p : node->point_buffer) DEBUG_MSG(" - " << p);
#endif

    point min_y = *std::min_element(node->point_buffer.begin(),
                                    node->point_buffer.end(),
                                    comp_y);
    
    if (node->point_buffer.empty()) min_y = point(INF,INF);
    
    if (!X.empty())
      min_y = std::min(min_y, *std::min_element(X.begin(), X.end(), comp_y), comp_y);

    DEBUG_MSG("Removing deletes that can not cancel points further down from node "
              << node->id);
    DEBUG_MSG("Found min_y to be " << min_y);
    std::set<point> new_delete_buffer;
    for (point p : node->delete_buffer) {
      if (p.y < min_y.y || (p.y == min_y.y && p.x < min_y.x)) new_delete_buffer.insert(p);
      else DEBUG_MSG_FAIL(" - removed point " << p);
    }
   
    node->delete_buffer = new_delete_buffer;

    DEBUG_MSG("Rebuilding our ranges");
   
    internal::rb_tree<range> new_ranges;
    for (range r : node->ranges) {
      point max_y = *std::max_element(children[node_id_to_index[r.node_id]]->
                                      point_buffer.begin(),
                                      children[node_id_to_index[r.node_id]]->
                                      point_buffer.end(),
                                      comp_y);
      if (children[node_id_to_index[r.node_id]]->point_buffer.empty())
        new_ranges.insert(range(r.min, INF, r.node_id));
      else new_ranges.insert(range(r.min, max_y.y, r.node_id));
    }
    node->ranges = new_ranges;
    DEBUG_MSG("Our ranges now look LIKE this:");
#ifdef DEBUG
    for (range r : node->ranges) DEBUG_MSG(r);
#endif

    DEBUG_MSG("Finally point in X are inserted into Pv");
    node->insert_into_point_buffer(X);
    
    //if we are not the root then insert the pulled up points in our parents child structure
    if (!node->is_root()) {
      DEBUG_MSG("Insert X into our parents child structure " << parent->id);
      for (point p : X) {
        DEBUG_MSG(" - " << p);
        parent->child_structure->insert(p);
      }
    }
    
    int max_y = std::max_element(node->point_buffer.begin(),
                                 node->point_buffer.end(),
                                 comp_y)->y;

    if (node->point_buffer.empty()) max_y = INF;

    DEBUG_MSG("Updating node " << node->id << " parent ranges on parent id " << parent->id);
    for (range r : parent->ranges) {
      if (r.node_id == node->id) {
        parent->ranges.erase(r);
        parent->ranges.insert(range(r.min,max_y,r.node_id));
        break;
      }
    }

#ifdef DEBUG
    DEBUG_MSG("Parents ranges now look like this:");
    for (range r : parent->ranges)
      DEBUG_MSG(" - " << r);
#endif
    
    if ( state == STATE::normal || state == STATE::fix_up
         || state == STATE::global_rebuild) {
      event_stack.push({copy_node(node), EVENT::insert_buffer_overflow});
      event_stack.push({copy_node(node), EVENT::delete_buffer_overflow});
    }
    
  }

  
  void buffered_pst::handle_node_degree_overflow(buffered_pst_node* node,
                                                 buffered_pst_node* parent,
                                                 std::map<int,buffered_pst_node*>
                                                 &children) {
    
    DEBUG_MSG("Starting to handle node degree overflow of node " << node->id);
    
#ifdef DEBUG
    assert(node->is_ranges_loaded);
    assert(node->is_info_file_loaded);
    assert(node->is_point_buffer_loaded);
    assert(node->is_delete_buffer_loaded);
    assert(node->is_insert_buffer_loaded);
    assert(node->is_child_structure_loaded);
    assert(parent->is_ranges_loaded);
    for (const auto& c : children) {
      assert(c.second->is_point_buffer_loaded);
      assert(c.second->is_info_file_loaded);
    }
#endif
      
    DEBUG_MSG("Handle node degree overflow");

    /*
      - Remember buffers on old node
      - Clear buffers on old node
      - Put old node in new_children vector
    */

    std::set<point> temp_delete_buffer(node->delete_buffer.begin(), node->delete_buffer.end());
    std::set<point> temp_point_buffer(node->point_buffer.begin(), node->point_buffer.end());
    std::set<point> temp_insert_buffer(node->insert_buffer.begin(), node->insert_buffer.end());
    internal::rb_tree<range> temp_ranges(node->ranges.begin(), node->ranges.end());
    
    node->delete_buffer.clear();
    node->insert_buffer.clear();
    node->point_buffer.clear();
    node->ranges.clear();

    int nodes_to_create = (int)ceil((double)temp_ranges.size() / (double)B_epsilon);
    DEBUG_MSG("NODES TO CREATE: " << temp_ranges.size() << " / " << B_epsilon << " = " << nodes_to_create);
    buffered_pst_node* new_children[nodes_to_create];
    std::map<int,int> node_id_to_idx;
    
    if (node->is_root()) {
      node_id_to_idx[next_id] = 0;
      new_children[0] =
        new buffered_pst_node(next_id++, node->parent_id, buffer_size, B_epsilon,
                              epsilon, root);
      node->child_structure->destroy();
      delete node->child_structure;
    } else {
      new_children[0] = node;
      node_id_to_idx[node->id] = 0;
    }

    for (int i = 1; i < nodes_to_create; i++) {
      node_id_to_idx[next_id] = i;
      new_children[i] =
        new buffered_pst_node(next_id++, node->parent_id, buffer_size, B_epsilon, epsilon, root);
    }

    DEBUG_MSG("Distributing children");
#ifdef DEBUG
    for (auto r : temp_ranges)
      DEBUG_MSG(" - " << r);
#endif
                               
    size_t idx = 0;
    int each_get = ceil((double)temp_ranges.size()/nodes_to_create);
    std::vector<std::vector<point> > children_points(nodes_to_create, std::vector<point>());
    for (range r : temp_ranges) {
      new_children[idx/each_get]->add_child(r);
      children[r.node_id]->parent_id = new_children[idx/each_get]->id;
      for (point p : children[r.node_id]->point_buffer) {
        children_points[idx/each_get].push_back(p);
      }
      idx++;
    }

    DEBUG_MSG("Constructing child structures Cv'");
    idx = 0;
    for (auto bpn : new_children) {
      bpn->child_structure->destroy();
      delete bpn->child_structure;
      std::sort(children_points[idx].begin(), children_points[idx].end());
      bpn->child_structure =
        new ext::child_structure(bpn->id, buffer_size, epsilon, children_points[idx]);
      idx++;
    }

    DEBUG_MSG("Deleting our range in parent " << parent->id << " we are: " << node->id);
    range our_range(point(INF,INF), -INF, node->id);
    if (!node->is_root()) {
      for (auto r : parent->ranges) {
        if (r.node_id == node->id) {
          parent->ranges.erase(r);
          our_range = r;
          DEBUG_MSG("Found range " << r << ". Deleted from parents range.");
          break;
        }
      }
    }

    DEBUG_MSG("Inserting new ranges");
    bool is_first_child = true;
    for (auto bpn : new_children) {
      range r = *(bpn->ranges.begin());
      if (is_first_child) {
        r.min = std::min(r.min,our_range.min);
        is_first_child = false;
      }
      parent->add_child(range(r.min, r.max_y, bpn->id));
    }

    DEBUG_MSG("Distributing points from point_buffer");

    for (point p : temp_point_buffer) {
      range r = parent->ranges.belong_to(range(p,-1,-1));
      new_children[node_id_to_idx[r.node_id]]->point_buffer.insert(p);
      DEBUG_MSG("point " << p << " went into " << r.node_id << " node_id_to_idx: " << node_id_to_idx[r.node_id]);
    }

    if (node->is_root()) {
      DEBUG_MSG("Inserting points from temp_point_buffer into roots child structure");
      std::vector<point> root_child_structure(temp_point_buffer.begin(),
                                              temp_point_buffer.end());
      node->child_structure = new ext::child_structure(node->id, buffer_size, epsilon,
                                                       root_child_structure);
    }

    DEBUG_MSG("Distributing points from insert_buffer");
    for (point p : temp_insert_buffer) {
      range r = parent->ranges.belong_to(range(p,-1,-1));
      new_children[node_id_to_idx[r.node_id]]->insert_buffer.insert(p);
      DEBUG_MSG("point " << p << " went into " << r.node_id);
    }

    DEBUG_MSG("Distributing points from delete_buffer");
    for (point p : temp_delete_buffer) {
      range r = parent->ranges.belong_to(range(p,-1,-1));
      new_children[node_id_to_idx[r.node_id]]->delete_buffer.insert(p);
      DEBUG_MSG("point " << p << " went into " << r.node_id);
    }

    DEBUG_MSG("Rebuild ranges to fit max_y of child");
    for (auto bpn : new_children) {
      int max_y = std::max_element(bpn->point_buffer.begin(),
                                   bpn->point_buffer.end(),
                                   comp_y)->y;
      if (bpn->point_buffer.empty()) max_y = INF;
      range ra(point(-1,-1),-1,-1);
      for (range r : parent->ranges)
        if (r.node_id == bpn->id) {
          ra = r;
          break;
        }
      parent->ranges.erase(ra);
      parent->ranges.insert(range(ra.min, max_y, ra.node_id));
    }
#ifdef DEBUG
    DEBUG_MSG("PARENTS RANGES AFTER:");
    for (auto r : parent->ranges)
      DEBUG_MSG(" - " << r);
#endif

    if (state == STATE::normal || state == STATE::global_rebuild ||
        (state == STATE::fix_up && (parent->id != parent_to_stop_at || node->is_root())))
      event_stack.push({copy_node(parent), EVENT::node_degree_overflow});

    if ( node->is_root() && (state == STATE::normal || state == STATE::fix_up
                             || state == STATE::global_rebuild))
      event_stack.push({node, EVENT::point_buffer_underflow});
    
    for (auto bpn : new_children) {
      if (state == STATE::normal || state == STATE::fix_up || state == STATE::global_rebuild)
        event_stack.push({copy_node(bpn), EVENT::point_buffer_underflow});
      bpn->flush_all();
    }
    
    for (int i = node->is_root() ? 0 : 1; i < nodes_to_create; i++) {
      delete new_children[i];
    }
  
  }

  buffered_pst::buffered_pst_node* buffered_pst::find_child(buffered_pst_node* node,
                                              const std::set<point> &buffer) {
#ifdef DEBUG
    assert(node->is_ranges_loaded);
#endif
    DEBUG_MSG("Starting to find child of " << node->id);

#ifdef DEBUG
    DEBUG_MSG("Ranges to consider:");
    for (auto r : node->ranges)
      DEBUG_MSG(" - " << r);
#endif
    
    std::set<point> tmp,best;
    range best_range = node->ranges.belong_to(range(*buffer.begin(),-1,-1));
    range cur_range = node->ranges.belong_to(range(*buffer.begin(),-1,-1));
    for (point p : buffer) {
      range blt = node->ranges.belong_to(range(p,-1,-1));
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

    return new buffered_pst_node(best_range.node_id, buffer_size, epsilon, root);
  }

  /*
    Splits a leaf such that the point buffers of non of the leaves
    are overflowed
   */
  void buffered_pst::split_leaf(buffered_pst_node* node, buffered_pst_node* parent) {
    DEBUG_MSG("Started splitting of leaf " << node->id);
#ifdef DEBUG
    assert(node->is_point_buffer_loaded);
    assert(node->is_info_file_loaded);
    assert(parent->is_ranges_loaded);
#endif
    
#ifdef DEBUG
    DEBUG_MSG_FAIL("We are distributing point_buffer");
    for (point p : node->point_buffer)
      DEBUG_MSG_FAIL(" - " << p);
#endif
   
    int nodes_to_create = (int)ceil((double)node->point_buffer.size()/(double)(buffer_size/2-1));

    DEBUG_MSG_FAIL("We are creating (" << node->point_buffer.size() << " / " << buffer_size/2-1 <<
                   ") - 1 = " << nodes_to_create << "-1 new children");

    buffered_pst_node* new_leaves[nodes_to_create];
    new_leaves[0] = node;

    parent->ranges.erase(parent->ranges.belong_to(range(*(node->point_buffer).begin(),-1,-1)));
    
    std::vector<range> new_ranges;
    new_ranges.push_back(range(point(INF,INF), -INF, node->id));
    
    for (int i = 1; i < nodes_to_create; i++) {
      new_leaves[i] = new buffered_pst_node(next_id, node->parent_id, buffer_size,
                                            B_epsilon, epsilon, root);
      new_ranges.push_back(range(point(INF,INF), -INF, next_id));
      next_id++;
    }
    
    std::set<point> point_buffer_to_split(node->point_buffer.begin(),
                                          node->point_buffer.end());
    node->point_buffer.clear();
    
    int each_get = (int)ceil((double)point_buffer_to_split.size()/(double)nodes_to_create);
    DEBUG_MSG_FAIL("Each node should get " << each_get << " points");
    
    int idx = 0;
    DEBUG_MSG_FAIL("Starting to distribute points between children initiated by node "
                   << node->id);
    for (point p : point_buffer_to_split) {
      DEBUG_MSG_FAIL(" - " << p << " went into child "
                     << std::to_string(new_leaves[idx/each_get]->id) << " in index " << idx);
      new_leaves[idx/each_get]->point_buffer.insert(p);
      new_ranges[idx/each_get].min = std::min(p,new_ranges[idx/each_get].min);
      new_ranges[idx/each_get].max_y = std::max(p.y,new_ranges[idx/each_get].max_y);
      idx++;
    }

    for (range r : new_ranges) {
      parent->add_child(r);
    }

#ifdef DEBUG
    DEBUG_MSG("Ranges in parent now contains:");
    for (auto r : parent->ranges)
      DEBUG_MSG(" - " << r);
#endif

    for (int i = 1; i < nodes_to_create; i++) {
      new_leaves[i]->flush_all();
    }

    if (state == STATE::normal || state == STATE::global_rebuild)
      event_stack.push({copy_node(parent), EVENT::node_degree_overflow});

    if (state == STATE::fix_up && parent->id != parent_to_stop_at)
      event_stack.push({copy_node(parent), EVENT::node_degree_overflow});

  }

  void buffered_pst::flush_buffers_to_child(buffered_pst_node* node,
                                            std::deque<buffered_pst_node*> &q,
                                            std::deque<buffered_pst_node*> &fix_up_queue,
                                            int x1, int x2, int y) {
    auto left_it = node->ranges.belong_to_iterator(range(point(x1,-INF),-1,-1));
    auto right_it = node->ranges.belong_to_iterator(range(point(x2,INF),-1,-1));
    internal::rb_tree<range> new_ranges;
    for (range r : node->ranges) new_ranges.insert(r);
    for (auto it=left_it; !(node->is_leaf() && node->is_virtual_leaf()); it++) {
      DEBUG_MSG_FAIL("Opening child " << it->node_id);
      buffered_pst_node* child =
        new buffered_pst_node(it->node_id, buffer_size, epsilon, root);

      child->load_all();

      // TODO: min_y value should be on ranges.
      point min_y = *std::min_element(child->point_buffer.begin(),
                                      child->point_buffer.end(),
                                      comp_y);
      if (child->point_buffer.empty()) min_y = point(-INF,-INF);
      DEBUG_MSG("Found min_y of child " << child->id << " to be " << min_y);
      DEBUG_MSG("Looking at delete_buffer of " << node->id);
      std::set<point> new_delete_buffer;
      for (point p : node->delete_buffer) {
        DEBUG_MSG_FAIL("Looking at delete " << p);
        if (node->ranges.belong_to(range(p,-1,-1)) == *it) {
          if (child->point_buffer.erase(p)) {
            DEBUG_MSG_FAIL("Removing " << p <<
                           " from childs point buffer and child structure" <<
                           " of node " << node->id);
            node->child_structure->remove(p);
#ifdef DEBUG
            CONTAINED_POINTS.erase(p);
#endif
          } else if (child->insert_buffer.erase(p)) {
#ifdef DEBUG
            CONTAINED_POINTS.erase(p);
#endif
          } else if ( (p.y < min_y.y || (p.y == min_y.y && p.x < min_y.x)) && !child->is_leaf() ) {
            DEBUG_MSG_FAIL("Inserting delete " << p << " into child " << child->id
                           << " delete_buffer");
            child->delete_buffer.insert(p);
          }
          
        } else {
          new_delete_buffer.insert(p);
        }
      }
      node->delete_buffer = new_delete_buffer;
      DEBUG_MSG("Looking at insert_buffer of " << node->id);
      std::set<point> new_insert_buffer;
      for (point p : node->insert_buffer) {
        if (node->ranges.belong_to(range(p,-1,-1)) == *it) {
          if (child->point_buffer.erase(p))
            node->child_structure->remove(p);       
          child->insert_buffer.erase(p);
          child->delete_buffer.erase(p);
          if (p.y < min_y.y || (p.y == min_y.y && p.x < min_y.x)) {
            child->insert_buffer.insert(p);
          } else {
            child->point_buffer.insert(p);
            node->child_structure->insert(p);
          }
        } else {
          new_insert_buffer.insert(p);
        }
      }
      node->insert_buffer = new_insert_buffer;

      DEBUG_MSG("Check if point_buffer overflows at child " << child->id);
      if (child->point_buffer_overflow()) {
        
        std::vector<point> sorted_points(child->point_buffer.begin(),
                                         child->point_buffer.end());
        std::sort(sorted_points.begin(),sorted_points.end(),comp_y);

        size_t points_to_remove = child->is_leaf()
          ? sorted_points.size() - (buffer_size / 2 - 1)
          : sorted_points.size() - buffer_size;
        
        DEBUG_MSG("we remove " << points_to_remove << " points of a total of " << sorted_points.size());
        
        for (size_t i = 0; i < points_to_remove; i++) {
          node->child_structure->remove(sorted_points[i]);
          child->insert_buffer.insert(sorted_points[i]);
        }

        child->point_buffer = std::set<point>
          (sorted_points.begin()+points_to_remove, sorted_points.end());
      }

      // At this point we can rebuild the range of the child!
      // TODO: rebuild min_y
      int max_y = std::max_element(child->point_buffer.begin(),
                                   child->point_buffer.end(),
                                   comp_y)->y;
      if (child->point_buffer.empty()) max_y = INF;

      if (max_y != it->max_y) {
        DEBUG_MSG("Updated the range: " << *it << " to " << range(it->min, max_y, it->node_id));
        new_ranges.erase(*it);
        new_ranges.insert(range(it->min, max_y, it->node_id));
      }

      child->flush_all();
      
      q.push_back(child);
      fix_up_queue.push_back(child);
      
      if (it == right_it) break;
    }
    node->ranges = new_ranges;
  }

  void buffered_pst::report(int x1, int x2, int y, const std::string &output_file) {

    DEBUG_MSG_FAIL("Reporting points in [" << x1 << ", " << x2 << "] X [" <<
                   y << ", \u221E]");
    
    if (util::file_exists(output_file))
      error(1, ECANCELED, "Output file exists. Aborting.");

    io::buffered_stream<point> result(buffer_size);
    result.open(output_file);

    if (x2 < x1) {
      DEBUG_MSG("x2 < x1 for x1: " << x1 << " x2: " <<x2);
      result.close();
      return;
    }

    std::deque<buffered_pst_node*> q, fix_up_queue;
    fix_up_queue.push_front(root);
    
    flush_buffers_to_child(root, q, fix_up_queue, x1, x2, y);

    // Report
    for (point p : root->point_buffer) {
      if (util::in_range(p,x1,x2,y)) {
        DEBUG_MSG_FAIL("- reporting point p " << p << " from Pr");
        result.write(p);
      }
    }

    for (point p : root->insert_buffer) {
      if (util::in_range(p,x1,x2,y)) {
        DEBUG_MSG_FAIL("- reporting point p " << p << " from Ir");
        result.write(p);
      }
    }

    for (point p : root->child_structure->report(x1,x2,y)) {
      // TODO: Should we check if point lies in Dr??
      DEBUG_MSG_FAIL("- reporting point p " << p << " from Cr");
      result.write(p);
    }

    /* *****************************************************************************
       RECURSE ON CHILDREN !!!!
    ****************************************************************************/ 
    while (!q.empty()) {

      buffered_pst_node* node = q.front(); q.pop_front();
      node->load_all();
      flush_buffers_to_child(node, q, fix_up_queue, x1, x2, y);

      DEBUG_MSG("Starting report from insert_buffer of node " << node->id);
      for (point p : node->insert_buffer) {
        if (util::in_range(p,x1,x2,y)) {
          DEBUG_MSG_FAIL("Reporting point " << p << " from IB in node " << node->id);
          result.write(p);
        }
      }

      DEBUG_MSG("Starting report from child_structure of node " << node->id);
      std::vector<point> cs_result = node->child_structure->report(x1,x2,y);
      DEBUG_MSG_FAIL("Result from CS of node " << node->id << " has size "
                     << cs_result.size());
      for (point p : cs_result) {
        // TODO: Should we check if point lies in Dr??
        DEBUG_MSG_FAIL("Reporting point " << p << " from CS in node " << node->id);
        result.write(p);
      }
      DEBUG_MSG_FAIL("Flushing node " << node->id);
      node->flush_all();
    }
    result.close();

    handle_invalid_buffer_events(fix_up_queue);
  }

  void buffered_pst::handle_invalid_buffer_events(std::deque<buffered_pst_node*> &q) {
    DEBUG_MSG("Starting to handle invalid buffers");

    state = STATE::fix_up;

    while (!q.empty()) {
      buffered_pst_node* node = q.back(); q.pop_back();
      DEBUG_MSG_FAIL("We are about to fix node " << node->id);
      node->load_info_file();
      parent_to_stop_at = node->parent_id;
      node->flush_info_file();
      event_stack.push({copy_node(node), EVENT::point_buffer_underflow});
      event_stack.push({copy_node(node), EVENT::node_degree_overflow});
      event_stack.push({copy_node(node), EVENT::insert_buffer_overflow});
      event_stack.push({copy_node(node), EVENT::delete_buffer_overflow});
      if (!node->is_root()) delete node;
      handle_events();
    }

    state = STATE::normal;
    
  }

  void buffered_pst::global_rebuild() {

    DEBUG_MSG_FAIL("Starting GLOBAL REBUILD");

    state = STATE::global_rebuild;
    
    buffered_pst_node* old_root = root;
    root = new buffered_pst_node(0, 0, buffer_size, B_epsilon, epsilon, 0);

    int epoch_end = next_id;
    epoch_begin_point_count = 0;
    
    std::deque<buffered_pst_node*> q, q_temp;
    q.push_front(old_root);

    while (!q.empty()) {
      buffered_pst_node* node = q.front(); q.pop_front();
      node->load_all();
      flush_buffers_to_child(node, q, q_temp, -INF, INF, -INF);
      for (point p : node->point_buffer) {
        insert(p);
        epoch_begin_point_count++;
      }
      for (point p : node->insert_buffer) {
        insert(p);
        epoch_begin_point_count++;
      }
      node->point_buffer.clear();
      node->delete_buffer.clear();
      node->insert_buffer.clear();
      node->ranges.clear();
      node->child_structure->destroy();
      node->flush_all();
      delete node;
    }

    for (int i=epoch_begin; i < epoch_end; i++) {
      DEBUG_MSG("Destructing file " << i);
      util::remove_directory(std::to_string(i));
      util::remove_directory("c_"+std::to_string(i));
    }

    epoch_begin = epoch_end;
    operation_count = 0;

    state = STATE::normal;
    
  }

  void buffered_pst::construct_sorted(const std::string &file_name) {
    DEBUG_MSG("Starting construction on sorted file: " << file_name);

    state = STATE::construct;
    
    io::buffered_stream<point> points(buffer_size);
    points.open(file_name);
    DEBUG_MSG("File contains " << points.size() << " points.");

    int each_leaf_get = buffer_size/2-1;
    DEBUG_MSG_FAIL("Each leaf get " << buffer_size << " / 2 - 1 = " << each_leaf_get);

    std::vector<buffered_pst_node*> layer_i_plus_1, layer_i;
    std::vector<point> min_points;
    std::vector<int> max_ys;
    std::vector<buffered_pst_node*> nodes;
    buffered_pst_node* child, *parent;
    point min_point = point(INF, INF);
    int max_y = -INF;
    for (size_t i = 0; !points.eof(); i++) {
      DEBUG_MSG(i);
      if (i % each_leaf_get == 0) {

        if (i != 0) {
          max_ys.push_back(max_y);
          min_points.push_back(min_point);
          child->flush_all();
        }

        DEBUG_MSG("Make a new child");
        child = new buffered_pst_node(next_id++, -1, buffer_size,
                                      epsilon, B_epsilon, root);
        
        layer_i.push_back(child);
        min_point = point(INF, INF);
        max_y = -INF;
      }
      point p = points.read();
      child->point_buffer.insert(p);
      min_point = std::min(min_point, p);
      max_y = std::max(max_y, p.y);
    }
    max_ys.push_back(max_y);
    min_points.push_back(min_point);
    child->flush_all();
    
    bool leaf_layer = true;
    while (layer_i.size() > (size_t)B_epsilon) {
      DEBUG_MSG("LAYER I NOW HAS: " << layer_i.size() << " nodes");
      std::swap(layer_i, layer_i_plus_1);
      for (auto bpn : layer_i) { bpn->flush_all(); delete bpn; }
      layer_i.clear();
      DEBUG_MSG("LAYER I+1 NOW HAS: " << layer_i_plus_1.size() << " nodes");
      for (size_t i = 0; i < layer_i_plus_1.size(); i++) {
        if (i % B_epsilon == 0) {
          DEBUG_MSG_FAIL("Added new parent");
          if (i != 0) parent->flush_all();
          parent = new buffered_pst_node(next_id++, -1, buffer_size, epsilon, B_epsilon, root);
          layer_i.push_back(parent);
          nodes.push_back(copy_node(parent));
        }
        layer_i_plus_1[i]->load_info_file();
        layer_i_plus_1[i]->load_ranges();
        layer_i_plus_1[i]->parent_id = parent->id;
        if (leaf_layer) {
          parent->add_child(range(min_points[i], max_ys[i], layer_i_plus_1[i]->id));
        } else {
          parent->add_child(range(layer_i_plus_1[i]->ranges.begin()->min, -INF,
                                  layer_i_plus_1[i]->id));
        }
        layer_i_plus_1[i]->flush_info_file();
        layer_i_plus_1[i]->flush_ranges();
      }
      leaf_layer = false;
      parent->flush_all();
    }
    
    DEBUG_MSG("Add " << layer_i.size() << " nodes to root");
    for (size_t i = 0; i < layer_i.size(); i++) {
      layer_i[i]->load_ranges();
      layer_i[i]->load_info_file();
      if (leaf_layer)
        root->add_child(range(min_points[i], max_ys[i], layer_i[i]->id));
      else
        root->add_child(range(layer_i[i]->ranges.begin()->min, -INF, layer_i[i]->id));
      layer_i[i]->parent_id = 0;
      layer_i[i]->flush_all();
      delete layer_i[i];
    }

    nodes.push_back(copy_node(root));

    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
      event_stack.push({copy_node(*it), EVENT::point_buffer_underflow_construct});
    }
    
    for (auto it = nodes.rbegin(); it != nodes.rend(); ++it) {
      event_stack.push({copy_node(*it), EVENT::point_buffer_underflow_construct});
    }

    // print();
    
    // int k;
    // std::cin >> k;
    
    handle_events();

    for (auto node : nodes)
      if (!node->is_root()) delete node;

    epoch_begin_point_count = points.size();

    points.close();
    
    state = STATE::normal;
    // print();
    
  }
  
  
  void buffered_pst::print() {
    assert(root);
    std::ofstream dot_file;
    dot_file.open("temp.dot");
    dot_file << "digraph {\n";
    dot_file << "0 [shape=rectangle label=\"0\nParent: "<<root->parent_id <<"\npb: ";
    for (point p : root->point_buffer) dot_file << p << ", ";
    dot_file << "\nib: ";
    for (point p : root->insert_buffer) dot_file << p << ", ";
    dot_file << "\ndb: ";
    for (point p : root->delete_buffer) dot_file << p << ", ";
    dot_file << "\nCS: ";
    for (point p : root->child_structure->get_points()) dot_file << p << ", ";
    dot_file << "\"]\n";
    std::queue<buffered_pst_node> q;
    for (auto r : root->ranges) {
      q.push(buffered_pst_node(r.node_id, buffer_size, epsilon, root));
      dot_file << "0 -> " << std::to_string(r.node_id) << "[label=\"" << r << "\"]\n";
    }
    while (!q.empty()) {
      buffered_pst_node bpn = q.front(); q.pop();
      bpn.load_all();
      dot_file << std::to_string(bpn.id) << " [shape=folder label=\" " << std::to_string(bpn.id) << "\nParent: " << bpn.parent_id << "\npb: ";
      for (point p : bpn.point_buffer) dot_file << p << ", ";
      dot_file << "\nib: ";
      for (point p : bpn.insert_buffer) dot_file << p << ", ";
      dot_file << "\ndb: ";
      for (point p : bpn.delete_buffer) dot_file << p << ", ";
      dot_file << "\nCS: ";
      for (point p : bpn.child_structure->get_points()) dot_file << p << ", ";
      dot_file << "\"]\n";
      for (auto r : bpn.ranges) {
        q.push(buffered_pst_node(r.node_id, buffer_size, epsilon, root));
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
    delete root;
    for (int i=0; i < next_id; i++) {
      DEBUG_MSG("Destructing file " << i);
      util::remove_directory(std::to_string(i));
      util::remove_directory("c_"+std::to_string(i));
    }
  }

#ifdef DEBUG
  bool buffered_pst::is_valid() {
    return root->is_valid();
  }
#endif
  
};

#endif
