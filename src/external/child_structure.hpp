#ifndef CHILD_STRUCTURE_HPP
#define CHILD_STRUCTURE_HPP

#include "../common/debug.hpp"
#include "../common/point.hpp"
#include "../common/utilities.hpp"
#include "../stream/stream.hpp"
#include "../internal/rb_tree.hpp"
#include "child_structure_interface.hpp"
#include <string>
#include <error.h>
#include <vector>
#include <set>
#include <queue>
#include <bitset>
#include <assert.h>

#define INF 1000000000
namespace ext {

  class child_structure : public child_structure_interface {
  public:
    child_structure(size_t id, size_t buffer_size,
		    double epsilon, std::vector<point> points);
    child_structure(size_t id);
    ~child_structure();
    void insert(const point &p);
    void remove(const point &p);
    std::vector<point> report(int x1, int x2, int y);
    void destroy();
#ifdef DEBUG
    std::vector<point> get_points()
    {
      if (!L_in_memory) {
        DEBUG_MSG("Loading L");
        util::load_file_to_container<std::vector<point>, point>(L, get_L_file(), buffer_size);
      }
      std::vector<point> L_tmp,L_new;
      std::set_union(L.begin(),L.begin()+L_size,
                     I.begin(),I.end(),
                     std::back_inserter(L_tmp));

      std::set_difference(L_tmp.begin(), L_tmp.end(),
                          D.begin(), D.end(),
                          std::back_inserter(L_new));
      //L_new should now be equivalent to L' in article
      return L_new;
    }
#endif
#ifdef VALIDATE
    bool valid_disk();
    bool valid_memory();
#endif
  private:
    struct catalog_item {
      int min_x, max_x, min_y, i, j, start_idx, end_idx;
      catalog_item() {}
      catalog_item(int _min_x, int _max_x, int _min_y, int _i, int _j,
		   int _start_idx, int _end_idx) :
        min_x(_min_x), max_x(_max_x), min_y(_min_y), i(_i), j(_j),
	start_idx(_start_idx), end_idx(_end_idx) {}

      friend std::ostream& operator<<(std::ostream& os, const catalog_item& ci) {
	os << "(min_x:" << ci.min_x << ", max_x:" << ci.max_x << ", min_y:" << ci.min_y <<
	  ", i:" << ci.i << ", j:" << ci.j << " start_idx: " << ci.start_idx <<
	  " end_idx: " << ci.end_idx << ")";
	return os;
      }
    };
    std::string get_info_file();
    std::string get_directory();
    std::string get_L_file();
    std::string get_I_file();
    std::string get_D_file();
    std::string get_catalog_file();
    void construct(std::vector<point> points);
    void rebuild();
    inline bool in_range(const point &p, int x1, int x2, int y);
    inline bool in_range(const catalog_item &ci, int x1, int x2, int y);
    inline bool above_sweep_line(const point &p, const point &sweep);
    std::vector<point> L;
    std::set<point> I, D;
    std::vector<catalog_item> catalog;
    size_t id, buffer_size, L_buffer_size, epsilon, L_size, I_size, D_size;
    bool L_in_memory;
    bool should_delete_structure;
    const int NUM_VARIABLES = 6;
  };

  child_structure::child_structure(size_t id) {
    this->id = id;
    this->L_in_memory = false;
    DEBUG_MSG("Load variables into structure");
    io::buffered_stream<size_t> info_file(NUM_VARIABLES);
    info_file.open(get_info_file());
    this->id = info_file.read();
    this->buffer_size = info_file.read();
    this->epsilon = info_file.read();
    this->L_size = info_file.read();
    // this->I_size = info_file.read();
    // this->D_size = info_file.read();
    info_file.close();

    DEBUG_MSG("Load data into structure");
    //DEBUG_MSG("Loading L");
    //load_file_to_container<std::vector<point>, point>(L, get_L_file());
    DEBUG_MSG("Loading I");
    util::load_file_to_container<std::set<point>, point>(I, get_I_file(),buffer_size);
    I_size = I.size();
    DEBUG_MSG("Loading D");
    util::load_file_to_container<std::set<point>, point>(D, get_D_file(),buffer_size);
    D_size = D.size();
    DEBUG_MSG("Loading Catalog");
    util::load_file_to_container<std::vector<child_structure::catalog_item>,child_structure::catalog_item>(catalog, get_catalog_file(), buffer_size);
    DEBUG_MSG("Finished loading");
    DEBUG_MSG(" - L.size(): " << L.size() << ", L_size: " << L_size);
    DEBUG_MSG(" - I.size(): " << I.size());
    DEBUG_MSG(" - D.size(): " << D.size());
    DEBUG_MSG(" - Catalog.size(): " << catalog.size());
    should_delete_structure = false;
  }

