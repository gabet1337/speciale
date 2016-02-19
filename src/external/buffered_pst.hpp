#ifndef BUFFERED_PST_HPP
#define BUFFERED_PST_HPP
#include "../common/point.hpp"
#include "../common/debug.hpp"
#include "child_structure.hpp"
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <iterator>

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
      buffered_pst_node(int id, size_t buffer_size);
      ~buffered_pst_node();
      void insert(const point &p);
      void remove(const point &p);
#ifdef DEBUG
      bool is_valid();
#endif
    private:
      bool insert_buffer_overflow();
      bool delete_buffer_overflow();
      bool point_buffer_overflow();
      bool point_buffer_underflow();
      void handle_insert_buffer_overflow();
      bool is_leaf();
      bool is_root();
      int id;
      size_t buffer_size;
      std::set<point> insert_buffer, delete_buffer, point_buffer;
      //ext::child_structure child_structure;
    };
    size_t buffer_size;
    int fanout;
    double epsilon;
    buffered_pst_node* root;
  };

  // BUFFERED_PST_NODE /////////////////////////////////////////////////////
  buffered_pst::buffered_pst_node::buffered_pst_node(int id, size_t _buffer_size)
    : buffer_size(_buffer_size)  {
    this->id = id;
    DEBUG_MSG("Constructed pst_node with id " << id << " and buffer_size: " << buffer_size);
  }

  buffered_pst::buffered_pst_node::~buffered_pst_node() {
  }

  void buffered_pst::buffered_pst_node::insert(const point &p) {
    DEBUG_MSG("Inserting point " << p << " into node " << id);

    if (is_root()) {
      /* Inserting into root
	 1.) Remove duplicates of p from Pr, Ir, Dr
	 2.) If p.y < smallest y in Pr then p is inserted in Ir or Dr, o.w. in Pr.
	 3.) If pr overflows then move smallest y-value point to Ir */
      DEBUG_MSG("remove duplicates of p from Pr, Ir, Dr");
      point_buffer.erase(p);
      insert_buffer.erase(p);
      delete_buffer.erase(p);
      DEBUG_MSG("Check if put into Ir or Pr");
      point min_y = *std::min_element(point_buffer.begin(), point_buffer.end(),
                                      [] (const point &p1, const point &p2) {
                                        return p1.y < p2.y;
                                      });
      if (p.y < min_y.y) insert_buffer.insert(p);
      else point_buffer.insert(p);
      if ( point_buffer_overflow() ) {
        DEBUG_MSG("Pr overflow: Moving point with smallest y-value from Pr to Ir");
        point_buffer.erase(min_y);
        insert_buffer.insert(min_y);
        //TODO: handle insert_buffer overflow here
        if ( insert_buffer_overflow() ) handle_insert_buffer_overflow();
      }
    }
  }

  bool buffered_pst::buffered_pst_node::point_buffer_overflow() {
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
    return true;
  }

  bool buffered_pst::buffered_pst_node::is_root() {
    return id == 0;
  }

  void buffered_pst::buffered_pst_node::handle_insert_buffer_overflow() {
    DEBUG_MSG("Starting to handle insert buffer overflow");
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
    root = new buffered_pst_node(0,buffer_size);
    DEBUG_MSG("Constructing buffered_pst with buffer_size: "
	      << buffer_size << " epsilon: " << epsilon << " fanout: " << fanout);
      
  }

  void buffered_pst::insert(const point &p) {
    DEBUG_MSG("Inserting point " << p << " into root");
    root->insert(p);
  }
  
  buffered_pst::~buffered_pst() {  }

#ifdef DEBUG
  bool buffered_pst::is_valid() {
    return root->is_valid();
  }
#endif
  
};

#endif
