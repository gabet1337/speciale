#ifndef EXTERNAL_PRIORITY_SEARCH_TREE
#define EXTERNAL_PRIORITY_SEARCH_TREE

#include "../common/point.hpp"
#include "../common/debug.hpp"
#include "../common/validate.hpp"
#include "../common/utilities.hpp"
#include "../common/definitions.hpp"
#include "child_structure_interface.hpp"
#include "child_structure.hpp"
//#include "child_structure_stub.hpp"
#include "../common/pst_interface.hpp"
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

  class external_priority_search_tree : public common::pst_interface {

  public:
    external_priority_search_tree(size_t buffer_size);
    external_priority_search_tree(size_t buffer_size, double epsilon);
    ~external_priority_search_tree();
    void insert(const point &p);
    void remove(const point &p);
    void report(int x1, int x2, int y, const std::string &output_file);
    void print();
#ifdef VALIDATE
    bool is_valid();
#endif
  private:
    typedef struct point_type {
      point pt;
      size_t c;
      bool deleted;
      point_type() {}
      point_type(point _p, size_t _c) : pt(_p), c(_c), deleted(false) {}
      bool operator<(const point_type &p) const {
        return pt < p.pt;
      }
    } point_type;

    //Type definitions:
    typedef std::set<point_type> points_type;
    typedef size_t info_file_entry_type;
    typedef std::vector<info_file_entry_type> info_file_type;
    typedef std::pair<point,point> range_type;
    typedef io::buffered_stream<point> point_stream_type;

    class node {
    public:
      node(size_t id);
      bool is_leaf();
      bool is_root();
      /*
        Variables
      */
      ext::child_structure* query_data_structure;
      size_t id;
      size_t parent_id;
      size_t right_most_child;
      points_type points;
      bool is_points_loaded;
      bool is_info_file_loaded;
      bool is_query_data_structure_loaded;
      bool b_is_leaf;
    };
    
    enum struct EVENT_TYPE {
      insert_in_base_tree,
      delete_in_base_tree,
      split_node,
      insert_point_in_node,
      set_parent_of_children,
      bubble_down,
      bubble_up,
      report,
      report_in_node,
      global_rebuild,
      delete_in_query_data_structure
    };
    
    static std::string event_type_to_string(EVENT_TYPE e);
    friend std::ostream& operator<<(std::ostream& o, EVENT_TYPE e) {
      o << event_type_to_string(e);
      return o;
    }

    class event {
    public:
      EVENT_TYPE type;
      node* n = 0;
      node* n2 = 0;
      point p = INF_POINT;
      int x1 = INF, x2 = INF, y = INF;
      bool lm = false,rm = false;
      point_stream_type* stream = 0;
      event(EVENT_TYPE _type, node* _n, point _p) : type(_type), n(_n), n2(0), p(_p) {}
      event(EVENT_TYPE _type, node* _n, node* _n2, point _p) : type(_type), n(_n), n2(_n2), p(_p) {}
      event(EVENT_TYPE _type, node* _n, int _x1, int _x2, int _y, bool _lm, bool _rm, point_stream_type* _stream)
        : type(_type), n(_n), n2(0), p(INF_POINT), x1(_x1), x2(_x2), y(_y), lm(_lm), rm(_rm), stream(_stream) {}
      ~event() {}
      friend std::ostream& operator<<(std::ostream& o, const event &e) {
        o << e.type << " in node " << e.n->id;
        if (e.p != INF_POINT) o << " with point " << e.p;
        if (e.n2) o << " and extra node " << e.n2->id;
        if ( !(e.x1 == INF && e.x2 == INF && e.y == INF) ) {
          o << " report: [" << e.x1 << ", " << e.x2 << "] X [" << e.y << ", \u221E] ";
          o << (e.lm ? "LM" : "") << " " << (e.rm ? "RM" : "");
        }
        return o;
      }
    };

    enum struct DATA_TYPE {
      points,
      info_file,
      query_data_structure,
      all
    };

        
    static std::string data_type_to_string(DATA_TYPE dt);
    friend std::ostream& operator<<(std::ostream& o, DATA_TYPE dt) {
      o << data_type_to_string(dt);
      return o;
    }
    
    /*****************
      Event handling
    ******************/
    void handle_events();
    void add_event(event e);
    void handle_insert_in_base_tree(node* n, const point &p);
    void handle_delete_in_base_tree(node* n, const point &p);
    void handle_delete_in_query_data_structure(node* n, const point &p);
    void handle_split_child(node* parent, node* n, node* new_node, bool split_root);
    void handle_set_parent_of_children(node* n, std::vector<node*> &children);
    void handle_bubble_down(node* n, const point &p);
    void handle_bubble_up(node* parent, node* n);
    void handle_report(node* n, int x1, int x2, int y, bool lm, bool rm, point_stream_type* stream);
    void handle_report_in_node(node* n, int x1, int x2, int y, bool lm, bool rm, point_stream_type* stream);
    void handle_global_rebuild();

    /***************
      Helper methods
    ****************/
    void insert_point_in_node(node* n, const point &p);
    void load_data(node* n, DATA_TYPE dt);
    void flush_data(node* n, DATA_TYPE dt);
    void load_points(node* n);
    void load_info_file(node* n);
    void load_query_data_structure(node* n);
    void flush_points(node* n);
    void flush_info_file(node* n);
    void flush_query_data_structure(node* n);
    void remove_query_data_structure(node* n);
    void distribute_query_data_structure(node* qs_node, node* left, node* right, const point &split_point);
    node* copy_node(node* n);
    node* allocate_node();
    node* retrieve_node(size_t id);
    node* find_child(node* n, const point &p);
    range_type find_range(node* n, const point &p);
    range_type find_range(node* n, node* child);
    point find_top_most_point(node* n);
    std::vector<point> get_Y_set(node *n, const range_type &range);
    std::string get_points_file_name(size_t id);
    std::string get_info_file_file_name(size_t id);
    std::string get_directory(size_t id);
    bool is_degree_overflow(node* n);
    bool point_below_all(const point &p, const std::vector<point> &points);

#ifdef VALIDATE
    bool validate_child(node& n, size_t c, const point &all_points_in_qs_should_be_below_this, std::stack<point> &qs, std::stack<size_t> &qs_size);
#endif

    /*******************
      Private variables
    ********************/
    size_t next_id;
    size_t buffer_size;
    size_t leaf_parameter;
    size_t branching_parameter;
    size_t next_global_rebuild;
    size_t epoch_start_id;
    size_t delete_count;
    std::stack<event> event_stack;

    node* root;

#ifdef VALIDATE
    std::set<point> CONTAINED_POINTS;
#endif

  };

  /****************
    PUBLIC METHODS
  *****************/
  
  external_priority_search_tree::external_priority_search_tree(size_t buffer_size) {
    assert(buffer_size >= 12);
    this->next_id = 0;
    this->buffer_size = buffer_size;
    this->leaf_parameter = buffer_size;
    this->branching_parameter = std::max((size_t)2,(size_t)ceil((double)buffer_size/4));
    this->next_global_rebuild = 1;
    this->delete_count = 0;
    this->epoch_start_id = 0;
    
    this->root = allocate_node();

    DEBUG_MSG("Constructing with buffer_size = " << buffer_size);
    DEBUG_MSG("leaf parameter = " << leaf_parameter);
    DEBUG_MSG("branching_parameter = " << branching_parameter);
  }

  external_priority_search_tree::external_priority_search_tree(size_t buffer_size, double epsilon)
    : external_priority_search_tree(buffer_size)
  {}

  external_priority_search_tree::~external_priority_search_tree() {
    root->query_data_structure->destroy();
    delete root->query_data_structure;
    delete root;
    for (size_t i = 0; i <= next_id; i++) {
      util::remove_directory(std::to_string(i));
      util::remove_directory("c_"+std::to_string(i));
    }
  }

  void external_priority_search_tree::insert(const point &p) {
    DEBUG_MSG("Starting to insert point " << p);
#ifdef VALIDATE
    CONTAINED_POINTS.insert(p);
#endif
    add_event(event(EVENT_TYPE::insert_in_base_tree, root, p));
    handle_events();
    add_event(event(EVENT_TYPE::bubble_down, root, p));
    handle_events();
  }

  void external_priority_search_tree::remove(const point &p) {
    DEBUG_MSG("Starting to remove point " << p);
#ifdef VALIDATE
    CONTAINED_POINTS.erase(p);
#endif
    add_event(event(EVENT_TYPE::delete_in_query_data_structure, root, p));
    add_event(event(EVENT_TYPE::delete_in_base_tree, root, p));
    handle_events();
    delete_count++;
    if (delete_count == next_global_rebuild) {
      add_event(event(EVENT_TYPE::global_rebuild, root, INF_POINT));
      handle_events();
    }
  }

  void external_priority_search_tree::report(int x1, int x2, int y, const std::string &output_file) {
    DEBUG_MSG("Starting to report [" << x1 << ", " << x2 << "] X [" << y << ", \u221E]");
    if (x1 > x2) return;
    point_stream_type* stream = new point_stream_type(buffer_size);
    stream->open(output_file);
    add_event(event(EVENT_TYPE::report_in_node, root, x1, x2, y, true, true, stream));
    handle_events();
    stream->close();
    delete stream;
  }

  void external_priority_search_tree::print() {
    DEBUG_MSG("Starting to print");
    std::ofstream dot_file;
    dot_file.open("temp.dot");
    dot_file << "digraph {\n";
    
    std::queue<node*> q;
    q.push(root);
    while (!q.empty()) {
      node* n = q.front(); q.pop();
      load_data(n, DATA_TYPE::all);
      //print label:
      dot_file << n->id << " [shape=folder label=\"" << n->id << "\nParent: " << n->parent_id << "\nis_leaf: " << n->is_leaf() << "\nPoints: ";
      //print points:
      size_t idx = 1;
      for (auto p : n->points) { dot_file << (p.deleted ? "x" : "") << p.pt << ", "; if (++idx % 8 == 0) dot_file << "\n"; }
      //print query data structure:
      dot_file << "\nQ: ";
      idx = 1;
      for (auto p : n->query_data_structure->get_points()) { dot_file << p << ", "; if (++idx % 8 == 0) dot_file << "\n"; }
      dot_file << "\"]\n";
      //print children:
      if ( !n->is_leaf() ) {
        for (auto p : n->points) {
          dot_file << n->id << " -> " << p.c << "[label=\"" << p.pt << "\"]\n";
          q.push(retrieve_node(p.c));
        }
        if (n->right_most_child != (size_t)-1) {
          dot_file << n->id << " -> " << n->right_most_child << "[label=\"rm\"]\n";
          q.push(retrieve_node(n->right_most_child));
        }
      }
      flush_data(n, DATA_TYPE::all);
      if ( !n->is_root() ) delete n;
    }
    dot_file << "}";
    dot_file.close();
    int r = system("dot -Tpng -o tree.png temp.dot");
    //r = system("eog tree.png");
    r++;
  }