  child_structure::child_structure(size_t id, size_t buffer_size,
				   double epsilon, std::vector<point> points) {

    this->id = id;
    this->L_in_memory = true;
    //TODO use exceptions
    if (util::file_exists(get_info_file()))
      error(1, EEXIST, "child structure already exists");
    
    this->buffer_size = buffer_size;
    this->epsilon = (size_t)(1000.0*epsilon);
    this->L_buffer_size = (size_t)pow((double)buffer_size, 1.0+this->epsilon/1000.0);
    DEBUG_MSG("constructing child structure " << id <<
              " with B=" << buffer_size << " - e=" << epsilon << " - L_size=" << L_buffer_size);
    
    I_size = 0;
    D_size = 0;
    should_delete_structure = false;
    construct(points);
  }

  child_structure::~child_structure() {
    DEBUG_MSG("destructing child structure " << id);
    DEBUG_MSG("flushing variables to " << get_info_file());

    //check if directory exists and open:
    if (!util::file_exists(get_directory())) mkdir(get_directory().c_str(), 0700);
    
    if (should_delete_structure) {
      DEBUG_MSG_FAIL("DELETING " << get_directory());
      util::remove_directory(get_directory());
      return;
    }

    io::buffered_stream<size_t> info_file(NUM_VARIABLES);
    info_file.open(get_info_file());
    info_file.write(id);
    info_file.write(buffer_size);
    info_file.write(epsilon);
    info_file.write(L_size);
    info_file.write(I.size());
    info_file.write(D.size());
    
    info_file.close();

    if (L_in_memory) {
      DEBUG_MSG("Flushing L");
      util::flush_container_to_file<std::vector<point>::iterator,point>(L.begin(), L.end(),
									get_L_file(),buffer_size);
      L.clear();
      L.shrink_to_fit();
    }

    DEBUG_MSG("Flushing I");
    util::flush_container_to_file<std::set<point>::iterator,point>(I.begin(), I.end(), get_I_file(), buffer_size);
    I.clear();

    DEBUG_MSG("Flushing D");
    util::flush_container_to_file<std::set<point>::iterator,point>(D.begin(), D.end(), get_D_file(), buffer_size);
    D.clear();

    DEBUG_MSG("Flushing Catalog");
    util::flush_container_to_file<std::vector<child_structure::catalog_item>::iterator,
				  child_structure::catalog_item>(catalog.begin(), catalog.end(), get_catalog_file(), buffer_size);
    catalog.clear();
    catalog.shrink_to_fit();
  }

