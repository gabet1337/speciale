#ifndef EXTERNAL_PRIORITY_SEARCH_TREE
#define EXTERNAL_PRIORITY_SEARCH_TREE

#include "../common/point.hpp"
#include "../common/debug.hpp"
#include "../common/validate.hpp"
#include "../common/utilities.hpp"
#include "../common/definitions.hpp"
//#include "../internal/rb_tree.hpp"
//#include "range.hpp"
#include "child_structure_interface.hpp"
#include "child_structure.hpp"
//#include "child_structure_stub.hpp"
#include "pst_interface.hpp"
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
#define INF_POINT point(INF,INF)
#define MINUS_INF_POINT point(-INF,-INF)
namespace ext {

  class external_priority_search_tree : public pst_interface {

  public:
    external_priority_search_tree(size_t buffer_size);
    external_priority_search_tree(size_t buffer_size, double epsilon);
    ~external_priority_search_tree();
    void insert(const point &p);
    void remove(const point &p);
    void report(int x1, int x2, int y, const std::string &output_file);
    void print();
  private:
    class node {
    public:
      node(size_t id);
      bool is_leaf();
      bool is_root();
      /*
	Variables
      */
      size_t id;
      std::set<std::pair<point,size_t> > points;
      size_t right_most_child;
      bool is_points_loaded;
    };
    
    enum struct EVENT_TYPE {
      insert_in_base_tree,
      split_node,
      insert_point_in_node
    };
    
    static std::string event_type_to_string(EVENT_TYPE e);
    friend std::ostream& operator<<(std::ostream& o, EVENT_TYPE e) {
      o << event_type_to_string(e);
      return o;
    }

    class event {
    public:
      EVENT_TYPE type;
      node* n;
      point p;
      event(EVENT_TYPE _type, node* _n, point _p) : type(_type), n(_n), p(_p) {}
      ~event() {}
      friend std::ostream& operator<<(std::ostream& o, const event &e) {
        o << e.type << " in node "
	  << e.n->id << " with point "
	  << e.p;
	return o;
      }
    };

    enum struct DATA_TYPE {
      points
    };
    
    /*****************
      Event handling
    ******************/
    void handle_events();
    void add_event(event e);
    void handle_insert_in_base_tree(node* n, const point &p);

    /*
      Helper methods
    */
    void insert_point_in_node(node* n, const point &p);
    node* copy_node(node* n);
    node* find_child(node* n, const point &p);
    void load_data(node* n, DATA_TYPE dt);
    void load_points(node* n);
    void flush_points(node* n);
    std::string get_points_file_name(size_t id);
    std::string get_directory(size_t id);
    node* allocate_node(size_t id);
    node* retrieve_node(size_t id);

    /*
      Private variables
    */
    size_t buffer_size;
    size_t leaf_parameter;
    size_t branching_parameter;

    std::stack<event> event_stack;

    node* root;


  };

  /*
    PUBLIC METHODS
  */
  
  external_priority_search_tree::external_priority_search_tree(size_t buffer_size) {
    this->buffer_size = buffer_size;
    this->leaf_parameter = buffer_size;
    this->branching_parameter = ceil((double)buffer_size/4);

    this->root = allocate_node(0);

    DEBUG_MSG("Constructing with buffer_size = " << buffer_size);
    DEBUG_MSG("leaf parameter = " << leaf_parameter);
    DEBUG_MSG("branching_parameter = " << branching_parameter);
  }

  external_priority_search_tree::external_priority_search_tree(size_t buffer_size, double epsilon)
    : external_priority_search_tree(buffer_size)
  {}

  external_priority_search_tree::~external_priority_search_tree() {}

  void external_priority_search_tree::insert(const point &p) {
    DEBUG_MSG("Starting to insert point " << p);
    add_event(event(EVENT_TYPE::insert_in_base_tree, root, p));

    handle_events();
  }

  void external_priority_search_tree::remove(const point &p) {
    DEBUG_MSG("Starting to remove point " << p);

    handle_events();
  }

  void external_priority_search_tree::report(int x1, int x2, int y, const std::string &output_file) {}

  void external_priority_search_tree::print() {}

  /*
    EVENT HANDLING
  */
  void external_priority_search_tree::handle_events() {

    while ( !event_stack.empty() ) {
      auto cur_event = event_stack.top();
      event_stack.pop();
      DEBUG_MSG("Popped event: " << cur_event << " from the event stack!");
      node* n = cur_event.n;
      point p = cur_event.p;
      switch (cur_event.type) {
      case EVENT_TYPE::insert_in_base_tree:
	load_data(n, DATA_TYPE::points);
	handle_insert_in_base_tree(n, p);
	break;
      default:
	DEBUG_MSG_FAIL("UNHANDLED EVENT!");
	break;

      };
    }
    
  }