#ifdef VALIDATE
  bool external_priority_search_tree::is_valid() {
    VALIDATE_MSG("Starting to test if structure is valid");
    typedef std::pair<point,point> pp;
    std::stack<node*> s;
    std::stack<pp> ranges;
    std::stack<point> qs;
    std::stack<size_t> qs_size;
    s.push(root);
    ranges.push({MINUS_INF_POINT, INF_POINT});
    qs.push(INF_POINT);
    qs_size.push(INF);
    std::set<point> collected_points;
    std::set<point> collected_points_in_qds;

    while (!s.empty()) {
      node* n = s.top(); s.pop();
      pp range = ranges.top(); ranges.pop();
      point all_points_in_qs_should_be_below_this = qs.top(); qs.pop();
      size_t parent_qs_size = qs_size.top(); qs_size.pop();
      // VALIDATE_MSG_FAIL("1: " << n->id);
      load_data(n, DATA_TYPE::all);

      //test that nodes stores points:
      if (n->points.empty()) {
        VALIDATE_MSG_FAIL("node " << n->id << " does not store any points");
        return false;
      }

      // ITERATES ALL POINTS, SO DO CHECKS OF THEM IN HERE:
      for (auto p : n->points) {
        // add points to collected points:
        if (!p.deleted)
          collected_points.insert(p.pt);

        // test if points are in the correct range:
        if (p.pt < range.first || p.pt > range.second) {
          VALIDATE_MSG_FAIL(p.pt << " is not in the range [" << range.first << ", " << range.second << "] in node " << n->id);
          return false;
        }

        // VALIDATE_MSG_FAIL("2: " << n->id);
        // VALIDATE_MSG_FAIL("3: " << p.c);
        if (!validate_child(*n, p.c, all_points_in_qs_should_be_below_this, qs, qs_size)) return false;
      }

      //REMEMBER THE RIGHT MOST CHILD!
      // VALIDATE_MSG_FAIL("4: " << n->id);
      // VALIDATE_MSG_FAIL("5: " << n->right_most_child);
      if (!validate_child(*n, n->right_most_child, all_points_in_qs_should_be_below_this, qs, qs_size)) return false;

      std::vector<point> qds = n->query_data_structure->get_points();

      //add query data structure points to collected_points_in_qds
      collected_points_in_qds.insert(qds.begin(), qds.end());
      
      // test that the query data structure of a leaf is not too large!
      if ( n->is_leaf() ) {
        if (qds.size() > 2*leaf_parameter) {
          VALIDATE_MSG_FAIL("Size of query data structure in leaf " << n->id << " is too large: " << qds.size());
          return false;
        }
      }

      // test that if the query data structure is non empty then Y(n) in parent(n) >= buffer_size/2
      if (!qds.empty() && parent_qs_size < buffer_size/2) {
        VALIDATE_MSG_FAIL("node " << n->id << " has non empty query data structure but Y(" << n->id << ") is not >= buffer_size/2: " << parent_qs_size);
        return false;
      }

      // Add children and ranges to stacks:
      if ( !n->is_leaf() ) {
        bool first = true;
        for (auto p : n->points) {
          //VALIDATE_MSG_FAIL("6: " << n->id);
          //VALIDATE_MSG_FAIL("7: " << p.c);
          s.push(retrieve_node(p.c));
          if (first) {
            ranges.push({range.first, p.pt});
            first = false;
          } else {
            ranges.push({ranges.top().second, p.pt});
          }
        }
        //VALIDATE_MSG_FAIL("8: " << n->id);
        //VALIDATE_MSG_FAIL("9: " << n->right_most_child);
        s.push(retrieve_node(n->right_most_child));
        ranges.push({ranges.top().second, range.second});
      }
      //VALIDATE_MSG_FAIL("10: " << n->id);
      flush_data(n, DATA_TYPE::all);
      if ( !n->is_root() ) delete n;
    }

    if (collected_points != CONTAINED_POINTS) {
      VALIDATE_MSG_FAIL("The collected points were not equal to the actual points");
      VALIDATE_MSG_FAIL("Collected points:");
      for (point p : collected_points) VALIDATE_MSG_FAIL(" - " << p);
      VALIDATE_MSG_FAIL("Contained points:");
      for (point p : CONTAINED_POINTS) VALIDATE_MSG_FAIL(" - " << p);
      return false;
    }
    if (collected_points_in_qds != CONTAINED_POINTS) {
      VALIDATE_MSG_FAIL("The collected points in the query data structure were not equal to the actual points");
      VALIDATE_MSG_FAIL("Collected points:");
      for (point p : collected_points_in_qds) VALIDATE_MSG_FAIL(" - " << p);
      VALIDATE_MSG_FAIL("Contained points:");
      for (point p : CONTAINED_POINTS) VALIDATE_MSG_FAIL(" - " << p);
      return false;
    }
    return true;
  }

  bool external_priority_search_tree::validate_child(node& n, size_t c, const point &all_points_in_qs_should_be_below_this, std::stack<point> &qs, std::stack<size_t> &qs_size) {
    //VALIDATE_MSG_FAIL("Validating child " << c << " of node " << n.id);
    if ( n.is_leaf() ) {
      //test that leafs doesnt have any children
      if ( c != (size_t)-1 ) {
        VALIDATE_MSG_FAIL("leaf " << n.id << " has a child " << c << " but should not have!");
        return false;
      }
      return true;
    }
    if ( !n.is_leaf() ) {
      //test that internal nodes does not have invalid children!
      if ( c == (size_t)-1 ) {
        VALIDATE_MSG_FAIL("internal node " << n.id << " has a child with invalid id: " << c);
        return false;
      }
    }

    node* child = retrieve_node(c);
    //VALIDATE_MSG_FAIL("11: " << child->id);
    load_data(child, DATA_TYPE::all);
    range_type range_of_child = find_range(&n, child);
    std::vector<point> Y_set = get_Y_set(&n, range_of_child);
    qs_size.push(Y_set.size());
    if (Y_set.empty()) qs.push(MINUS_INF_POINT);
    else qs.push(*std::min_element(Y_set.begin(), Y_set.end(), comp_y));
    //test if Y_set is at most buffer_size points
    if (Y_set.size() > buffer_size) {
      VALIDATE_MSG_FAIL("Y(" << c << ") has too large size: " << Y_set.size() << " in node " << n.id);
      return false;
    }

    // test that all points in the query data structure of w is in heap order according to the Y(w) in parent(w)
    if (!std::all_of(Y_set.begin(), Y_set.end(),
                     [&all_points_in_qs_should_be_below_this](const point &p) {
                       return comp_y(p, all_points_in_qs_should_be_below_this);
                     })) {
      VALIDATE_MSG_FAIL("A point in the query data structure is above a point in the Y set of the parent in node " << n.id);
      return false;
    }
    //VALIDATE_MSG_FAIL("12: " << child->id);
    flush_data(child, DATA_TYPE::all);
    if (!child->is_root()) delete child;
    return true;
  }