  void child_structure::construct(std::vector<point> points) {
    L_in_memory = true;
    L = points;
    L_size = points.size();
#ifdef DEBUG
    assert(std::is_sorted(L.begin(), L.end()));
#endif
    catalog.clear();

    // sweep and construct rest of L
    typedef std::pair<point,int> point_block;
    auto comp = [](point_block p1, point_block p2) {
      return p1.first.y > p2.first.y || (p1.first.y == p2.first.y && p1.first.x > p2.first.x);
    };
    std::priority_queue<point_block, std::vector<point_block>,
                        decltype(comp)> pq(comp);

    struct block {
      int id,size,right;
      block(int _id, int _size, int _right) : id(_id), size(_size), right(_right) {}
      bool operator<(const block b) const { return id < b.id; }
      bool operator==(const block b) const { return id == b.id; }
    };
    
    internal::rb_tree<block> intervals;

    intervals.insert(block(-1000000,-1000000,-100000));
    intervals.insert(block(1000000,-1000000,-100000));

    std::vector<std::vector<point> > points_in_blocks(points.size()/buffer_size+1, std::vector<point>());
    int min_x = INF, max_x = -INF, min_y = INF;
    for (int i = 0; i < (int)points.size(); i++) {
      min_x = std::min(min_x, points[i].x);
      max_x = std::max(max_x, points[i].x);
      min_y = std::min(min_y, points[i].y);
      points_in_blocks[i/buffer_size].push_back(points[i]);
      DEBUG_MSG("point " << points[i] << " went into " << i/buffer_size);
      pq.push( point_block(points[i], i/buffer_size));
      if ((i+1)%buffer_size == 0) {
        DEBUG_MSG("Writing to catalog");
        DEBUG_MSG(min_x << " " << max_x << " " << min_y << " " << (i+1)/buffer_size-1 << " " << (i+1)/buffer_size-1);
        catalog.insert(catalog.end(), catalog_item(min_x, max_x, min_y, (i+1)/buffer_size-1, (i+1)/buffer_size-1,i+1-buffer_size,i+1));
        min_x = INF, max_x = -INF, min_y = INF;
        intervals.insert( block((i+1)/buffer_size-1,buffer_size,(i+1)/buffer_size-1));
        DEBUG_MSG("new block: " << (i+1)/buffer_size-1 << ", " << buffer_size);
      }
    }
    int remaining = points.size()%buffer_size;
    if (remaining) {
      intervals.insert(block(remaining==0 ? points.size()/buffer_size-1 : points.size()/buffer_size
			     ,remaining==0 ? buffer_size: remaining,
			     remaining==0 ? points.size()/buffer_size-1 : points.size()/buffer_size));
      DEBUG_MSG("remaining block " << (remaining==0 ? points.size()/buffer_size-1 : points.size()/buffer_size) << " with points " << (remaining==0 ? buffer_size: remaining));

      DEBUG_MSG("writing remaining block to catalog");
      DEBUG_MSG(min_x << " " << max_x << " " << min_y << " " << (remaining==0 ? points.size()/buffer_size-1 : points.size()/buffer_size) << " " << (remaining==0 ? points.size()/buffer_size-1 : points.size()/buffer_size));
      catalog.insert(catalog.end(), catalog_item(min_x, max_x, min_y, remaining==0 ? points.size()/buffer_size-1 : points.size()/buffer_size, remaining==0 ? points.size()/buffer_size-1 : points.size()/buffer_size,L_size-remaining,L_size));
    }

    std::vector<point> out;
    
    while (!pq.empty()) {
      bool should_update_catalog = false;
      point_block pb = pq.top(); pq.pop();

      block pb_belong_to = intervals.belong_to(block(pb.second,0,0));
      DEBUG_MSG(pb.first << " with bid " << pb.second << " belong to " << pb_belong_to.id);
      int our_size = pb_belong_to.size;
      block pred = intervals.predecessor(pb_belong_to);
      block succ = intervals.successor(pb_belong_to);
      DEBUG_MSG("osid " << pb_belong_to.id << ": " << our_size << std::endl
                << "psid " << pred.id << ": " << pred.size << std::endl
                << "ssid " << succ.id << ": " << succ.size);

      if (our_size + pred.size + succ.size == (int)buffer_size) {
        DEBUG_MSG("collapsing with left AND right neighbour");
#ifdef DEBUG
        assert(pred.size == 0 && succ.size == 0 && "error on collapsing with both neighbours");
#endif
        DEBUG_MSG("constructing [" << pred.id << ", " << succ.right << "]");
        for (point p : points_in_blocks[pb_belong_to.id]) {
#ifdef DEBUG
          assert(p.y >= pb.first.y);
#endif
	  out.push_back(p);
          L.push_back(p);
        }
        points_in_blocks[pred.id] = out;
        intervals.erase(succ);
        intervals.erase(pb_belong_to);
        intervals.erase(pred);
        intervals.insert(block(pred.id, pb_belong_to.size-1, succ.right));
	should_update_catalog = true;
      } else if (our_size + pred.size == (int)buffer_size) {
        DEBUG_MSG("collapsing with left neighbour");
        DEBUG_MSG("constructing [" << pred.id << ", " << pb_belong_to.right << "]");
        //int limit = pb.first.y;
        for (point p : points_in_blocks[pb_belong_to.id])
	  if (above_sweep_line(p,pb.first)) {
            DEBUG_MSG("above: " << p);
            out.push_back(p);
          }
#ifdef DEBUG
          else DEBUG_MSG("below: " << p);
#endif
        for (point p : points_in_blocks[pred.id])
	  if (above_sweep_line(p,pb.first)) {
            DEBUG_MSG("above: " << p);
            out.push_back(p);
          }
#ifdef DEBUG
          else DEBUG_MSG("below: " << p);
#endif
        points_in_blocks[pred.id] = out;
        for (point p : out) L.push_back(p);
        intervals.erase(pb_belong_to);
        intervals.erase(pred);
        intervals.insert(block(pred.id,out.size()-1,pb_belong_to.right));
	should_update_catalog = true;
      } else if (our_size + succ.size == (int)buffer_size) {
        DEBUG_MSG("collapsing with right neighbour");
        DEBUG_MSG("constructing [" << pb_belong_to.id << ", " << succ.right << "]");
	//int limit = pb.first.y;
        for (point p : points_in_blocks[pb_belong_to.id])
	  if (above_sweep_line(p,pb.first)) {
            DEBUG_MSG("above: " << p);
            out.push_back(p);
          }
#ifdef DEBUG
          else DEBUG_MSG("below: " << p);
#endif
        for (point p : points_in_blocks[succ.id])
	  if (above_sweep_line(p,pb.first)) {
            DEBUG_MSG("above: " << p);
            out.push_back(p);
          }
#ifdef DEBUG
          else DEBUG_MSG("below: " << p);
#endif
        points_in_blocks[pb_belong_to.id] = out;
        for (point p : out) L.push_back(p);
        intervals.erase(succ);
        intervals.erase(pb_belong_to);
        intervals.insert(block(pb_belong_to.id, out.size()-1, succ.right));
	should_update_catalog = true;
      } else {
        DEBUG_MSG("removing element: " << pb.first << " from " << pb_belong_to.id << " and down to " << pb_belong_to.size-1);
        intervals.erase(pb_belong_to);
        intervals.insert(block(pb_belong_to.id, pb_belong_to.size-1,pb_belong_to.right));
      }

      if (should_update_catalog) {
	DEBUG_MSG("Updating catalog with collapsed entry");
	DEBUG_MSG("- with " << out.size() << " points");
	for (point p : out)
	  DEBUG_MSG(" - " << p);
	pb_belong_to = intervals.belong_to(block(pb.second,0,0));
	min_x = std::min_element(out.begin(),out.end())->x;
	max_x = std::max_element(out.begin(),out.end())->x;
	DEBUG_MSG(min_x << " " << max_x << " " << pb.first.y << " " << pb_belong_to.id << " " << pb_belong_to.right);
	catalog.push_back(catalog_item(min_x, max_x, pb.first.y, pb_belong_to.id, pb_belong_to.right,L.size()-buffer_size, L.size()));
	out.clear();
	}
    }

  }

