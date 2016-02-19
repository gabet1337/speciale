#ifndef BUFFERED_PST_HPP
#define BUFFERED_PST_HPP
#include "../common/point.hpp"
#include "../common/debug.hpp"
#include "child_structure.hpp"
#include <vector>
#include <string>
#include <cmath>

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

    if (id == 0) {
      /* Inserting into root
	 1.) Remove duplicates of p from Pr, Ir, Dr
	 2.) If p.y < smallest y in Pr then p is inserted in Ir or Dr, o.w. in Pr.
	 3.) Of pr overflows then move smallest y-value point to Ir */
      
    }
  }

#ifdef DEBUG
  bool buffered_pst::buffered_pst_node::is_valid() {
    // TODO check all invariants. Recurse on children.
    if (insert_buffer.size() > buffer_size) {
      DEBUG_MSG("Insert buffer overflow in node " << id);
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