  void external_priority_search_tree::add_event(event e) {
    DEBUG_MSG("Adding event to stack: " << e);
    event_stack.push(e);
  }

  std::string external_priority_search_tree::event_type_to_string(EVENT_TYPE e) {
    switch (e) {
    case EVENT_TYPE::insert_in_base_tree:
      return "insert point in base tree";
    case EVENT_TYPE::split_node:
      return "split node";
    case EVENT_TYPE::insert_point_in_node:
      return "insert point in node";
    default:
      return "invalid event type";
    }
  }

  /*
    Search down tree for a leaf z to insert p into
    if z now has more than buffer_size elements then we split
    z into z' and z''
  */
  void external_priority_search_tree::handle_insert_in_base_tree(node* n, const point &p) {
    DEBUG_MSG("Handling insert of " << p << " in base tree at node " << n->id);
#ifdef DEBUG
    assert(n->is_points_loaded);
#endif
    if ( n->is_leaf() ) {
      add_event(event(EVENT_TYPE::split_node, copy_node(n), INF_POINT));
      add_event(event(EVENT_TYPE::insert_point_in_node, copy_node(n), p));
    } else if (false) {
      //add case that the node has the current point and replace it
    } else {
      add_event(event(EVENT_TYPE::insert_in_base_tree, find_child(n, p), p));
    }
  }



  /*
    HELPER METHODS
  */
  void external_priority_search_tree::insert_point_in_node(node* n, const point &p) {
    DEBUG_MSG("Inserting point " << p << " in node " << n->id);
#ifdef DEBUG
    assert(n->is_points_loaded);
#endif
    if ( n->is_leaf() )
      n->points.insert({p, -1});
    // handle for non leafs!
  }

  // Locates the child that point p belongs to
  external_priority_search_tree::node* external_priority_search_tree::find_child(node *n, const point &p) {
    DEBUG_MSG("Find child for point " << p << " in node " << n->id);
#ifdef DEBUG
    assert(n->is_points_loaded);
    assert(!n->is_leaf());
#endif
    size_t child_id = -1;
    auto it = n->points.upper_bound({p,-1});
    if (it == n->points.end()) child_id = n->right_most_child;
    else child_id = it->second;
    return retrieve_node(child_id);
  }

  
  external_priority_search_tree::node* external_priority_search_tree::copy_node(node* n) {
    if ( n->is_root() ) return n;
    return new node(n->id);
  }

  void external_priority_search_tree::load_data(node* n, DATA_TYPE dt) {
    switch (dt) {
    case DATA_TYPE::points:
      {
	// if ( n->is_root() ) break;
	if ( !n->is_points_loaded ) load_points(n);
	break;
      }

    default:
      DEBUG_MSG_FAIL("Cannot load the given data type");
      break;

    };
  }

  void external_priority_search_tree::load_points(node* n) {
#ifdef DEBUG
    assert( !n->is_points_loaded );
#endif
    util::load_file_to_container<std::set<std::pair<point,size_t> >, std::pair<point,size_t> >
      (n->points, get_points_file_name(n->id), buffer_size);
    n->is_points_loaded = true;
  }

  void external_priority_search_tree::flush_points(node* n) {
#ifdef DEBUG
    assert( n->is_points_loaded );
#endif
    util::flush_container_to_file<std::set<std::pair<point, size_t> >::iterator, std::pair<point,size_t> >
      (n->points.begin(), n->points.end(), get_points_file_name(n->id), buffer_size);
  }

  std::string external_priority_search_tree::get_points_file_name(size_t id) {
    return get_directory(id) + "/points";
  }

  std::string external_priority_search_tree::get_directory(size_t id) {
    return std::to_string(id);
  }

  external_priority_search_tree::node* external_priority_search_tree::allocate_node(size_t id) {
    node* n = new node(id);
    n->is_points_loaded = true;
    //create the necessary folder
    mkdir(get_directory(id).c_str(), 0700);
    return n;
  }

  external_priority_search_tree::node* external_priority_search_tree::retrieve_node(size_t id) {
    node* n = new node(id);
    return n;
  }


  /*
    METHODS OF THE NODE
  */

  external_priority_search_tree::node::node(size_t id) {
    this->id = id;
    DEBUG_MSG("Created a node with id: " << id);
  }

  bool external_priority_search_tree::node::is_leaf() {
    return true;
  }

  bool external_priority_search_tree::node::is_root() {
    return id == 0;
  }



  
  
};


#endif