  inline bool child_structure::above_sweep_line(const point &p, const point &sweep) {
    if (p.y == sweep.y) return p.x >= sweep.x;
    return p.y > sweep.y;
  }
  void child_structure::insert(const point &p) {
    DEBUG_MSG("Insert point " << p);

    if (I.size() >= buffer_size) rebuild();

    DEBUG_MSG(" - deleting from I");
    auto it = I.find(p);
    if (it != I.end()) I.erase(it);

    DEBUG_MSG(" - deleting from D");
    it = D.find(p);
    if (it != D.end()) D.erase(it);

    I.insert(p);
  }

  void child_structure::remove(const point &p) {
    DEBUG_MSG("Remove point " << p);

    if (D.size() >= buffer_size) rebuild();
    
    DEBUG_MSG(" - deleting from I");
    auto it = I.find(p);
    if (it != I.end()) I.erase(it);

    DEBUG_MSG(" - deleting from D");
    it = D.find(p);
    if (it != D.end()) D.erase(it);

    D.insert(p);
  }

  inline bool child_structure::in_range(const point &p, int x1, int x2, int y) {
    return x1 <= p.x && p.x <= x2 && p.y >= y;
  }

  inline bool child_structure::in_range(const catalog_item &ci, int x1, int x2, int y) {
    return (ci.i == ci.j || ci.min_y < y) &&
      ((x1 <= ci.min_x && x2 >= ci.max_x) || // interval spans block completely
       (ci.min_x <= x1 && x2 <= ci.max_x) || // block spans interval completely
       (ci.min_x <= x1 && x1 <= ci.max_x && ci.max_x <= x2) || // interval starts inside block
       (ci.min_x <= x2 && x2 <= ci.max_x && x1 <= ci.min_x)); // interval ends inside block
  }
      
