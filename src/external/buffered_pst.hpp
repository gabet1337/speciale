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

#define INF 1000000000
namespace ext {
  int next_id = 1;
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
      buffered_pst_node(int id, int parent_id, size_t buffer_size,
			double epsilon, buffered_pst_node* root);
      buffered_pst_node(int id, buffered_pst_node* root);
      ~buffered_pst_node();
      void remove(const point &p);
      void add_child(const range &r);
      void insert_into_point_buffer(const point &p);
      template <class Container>
      void insert_into_point_buffer(const Container &points);
      void insert_into_insert_buffer(const point &p);
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
      void handle_insert_buffer_overflow();
      void handle_point_buffer_overflow();
      std::string get_point_buffer_file_name(int id);
      std::string get_insert_buffer_file_name(int id);
      std::string get_delete_buffer_file_name(int id);
      std::string get_info_file_file_name(int id);
      std::string get_directory(int id);
      bool b_is_leaf;
      int id, parent_id;
      size_t buffer_size;
      size_t B_epsilon;
      double epsilon;
      buffered_pst_node* root;
      //ext::child_structure child_structure;
    };
    size_t buffer_size;

    int fanout;
    double epsilon;
    buffered_pst_node* root;
  };

  // BUFFERED_PST_NODE /////////////////////////////////////////////////////
  buffered_pst::buffered_pst_node::buffered_pst_node(int id, int parent_id, size_t _buffer_size, double epsilon, buffered_pst_node* root) : buffer_size(_buffer_size)  {
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
    B_epsilon = (size_t)pow((double)buffer_size, epsilon);
    b_is_leaf = true;
    DEBUG_MSG("Constructed pst_node with id " << id << " and buffer_size: " << buffer_size
              << " and B_epsilon " << B_epsilon << " and parent_id " << parent_id);
  }

  buffered_pst::buffered_pst_node::buffered_pst_node(int id, buffered_pst_node* root) {
    
    this->id = id;
    this->root = root;
    DEBUG_MSG("Constructing node " << id << " from files");

    DEBUG_MSG("Loading info file");
    std::vector<size_t> info_file;
    util::load_file_to_container<std::vector<size_t>, size_t>(info_file, get_info_file_file_name(id), 512);
    this->buffer_size = info_file[0];
    this->B_epsilon = info_file[1];
    this->b_is_leaf = (bool)info_file[2];
    this->parent_id = (int)info_file[3];
    DEBUG_MSG("Loading point buffer");
    util::load_file_to_container<std::set<point>, point>(point_buffer, get_point_buffer_file_name(id), buffer_size);
    DEBUG_MSG("Loading insert buffer");
    util::load_file_to_container<std::set<point>, point>(insert_buffer, get_insert_buffer_file_name(id), buffer_size);
    DEBUG_MSG("Loading delete buffer");
    util::load_file_to_container<std::set<point>, point>(delete_buffer, get_delete_buffer_file_name(id), buffer_size);
  }
  
  buffered_pst::buffered_pst_node::~buffered_pst_node() {

    if (!util::file_exists(get_directory(id))) mkdir(get_directory(id).c_str(), 0700);
    
    DEBUG_MSG("Flushing point buffer");
    util::flush_container_to_file<std::set<point>::iterator,point>
      (point_buffer.begin(),point_buffer.end(), get_point_buffer_file_name(id), buffer_size);

    DEBUG_MSG("Flushing insert buffer");
    util::flush_container_to_file<std::set<point>::iterator,point>
      (insert_buffer.begin(),insert_buffer.end(), get_insert_buffer_file_name(id), buffer_size);

    DEBUG_MSG("Flushing delete buffer");
    util::flush_container_to_file<std::set<point>::iterator,point>
      (delete_buffer.begin(),delete_buffer.end(), get_delete_buffer_file_name(id), buffer_size);

    DEBUG_MSG("Flushing info file");
    std::vector<size_t> info_file;
    info_file.push_back(buffer_size);
    info_file.push_back(B_epsilon);
    info_file.push_back((size_t)b_is_leaf);
    info_file.push_back((size_t)parent_id);
    
    util::flush_container_to_file<std::vector<size_t>::iterator, size_t>
      (info_file.begin(), info_file.end(), get_info_file_file_name(id), buffer_size);
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

  std::string buffered_pst::buffered_pst_node::get_directory(int id) {
    return std::to_string(id);
  }

  void buffered_pst::buffered_pst_node::insert_into_insert_buffer(const point &p) {
    std::vector<point> points;
    points.push_back(p);
    insert_into_insert_buffer(points);
  }

  template <class Container>
  void buffered_pst::buffered_pst_node::insert_into_insert_buffer(const Container &points) {
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
    DEBUG_MSG("inserting point(s) into point buffer: ");
#ifdef DEBUG
    for (point p : points) DEBUG_MSG(" - " << p);
#endif
    
    point_buffer.insert(points.begin(), points.end());
    if (point_buffer_overflow()) handle_point_buffer_overflow();
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
      //TODO call get_id instead
      int child1_id = next_id++;
      int child2_id = next_id++;
      buffered_pst_node c1(child1_id, id, buffer_size, epsilon, root);
      buffered_pst_node c2(child2_id, id, buffer_size, epsilon, root);
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
                                         return p1.y < p2.y;
                                       })->y, child1_id));

      add_child(range(*c2.point_buffer.begin(),
                      std::max_element(c2.point_buffer.begin(),
                                       c2.point_buffer.end(),
                                       [](const point &p1, const point &p2) {
                                         return p1.y < p2.y;
                                       })->y, child2_id));

      DEBUG_MSG("Rebuild child structure of node " << id);
      