#endif

  /*
    EVENT HANDLING
  */
  void external_priority_search_tree::handle_events() {

    while ( !event_stack.empty() ) {
      auto cur_event = event_stack.top();
      event_stack.pop();
      DEBUG_MSG("Popped event: " << cur_event << " from the event stack!");
      node* n = cur_event.n;
      node* n2 = cur_event.n2;
      point p = cur_event.p;
      switch (cur_event.type) {
      case EVENT_TYPE::insert_in_base_tree:
        load_data(n, DATA_TYPE::points);
        load_data(n, DATA_TYPE::info_file);
        handle_insert_in_base_tree(n, p);
        flush_data(n, DATA_TYPE::points);
        flush_data(n, DATA_TYPE::info_file);
        break;
      case EVENT_TYPE::delete_in_base_tree:
        load_data(n, DATA_TYPE::points);
        load_data(n, DATA_TYPE::info_file);
        handle_delete_in_base_tree(n, p);
        flush_data(n, DATA_TYPE::points);
        flush_data(n, DATA_TYPE::info_file);
        break;
      case EVENT_TYPE::delete_in_query_data_structure:
        load_data(n, DATA_TYPE::all);
        handle_delete_in_query_data_structure(n,p);
        flush_data(n, DATA_TYPE::all);
        break;
      case EVENT_TYPE::insert_point_in_node:
        load_data(n, DATA_TYPE::points);
        load_data(n, DATA_TYPE::info_file);
        insert_point_in_node(n, p);
        flush_data(n, DATA_TYPE::points);
        flush_data(n, DATA_TYPE::info_file);
        break;
      case EVENT_TYPE::split_node:
        load_data(n, DATA_TYPE::points);
        load_data(n, DATA_TYPE::info_file);
        if ( !is_degree_overflow(n) ) {
          flush_data(n, DATA_TYPE::all);
          break;
        }
        if ( n->is_root() ) {
          load_data(n, DATA_TYPE::query_data_structure); 
          root = n;
          //make the root the child of a new empty node
          node* empty_node = allocate_node(); // this is going to be the new root!
          node* new_node = allocate_node();

          root->parent_id = 0;
          root->id = empty_node->id;
          empty_node->id = 0;
          empty_node->b_is_leaf = false;
          std::swap(root->query_data_structure, empty_node->query_data_structure);
          
          handle_split_child(empty_node, root, new_node, true);
          //flush and delete properly here!

          flush_data(empty_node, DATA_TYPE::all);
          flush_data(root, DATA_TYPE::all);
          flush_data(new_node, DATA_TYPE::all);
          root = empty_node;
          // since root = n and n now no longer is root then we delete it later after switch. so dont worry.
          delete new_node;
        } else {
          load_data(n, DATA_TYPE::query_data_structure); 
          node* parent = retrieve_node(n->parent_id);
          node* new_node = allocate_node();
          
          load_data(parent, DATA_TYPE::points);
          load_data(parent, DATA_TYPE::info_file);

          handle_split_child(parent, n, new_node, false);
          flush_data(parent, DATA_TYPE::all);
          flush_data(new_node, DATA_TYPE::all);
          flush_data(n, DATA_TYPE::all);
          delete new_node;
          if (!parent->is_root()) delete parent;
        }
        break;
      case EVENT_TYPE::set_parent_of_children:
        {
          load_data(n, DATA_TYPE::info_file);
          if ( n->is_leaf() ) {
            flush_data(n, DATA_TYPE::info_file);
            break;
          }
          load_data(n, DATA_TYPE::points);
          std::vector<node*> children;
          for (auto c : n->points) {
            children.push_back(retrieve_node(c.c));
            load_data(children.back(), DATA_TYPE::info_file);
          }
          flush_data(n, DATA_TYPE::points);
          children.push_back(retrieve_node(n->right_most_child));
          load_data(children.back(), DATA_TYPE::info_file);
          
          handle_set_parent_of_children(n, children);
          
          for (auto c : children) {
            flush_data(c, DATA_TYPE::info_file);
            delete c;
          }

          flush_data(n, DATA_TYPE::info_file);
        }
        break;
      case EVENT_TYPE::bubble_down:
        {
          load_data(n, DATA_TYPE::all);
          handle_bubble_down(n,p);
          flush_data(n, DATA_TYPE::all);
        }
        break;
      case EVENT_TYPE::bubble_up:
        {
          load_data(n2, DATA_TYPE::all);
          load_data(n, DATA_TYPE::all);
          
          handle_bubble_up(n,n2);

          flush_data(n, DATA_TYPE::all);
          flush_data(n2, DATA_TYPE::all);
        }
        break;
      case EVENT_TYPE::report:
        {
          load_data(n, DATA_TYPE::all);
          handle_report(n, cur_event.x1, cur_event.x2, cur_event.y, cur_event.lm, cur_event.rm, cur_event.stream);
          flush_data(n, DATA_TYPE::all);
        }
        break;
      case EVENT_TYPE::report_in_node:
        {
          load_data(n, DATA_TYPE::query_data_structure);
          handle_report_in_node(n, cur_event.x1, cur_event.x2, cur_event.y, cur_event.lm, cur_event.rm, cur_event.stream);
          flush_data(n, DATA_TYPE::query_data_structure);
        }
        break;
      case EVENT_TYPE::global_rebuild:
        {
          break;
          handle_global_rebuild();
        }
        break;
      default:
        DEBUG_MSG_FAIL("UNHANDLED EVENT!");
        break;

      };
      flush_data(n, DATA_TYPE::all);
      if ( !n->is_root() ) delete n;
      if ( n2 && !n2->is_root() ) delete n2;
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
    case EVENT_TYPE::delete_in_base_tree:
      return "delete point in base tree";
    case EVENT_TYPE::split_node:
      return "split node";
    case EVENT_TYPE::insert_point_in_node:
      return "insert point";
    case EVENT_TYPE::set_parent_of_children:
      return "set parent of children";
    case EVENT_TYPE::bubble_down:
      return "bubble down";
    case EVENT_TYPE::bubble_up:
      return "bubble up";
    case EVENT_TYPE::global_rebuild:
      return "global rebuild";
    case EVENT_TYPE::delete_in_query_data_structure:
      return "delete point in query data structure";
    case EVENT_TYPE::report:
      return "report";
    case EVENT_TYPE::report_in_node:
      return "report in node";
    default:
      return "invalid event type";
    }
  }

  std::string external_priority_search_tree::data_type_to_string(DATA_TYPE dt) {
    switch (dt) {
    case DATA_TYPE::points:
      return "points";
    case DATA_TYPE::info_file:
      return "info file";
    case DATA_TYPE::query_data_structure:
      return "query data structure";
    case DATA_TYPE::all:
      return "all";
    default:
      return "invalid data type";
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
    assert( n->is_points_loaded );
    assert( n->is_info_file_loaded );
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

  void external_priority_search_tree::handle_delete_in_base_tree(node* n, const point &p) {
    DEBUG_MSG("Handling delete of " << p << " in base tree at node " << n->id);
#ifdef DEBUG
    assert( n->is_points_loaded );
    assert( n->is_info_file_loaded );
#endif
    points_type::iterator it = n->points.find(point_type(p,-1));
    if (it != n->points.end()) {
      point_type deref_it = *it;
      deref_it.deleted = true;
      n->points.erase(it);
      n->points.insert(deref_it);
      
    } else if ( !n->is_leaf() )
      add_event(event(EVENT_TYPE::delete_in_base_tree, find_child(n, p), p));
  }



  void external_priority_search_tree::handle_split_child(node* parent, node* n, node* new_node, bool split_root) {
    DEBUG_MSG("Handling split of node " << n->id << " with parent " << parent->id);
#ifdef DEBUG
    assert( n->is_points_loaded );
    if (!split_root) assert( n->is_query_data_structure_loaded );
    assert( parent->is_points_loaded );
    assert( parent->is_info_file_loaded );
#endif
    //allocate new node and distribute points around median.
    //move median to parent and possibly recurse on parent
    new_node->parent_id = parent->id;
    new_node->b_is_leaf = n->is_leaf();

#ifdef DEBUG
    assert( new_node->is_points_loaded);
    assert( new_node->is_info_file_loaded);
    assert( new_node->is_query_data_structure_loaded);
#endif
    
    size_t median = n->points.size() / 2;
    points_type points(n->points.begin(), n->points.end());
    n->points.clear();

    size_t idx = 0;
    point_type split_point = point_type(MINUS_INF_POINT, -1);
    for (point_type p : points) {
      if (idx < median) new_node->points.insert(p);
      else if (idx == median) {
        split_point = p;
        parent->points.insert(point_type(p.pt, new_node->id));
        new_node->right_most_child = p.c;
      }
      else n->points.insert(p);
      idx++;
    }

    if (split_root) {
      //distribute the query_data_structure of parent between n and new_node according to median
      distribute_query_data_structure(parent, new_node, n, split_point.pt);
    } else {
      //distrubute the query_data_structure of n between n and new_node according to median
      distribute_query_data_structure(n, new_node, n, split_point.pt);
    }

    if (split_root) {
      parent->right_most_child = n->id;
    }

    add_event(event(EVENT_TYPE::split_node, copy_node(parent), INF_POINT));
    add_event(event(EVENT_TYPE::bubble_up, copy_node(parent), copy_node(new_node), INF_POINT)); //we want to know the node aswell
    add_event(event(EVENT_TYPE::bubble_up, copy_node(parent), copy_node(n), INF_POINT)); //we want to know the node aswell
    // if (split_point.c != (size_t)-1) {
    //   add_event(event(EVENT_TYPE::bubble_up, copy_node(n), retrieve_node(split_point.c), INF_POINT));
    //   add_event(event(EVENT_TYPE::bubble_up, copy_node(new_node), retrieve_node(split_point.c), INF_POINT));
    // }
    add_event(event(EVENT_TYPE::set_parent_of_children, copy_node(n), INF_POINT));
    add_event(event(EVENT_TYPE::set_parent_of_children, copy_node(new_node), INF_POINT));
  }


  void external_priority_search_tree::handle_set_parent_of_children(node* n, std::vector<node*> &children) {
    DEBUG_MSG("Settings the parent of the children of node " << n->id);
#ifdef DEBUG
    for (auto c : children) assert( c->is_info_file_loaded );
#endif
    for (auto c : children) c->parent_id = n->id;
  }

  void external_priority_search_tree::handle_delete_in_query_data_structure(node* n, const point &p) {
    DEBUG_MSG("Handling delete of " << p << " in query data structure at node " << n->id);
#ifdef DEBUG
    assert( n->is_query_data_structure_loaded);
    assert( n->is_points_loaded);
    assert( n->is_info_file_loaded);
#endif
    if ( n->is_leaf() ) {
      n->query_data_structure->remove(p);
      //add_event(event(EVENT_TYPE::bubble_up, retrieve_node(n->parent_id), copy_node(n), INF_POINT));
    } else {
      auto Y_set = get_Y_set(n, find_range(n,p));
      node* child = find_child(n,p);
      if (std::find(Y_set.begin(), Y_set.end(), p) != Y_set.end()) {
        n->query_data_structure->remove(p);
        add_event(event(EVENT_TYPE::bubble_up, copy_node(n), copy_node(child), INF_POINT));
      } else {
        add_event(event(EVENT_TYPE::delete_in_query_data_structure, copy_node(child), p));        
      }
      delete child;
    }
  }

  void external_priority_search_tree::handle_bubble_down(node *n, const point &p) {
    DEBUG_MSG("Bubble down " << p << " in node " << n->id);
#ifdef DEBUG
    assert( n->is_points_loaded );
    assert( n->is_info_file_loaded );
    assert( n->is_query_data_structure_loaded );
#endif
    if ( n->is_leaf() ) {
      DEBUG_MSG(n->id << " is a leaf");
      //TODO: insert into Lv but for now just insert in query data structure for simplicity
      n->query_data_structure->insert(p);
      return;
    }

    range_type range_of_child = find_range(n,p);
    std::vector<point> Y_set_child = get_Y_set(n, range_of_child);
    node* child = find_child(n,p);
    if (Y_set_child.size() >= buffer_size/2 && point_below_all(p, Y_set_child)) { // not clear if buffer_size/2 or buffer_size. ionote says buffer_size
      add_event(event(EVENT_TYPE::bubble_down, copy_node(child), p));
    } else {
      n->query_data_structure->insert(p);
      if (Y_set_child.size() >= buffer_size) {
        point min_point = *std::min_element(Y_set_child.begin(), Y_set_child.end(), comp_y);
        n->query_data_structure->remove(min_point);
        add_event(event(EVENT_TYPE::bubble_down, copy_node(child), min_point));
      }
    }
    delete child;
  }

  void external_priority_search_tree::handle_bubble_up(node* parent, node *n) {
    DEBUG_MSG("Bubble up from node " << n->id << " to " << parent->id);
#ifdef DEBUG
    assert(parent->is_query_data_structure_loaded);
    assert(parent->is_points_loaded);
    assert(parent->is_info_file_loaded);
    assert(n->is_query_data_structure_loaded);
    assert(n->is_info_file_loaded);
    assert(n->is_points_loaded);
#endif
    
    std::vector<point> Y_set_child = get_Y_set(parent, find_range(parent, n));
    DEBUG_MSG("Size of Y(" << n->id << ") = " << Y_set_child.size());
    if (Y_set_child.size() >= buffer_size/2) return;

    point top_most = find_top_most_point(n);
    if (top_most == MINUS_INF_POINT) return;
    n->query_data_structure->remove(top_most);
    parent->query_data_structure->insert(top_most);

    if (Y_set_child.size() + 1 < buffer_size/2) {
      add_event(event(EVENT_TYPE::bubble_up, copy_node(parent), copy_node(n), INF_POINT));
    }
    
    if ( !n->is_leaf() ) {
      node* child_of_point = find_child(n, top_most);
      add_event(event(EVENT_TYPE::bubble_up, copy_node(n), copy_node(child_of_point), INF_POINT));
      delete child_of_point;
    }
  }

  void external_priority_search_tree::handle_report(node* n, int x1, int x2, int y, bool lm, bool rm, point_stream_type* stream) {
    DEBUG_MSG("Handle report of points in node " << n->id << " with Q = [" << x1 << ", " << x2 << "] X [ " << y << ", \u221E] and rm = " << rm << ", lm = " << lm);
#ifdef DEBUG
    assert(n->is_points_loaded);
    assert(n->is_info_file_loaded);
#endif

    if ( !n->is_leaf() ) {
      std::vector<event> upcoming_events;
      node* left_most = find_child(n, point(x1,-INF));
      node* right_most = find_child(n, point(x2,INF));
      bool in_range = false;
      for (auto p : n->points) {
        if (p.c == left_most->id) in_range = true;
        if (!in_range) continue;
        
        add_event(event(EVENT_TYPE::report_in_node,
                        retrieve_node(p.c),
                        x1, x2, y,
                        lm && p.c == left_most->id,
                        rm && p.c == right_most->id,
                        stream));
        
        if (p.c == right_most->id) break;
      }
      if (right_most->id == n->right_most_child) {
        add_event(event(EVENT_TYPE::report_in_node,
                        retrieve_node(n->right_most_child),
                        x1, x2, y,
                        lm && n->right_most_child == left_most->id,
                        rm && n->right_most_child == right_most->id,
                        stream));
      }
    }
  }

  void external_priority_search_tree::handle_report_in_node(node* n, int x1, int x2, int y, bool lm, bool rm, point_stream_type* stream) {
    DEBUG_MSG("Reporting of points in node " << n->id << " with Q = [" << x1 << ", " << x2 << "] X [ " << y << ", \u221E] and rm = " << rm << ", lm = " << lm);
#ifdef DEBUG
    assert(n->is_query_data_structure_loaded);
#endif
    std::vector<point> points_to_report_in_node = n->query_data_structure->report(x1, x2, y);
    for (point p : points_to_report_in_node)
      stream->write(p);
    if (points_to_report_in_node.empty() && !(lm || rm)) return;
    add_event(event(EVENT_TYPE::report, copy_node(n), x1, x2, y, lm, rm, stream));
  }

  void external_priority_search_tree::handle_global_rebuild() {
    size_t active_points = 0;
    size_t cur_id = next_id;
    
    node* old_root = root;
    root = allocate_node();
    root->id = 0;
    
#ifdef DEBUG
    std::set<point> old_contained_points(CONTAINED_POINTS.begin(), CONTAINED_POINTS.end());
    CONTAINED_POINTS.clear();
#endif
    //traverse tree and insert all non deleted points in a new tree
    std::queue<node*> bfs;
    bfs.push(old_root);

    while (!bfs.empty()) {
      node* n = bfs.front(); bfs.pop();
      load_data(n, DATA_TYPE::points);
      load_data(n, DATA_TYPE::info_file);
      for (auto p : n->points) {
        if (!p.deleted) {
          add_event(event(EVENT_TYPE::insert_in_base_tree, root, p.pt));
          active_points++;
        }
        handle_events();
        if (!n->is_leaf())
          bfs.push(retrieve_node(p.c));
      }
      flush_data(n, DATA_TYPE::points);
      flush_data(n, DATA_TYPE::info_file);
      delete n;
    }

    //delete all nodes upto cur_id and remember where we started
    for (size_t i = epoch_start_id; i < cur_id; i++) {
      util::remove_directory(std::to_string(i));
      util::remove_directory("c_"+std::to_string(i));
    }

#ifdef DEBUG
    assert(CONTAINED_POINTS == old_contained_points);
#endif
    
    epoch_start_id = cur_id;
    delete_count = 0;
    next_global_rebuild = std::max(active_points, (size_t)1);
  }  

  /*
    HELPER METHODS
  */
  void external_priority_search_tree::insert_point_in_node(node* n, const point &p) {
    DEBUG_MSG("Inserting point " << p << " in node " << n->id);
#ifdef DEBUG
    assert(n->is_points_loaded);
    assert(n->is_info_file_loaded);
    assert(n->is_leaf());
#endif
    if ( n->is_leaf() )
      n->points.insert(point_type(p, (size_t)-1));
    // handle for non leafs if we should replace a point!
  }

  // Locates the child that point p belongs to
  // each point k stores a pointer to a child c.
  // Invariant: k-1 < all keys of c < k
  // If p > all keys of n then we have pointer to that in right_most_child
  external_priority_search_tree::node* external_priority_search_tree::find_child(node *n, const point &p) {
    DEBUG_MSG("Find child for point " << p << " in node " << n->id);
#ifdef DEBUG
    assert(n->is_points_loaded);
    assert(n->is_info_file_loaded);
    assert(!n->is_leaf());
#endif
    size_t child_id = (size_t)-1;
    auto it = n->points.upper_bound(point_type(p,(size_t)-1));
    if (it == n->points.end()) child_id = n->right_most_child;
    else child_id = it->c;
    return retrieve_node(child_id);
  }

  external_priority_search_tree::range_type external_priority_search_tree::find_range(node *n, const point &p) {
    DEBUG_MSG("Find range of point " << p << " in node " << n->id);
#ifdef DEBUG
    assert(n->is_points_loaded);
    assert(n->is_info_file_loaded);
    assert(!n->is_leaf());
#endif
    //this gon' be ugly
    auto it = n->points.upper_bound(point_type(p,-1));
    if (it == n->points.end()) {
      return range_type(n->points.rbegin()->pt, INF_POINT);
    } else if (it == n->points.begin()) {
      return range_type(MINUS_INF_POINT, point(n->points.begin()->pt.x, n->points.begin()->pt.y-1));
    } else {
      auto right = it;
      return range_type((--it)->pt, point(right->pt.x, right->pt.y-1));
    }
  }

  external_priority_search_tree::range_type external_priority_search_tree::find_range(node *n, node* child) {
    DEBUG_MSG("Find range of child " << child->id << " in node " << n->id);
#ifdef DEBUG
    assert(n->is_points_loaded);
    assert(n->is_info_file_loaded);
    assert(!n->is_leaf());
#endif

    for (auto it = n->points.begin(); it != n->points.end(); ++it) {
      if (it->c == child->id) {
        if (it == n->points.begin()) return range_type(MINUS_INF_POINT, point(it->pt.x, it->pt.y-1));
        else {
          auto right = it;
          return range_type((--it)->pt, point(right->pt.x, right->pt.y-1));
        }
      }
    }
    return range_type(n->points.rbegin()->pt, INF_POINT);
  }

  point external_priority_search_tree::find_top_most_point(node* n) {
#ifdef DEBUG
    assert(n->is_query_data_structure_loaded);
#endif
    point top_most = n->query_data_structure->get_top_most_point();
    DEBUG_MSG("Found top most point to be " << top_most);
    return top_most;
  }

  std::vector<point> external_priority_search_tree::get_Y_set(node *n, const range_type &range) {
#ifdef DEBUG
    assert(n->is_query_data_structure_loaded);
#endif
    return n->query_data_structure->report(range.first, range.second, -INF);
  }
  
  external_priority_search_tree::node* external_priority_search_tree::copy_node(node* n) {
    if ( n->is_root() ) return n;
    return new node(n->id);
  }

  bool external_priority_search_tree::point_below_all(const point &p, const std::vector<point> &points) {
    // returns true if p is below all points in points. false otherwise
    for (point pt : points)
      if (pt.y < p.y || (pt.y == p.y && pt.x <= p.x)) return false;
    return true;
  }

  void external_priority_search_tree::remove_query_data_structure(node* n) {
    if (n->query_data_structure) {
      n->query_data_structure->destroy();
      delete n->query_data_structure;
      n->query_data_structure = 0;
    }
  }

  void external_priority_search_tree::distribute_query_data_structure(node* qs_node, node* left, node *right, const point &split_point) {
#ifdef DEBUG
    assert(qs_node->is_query_data_structure_loaded);
    assert(left->is_query_data_structure_loaded);
    assert(right->is_query_data_structure_loaded);
#endif
    
    std::vector<point> qs_left = qs_node->query_data_structure->report(MINUS_INF_POINT, split_point, -INF);
    std::vector<point> qs_right = qs_node->query_data_structure->report(split_point, INF_POINT, -INF);
    remove_query_data_structure(qs_node);
    remove_query_data_structure(left);
    remove_query_data_structure(right);
    left->query_data_structure = new ext::child_structure(left->id, buffer_size, 1, qs_left);
    right->query_data_structure = new ext::child_structure(right->id, buffer_size, 1, qs_right);
    left->query_data_structure->remove(split_point);
    if (!qs_node->query_data_structure) qs_node->query_data_structure = new ext::child_structure(qs_node->id, buffer_size, 1, std::vector<point>());
  }


  void external_priority_search_tree::load_data(node* n, DATA_TYPE dt) {
    if ( n->is_root() ) return;
    switch (dt) {
    case DATA_TYPE::points:
      {
        if ( !n->is_points_loaded ) load_points(n);
        break;
      }
    case DATA_TYPE::info_file:
      {
        if ( !n->is_info_file_loaded ) load_info_file(n);
        break;
      }
    case DATA_TYPE::query_data_structure:
      {
        if ( !n->is_query_data_structure_loaded ) load_query_data_structure(n);
        break;
      }
    case DATA_TYPE::all:
      {
        if ( !n->is_points_loaded ) load_points(n);
        if ( !n->is_info_file_loaded ) load_info_file(n);
        if ( !n->is_query_data_structure_loaded ) load_query_data_structure(n);
        break;
      }
    default:
      DEBUG_MSG_FAIL("Cannot load the given data type");
      break;

    };
  }

  void external_priority_search_tree::flush_data(node* n, DATA_TYPE dt) {
    if ( n->is_root() ) return;
    switch (dt) {
    case DATA_TYPE::points:
      {
        if ( n->is_points_loaded ) flush_points(n);
        break;
      }
    case DATA_TYPE::info_file:
      {
        if ( n->is_info_file_loaded ) flush_info_file(n);
        break;
      }
    case DATA_TYPE::query_data_structure:
      {
        if ( n->is_query_data_structure_loaded ) flush_query_data_structure(n);
        break;
      }
    case DATA_TYPE::all:
      {
        if ( n->is_points_loaded ) flush_points(n);
        if ( n->is_info_file_loaded ) flush_info_file(n);
        if ( n->is_query_data_structure_loaded ) flush_query_data_structure(n);
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
    util::load_file_to_container<points_type, point_type>
      (n->points, get_points_file_name(n->id), buffer_size);
    n->is_points_loaded = true;
  }

  void external_priority_search_tree::load_info_file(node* n) {
#ifdef DEBUG
    assert( !n->is_info_file_loaded );
#endif
    info_file_type temp;
    util::load_file_to_container<info_file_type, info_file_entry_type>
      (temp, get_info_file_file_name(n->id), buffer_size);
    n->b_is_leaf = (bool)temp[0];
    n->parent_id = temp[1];
    n->right_most_child = temp[2];
    n->is_info_file_loaded = true;
  }

  void external_priority_search_tree::load_query_data_structure(node *n) {
#ifdef DEBUG
    assert ( !n->is_query_data_structure_loaded );
#endif
    n->query_data_structure = new ext::child_structure(n->id);
    n->is_query_data_structure_loaded = true;
  }

  void external_priority_search_tree::flush_points(node* n) {
#ifdef DEBUG
    assert( n->is_points_loaded );
#endif
    util::flush_container_to_file<points_type::iterator, point_type>
      (n->points.begin(), n->points.end(), get_points_file_name(n->id), buffer_size);
    n->points.clear();
    n->is_points_loaded = false;
  }

  void external_priority_search_tree::flush_info_file(node* n) {
#ifdef DEBUG
    assert( n->is_info_file_loaded );
#endif
    info_file_type info_file;
    info_file.push_back((info_file_entry_type)n->b_is_leaf);
    info_file.push_back((info_file_entry_type)n->parent_id);
    info_file.push_back((info_file_entry_type)n->right_most_child);
    util::flush_container_to_file<info_file_type::iterator, info_file_entry_type>
      (info_file.begin(), info_file.end(), get_info_file_file_name(n->id), buffer_size);
    info_file.clear();
    n->is_info_file_loaded = false;
  }

  void external_priority_search_tree::flush_query_data_structure(node* n) {
#ifdef DEBUG
    assert ( n->is_query_data_structure_loaded );
#endif
    delete n->query_data_structure;
    n->query_data_structure = 0;
    n->is_query_data_structure_loaded = false;
  }

  std::string external_priority_search_tree::get_points_file_name(size_t id) {
    return get_directory(id) + "/points";
  }
  
  std::string external_priority_search_tree::get_info_file_file_name(size_t id) {
    return get_directory(id) + "/info_file";
  }

  std::string external_priority_search_tree::get_directory(size_t id) {
    return std::to_string(id);
  }

  external_priority_search_tree::node* external_priority_search_tree::allocate_node() {
    size_t id = next_id++;
    node* n = new node(id);
    n->is_points_loaded = true;
    n->is_info_file_loaded = true;
    n->b_is_leaf = true;
    n->parent_id = (size_t)-1;
    n->right_most_child = (size_t)-1;
    n->query_data_structure = new ext::child_structure(id, buffer_size, 1, std::vector<point>());
    n->is_query_data_structure_loaded = true;

    //create the necessary folder
    mkdir(get_directory(id).c_str(), 0700);
    DEBUG_MSG("Created a node with id: " << id);
    return n;
  }

  external_priority_search_tree::node* external_priority_search_tree::retrieve_node(size_t id) {
    if (id == 0) return root;
    node* n = new node(id);
    return n;
  }

  bool external_priority_search_tree::is_degree_overflow(node* n) {
#ifdef DEBUG
    assert( n->is_points_loaded );
#endif
    if ( n->is_leaf() ) return n->points.size() > leaf_parameter;
    else return n->points.size()+1 > branching_parameter;
  }

  /*
    METHODS OF THE NODE
  */

  external_priority_search_tree::node::node(size_t id) {
    this->id = id;
    this->is_points_loaded = false;
    this->is_info_file_loaded = false;
    this->b_is_leaf = false;
    this->parent_id = (size_t)1337;
    this->right_most_child = (size_t)1337;
    this->query_data_structure = 0;
    this->is_query_data_structure_loaded = false;
  }

  bool external_priority_search_tree::node::is_leaf() {
    return b_is_leaf;
  }

  bool external_priority_search_tree::node::is_root() {
    return id == 0;
  }
};
#endif