  std::vector<point> child_structure::report(int x1, int x2, int y) {
    DEBUG_MSG("Reporting points in [" << x1 << ", " << x2 << "] X [" <<
              y << ", \u221E]");
    std::set<point> result;

    if (x2 < x1) return std::vector<point>();
    
    DEBUG_MSG(" - reporting from I");
    for (point p : I) {
      if (in_range(p, x1, x2, y)) result.insert(p);
      DEBUG_MSG(" - added point " << p);
    }

    DEBUG_MSG(" - reporting from L");
    std::vector<bool> marked(L_size/buffer_size+1,false);

    io::buffered_stream<point>* L_file = 0;
    if (!L_in_memory) {
      L_file = new io::buffered_stream<point>(buffer_size);
      L_file->open(get_L_file());
    }
    
    for (int i = (int)catalog.size()-1; i >= 0; i--) {
      catalog_item ci = catalog[i];
      if (in_range(ci,x1,x2,y) && !marked[ci.i]) {
	for (int j = ci.i; j <= ci.j; j++) marked[j] = true;
	// Query block
	if (L_in_memory) {
	  for (int j = ci.start_idx; j < ci.end_idx; j++) {
	    if (in_range(L[j],x1,x2,y)) {
	      result.insert(L[j]);
	      DEBUG_MSG(" - added point " << L[j]);
	    } else DEBUG_MSG(" - rejected this bitch: point " << L[j]);
	  }
	} else {
	  DEBUG_MSG("L not in memory: Reading from file");
	  L_file->seek((off_t)(ci.start_idx*sizeof(point)),SEEK_SET);
	  for (int j=ci.start_idx; j < ci.end_idx; j++) {
	    point p = L_file->read();
	    if (in_range(p,x1,x2,y)) {
	      result.insert(p);
	      DEBUG_MSG(" - added point " << p);
	    } else DEBUG_MSG(" - rejected this bitch: point " << p);
	  }
	}
      }
    }

    if (!L_in_memory) {
      L_file->close();
      delete L_file;
    }
    
    DEBUG_MSG(" - removing D-points");
    std::vector<point> final_result;
    std::set_difference(result.begin(),result.end(),
			D.begin(),D.end(),
			std::back_inserter(final_result));
    
    return final_result;
  }

  void child_structure::rebuild() {

    if (!L_in_memory) {
      DEBUG_MSG("Loading L");
      util::load_file_to_container<std::vector<point>, point>(L, get_L_file(), buffer_size);
    }
    
    DEBUG_MSG("STARTING REBUILDING");
    //maintain size of L, i.e. L_size

    // Take the points of I and D and merge them into the first L_size points of L
    // points in I and D are sorted as they are in a set
    std::vector<point> L_tmp,L_new;
    std::set_union(L.begin(),L.begin()+L_size,
                   I.begin(),I.end(),
                   std::back_inserter(L_tmp));

    std::set_difference(L_tmp.begin(), L_tmp.end(),
                        D.begin(), D.end(),
                        std::back_inserter(L_new));
    //L_new should now be equivalent to L' in article
    I.clear();
    D.clear();
    construct(L_new);
  }

  void child_structure::destroy() {
    should_delete_structure = true;
  }
  
  std::string child_structure::get_directory() {
    return std::string("c_").append(std::to_string(id));
  }

  std::string child_structure::get_info_file() {
      return get_directory()+std::string("/info");
  }

  std::string child_structure::get_L_file() {
    return get_directory()+std::string("/L");
  }

  std::string child_structure::get_I_file() {
    return get_directory()+std::string("/I");
  }

  std::string child_structure::get_D_file() {
    return get_directory()+std::string("/D");
  }

  std::string child_structure::get_catalog_file() {
    return get_directory()+std::string("/catalog");
  }
  
#ifdef VALIDATE
  bool child_structure::valid_disk() {
    io::buffered_stream<size_t> info_file(NUM_VARIABLES);
    info_file.open(get_info_file());
    if (NUM_VARIABLES*sizeof(size_t) != info_file.size()) {
      DEBUG_MSG("NUM_VARIABLES != info file size " << NUM_VARIABLES*sizeof(size_t) << " != " << info_file.size());
      return false;
    }
    info_file.close();
    return true;
  }