#ifdef DEBUG
      DEBUG_MSG("Points in point_buffer");
      for (auto p : point_buffer)
	DEBUG_MSG(p);
#endif

      b_is_leaf = false;
    } else if ( is_root() ) {
      DEBUG_MSG("Overflow in the point buffer of the root. Move min point to insert buffer");
      point min_y = *std::min_element(point_buffer.begin(), point_buffer.end(),
				      [] (const point &p1, const point &p2) {
					return p1.y < p2.y;
				      });
      point_buffer.erase(min_y);
      insert_into_insert_buffer(min_y);
    } else if ( is_leaf() ) {
      DEBUG_MSG("Overflow in the point buffer of node " << id);

      /*
	1.) Move last b/4 buffer_point points to new leaf.
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

      buffered_pst_node parent = parent_id == 0 ? *root : buffered_pst_node(parent_id, root);
      
      range r = parent.ranges.belong_to(range(*(point_buffer.begin()),-1,-1));
      if (r.max_y < keep_max_y) {
	DEBUG_MSG("Existing interval has changed max_y from " << r.max_y << " to "
		  << keep_max_y);
	parent.ranges.erase(r);
	parent.ranges.insert(range(r.min,keep_max_y,r.node_id));
      }

      int new_node_id = next_id++;
      DEBUG_MSG("Insert interval for new leaf " << move_min << " "
		<< move_max_y << " " << new_node_id);
      buffered_pst_node new_child(new_node_id,parent_id,buffer_size,epsilon,root);
      parent.ranges.insert(range(move_min,move_max_y,new_node_id));

      new_child.insert_into_point_buffer(move_points);
      
    } else {
      error(1, ECANCELED, "We should never overflow a point buffer in an internal node");
    }
  }

  void buffered_pst::buffered_pst_node::add_child(const range &r) {
    DEBUG_MSG("Adding range " << r << " to node " << id);
    ranges.insert(r);
  }

  std::pair<int, std::set<point> > buffered_pst::buffered_pst_node::find_child(const std::set<point> &buffer) {
    DEBUG_MSG("Starting to find child");

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
          cur_range = blt;
          tmp.clear();
        }
      }
      tmp.insert(p);
    }
    if (tmp.size() > best.size()) best = tmp;

    return {best_range.node_id, best};
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
    auto child_info = find_child(insert_buffer);
    buffered_pst_node found_child(child_info.first,root);

    DEBUG_MSG("Create U to send to child");
    std::set<point> U;
    range blt = ranges.belong_to(range(*(child_info.second.begin()),-1,-1));
    size_t idx = 0;
    int max_y = blt.max_y;
    point min = blt.min;
    for (auto p : child_info.second) {
      if (++idx > buffer_size/B_epsilon) break;
      min = std::min(min,p);
      max_y = std::max(max_y, p.y);
      DEBUG_MSG("point " << p << " is in U");
      U.insert(p);
    }

    if (*(U.begin()) < blt.min || max_y > blt.max_y) {
      DEBUG_MSG("Updating range " << min << " " << max_y << " for node_id: " << blt.node_id);
      ranges.erase(blt);
      ranges.insert(range(min,max_y,blt.node_id));
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
      DEBUG_MSG("found child was a leaf... sending U do Pc");
      found_child.insert_into_point_buffer(U);
      
    } else {
        
    }
  }

#ifdef DEBUG
  bool buffered_pst::buffered_pst_node::is_valid() {

    DEBUG_MSG("Starting is_valid()");
    
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

    for (auto it = ranges.begin(); it != ranges.end(); it++) {
      io::buffered_stream<point> bs(4096);
      bs.open(get_point_buffer_file_name(it->node_id));
      while (!bs.eof()) {
        point p = bs.read();
        if (ranges.belong_to(range(p,0,0)) != *it) {
          DEBUG_MSG("point " << p << " is in the wrong child " << it->node_id);
          return false;
        }
	if (p.y > it->max_y) {
	  DEBUG_MSG("point " << p << " has y-value larger than range max_y " << it->max_y);
	  return false;
	}
      }
      bs.close();
    }

    for (auto r : ranges) {
      DEBUG_MSG("Opening child " << r.node_id);
      buffered_pst_node child(r.node_id,root);
      if (child.is_valid() == false) {
	return false;
      }
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
    root = new buffered_pst_node(0,-1,buffer_size,epsilon,0);
    DEBUG_MSG("Constructing buffered_pst with buffer_size: "
	      << buffer_size << " epsilon: " << epsilon << " fanout: " << fanout);
    next_id = 1;
  }

  void buffered_pst::insert(const point &p) {
    DEBUG_MSG("Inserting point " << p << " into root");
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
					return p1.y < p2.y;
				      });
      if (p.y < min_y.y) root->insert_into_insert_buffer(p);
      else root->insert_into_point_buffer(p);
    }
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