  bool child_structure::valid_memory() {
    DEBUG_MSG("VALIDATING MEMORY");
    if (L_in_memory) {
      if (!std::is_sorted(L.begin(), L.begin()+L_size)) {
	DEBUG_MSG("L is not sorted w.r.t x for the first L_size " << L_size << " points");
	for (point p : L)
	  DEBUG_MSG(" - " << p);
	return false;
      }
      auto comp = [](point p1, point p2) {
	return p1.y < p2.y || (p1.y == p2.y && p1.x < p2.x);
      };
      for (int i = L_size; i <= (int)L.size()-2*(int)buffer_size; i+=(int)buffer_size) {
	int b_min = std::min_element(L.begin()+i, L.begin()+(i+buffer_size),comp)->y;
	int b1_min = std::min_element(L.begin()+(i+buffer_size), L.begin()+(i+2*buffer_size),comp)->y;
	DEBUG_MSG("bi_min: " << b_min << std::endl << "bi+1_min: " << b1_min);
	if (b_min > b1_min) {
	  DEBUG_MSG("Blocks in L are not increasing in y-value for sweeped points");
	  return false;
	}
      }

      for (size_t i = 0; i < (L_size%buffer_size==0 ? L_size/buffer_size : L_size/buffer_size+1); i++) {
	int min_x = catalog[i].min_x;
	int max_x = catalog[i].max_x;
	int min_y = catalog[i].min_y;
	int start_idx = catalog[i].start_idx;
	int end_idx = catalog[i].end_idx;

	if (start_idx > end_idx) {
	  DEBUG_MSG("start_idx > end_idx in non-collapsed blocks");
	  return false;
	}
	if ((int)(i*buffer_size) != start_idx) {
	  DEBUG_MSG("start_idx " << start_idx << " not correct, expected " << i*buffer_size);
	  return false;
	}
	if (min_x > max_x) { DEBUG_MSG("min x > max_x"); return false;}
	for (size_t j = i*buffer_size; j < std::min(i*buffer_size+buffer_size,L_size); j++) {
	  DEBUG_MSG("Getting point on index " << j << " " << L[j] << " from block " << i);
	  if (L[j].x < min_x || max_x < L[j].x) {
	    DEBUG_MSG("point " << L[j] << " not between " << min_x << " and " << max_x << " in block " << i); return false;}
	  if (L[j].y < min_y) { DEBUG_MSG("point " << L[j] << " has y less than min_y for block " << i); return false;}
	}
      }

      for (size_t i = L_size%buffer_size==0 ? L_size/buffer_size : L_size/buffer_size+1; i < catalog.size(); i++) {
	int min_x = catalog[i].min_x;
	int max_x = catalog[i].max_x;
	int min_y = catalog[i].min_y;
	int start_idx = catalog[i].start_idx;
	int end_idx = catalog[i].end_idx;
	
	if (start_idx > end_idx) {
	  DEBUG_MSG("start_idx " << start_idx << " > end_idx " << end_idx);
	  return false;
	}

	if (i*buffer_size-((L_size%buffer_size==0 ? L_size/buffer_size : L_size/buffer_size+1)
			   *buffer_size-L_size) != (size_t)start_idx) {
	  DEBUG_MSG("start_idx " << start_idx << " not correct, expected " << i*buffer_size);
	  return false;
	}
	
	if (min_x > max_x) { DEBUG_MSG("sweepline: min x " << min_x << " > max_x" << max_x << " in block " << i); return false;}
	
	for (size_t j = i*buffer_size-((L_size%buffer_size==0 ? L_size/buffer_size : L_size/buffer_size+1)*buffer_size-L_size); j < (i*buffer_size+buffer_size)-((L_size%buffer_size==0 ? L_size/buffer_size : L_size/buffer_size+1)*buffer_size-L_size); j++) {
	  DEBUG_MSG("Getting point on index " << j << " " << L[j] << " from block " << i);
	  if (L[j].x < min_x || max_x < L[j].x) {
	    DEBUG_MSG("sweepline: point " << L[j] << " not between " << min_x << " and " << max_x << " in block " << i); return false;}
	  if (L[j].y < min_y) { DEBUG_MSG("point " << L[j] << " has y less than min_y for block " << i); return false;}
	}
	int min_y_i = std::min_element(L.begin()+i*buffer_size,L.begin()+i*buffer_size+buffer_size, comp)->y;
	if (i*buffer_size+2*buffer_size < L.size() &&
	    std::min_element(L.begin()+i*buffer_size+buffer_size, L.begin()+i*buffer_size+2*buffer_size, comp)->y < min_y_i) {
	  DEBUG_MSG("min_y " << min_y << " in i+1 must be greater than or equal to min_y " << min_y_i);
	  return false;	
	}
	
      }

    }

    for (catalog_item c : catalog) {
      int count = 0;
      for (catalog_item c2 : catalog) {
	if (c.i == c2.i && c.j == c2.j)
	  count++;
      }
      if (count > 1) {
	DEBUG_MSG("Catalog interval is not unique");
	return false;
      }
    }

    if (catalog.size() < L_size/buffer_size) {
      DEBUG_MSG("Catalog is invalid");
      return false;
    }

    return true;
  }
#endif

};

#endif
