#ifndef CHILD_STRUCTURE_HPP
#define CHILD_STRUCTURE_HPP

#include "../common/debug.hpp"
#include "../common/point.hpp"
#include "../stream/stream.hpp"
#include "../internal/rb_tree.hpp"
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <error.h>
#include <vector>
#include <set>
#include <queue>
#include <assert.h>
#define INF 1000000000
namespace ext {

  class child_structure {
  public:
    child_structure(size_t id, size_t buffer_size,
		    double epsilon, std::vector<point> points);
    child_structure(size_t id);
    ~child_structure();
    void insert(point p);
    void remove(point p);
#ifdef VALIDATE
    bool valid_disk();
    bool valid_memory();
#endif
  private:
    struct catalog_item {
      int min_x, max_x, min_y, i, j;
      catalog_item(int _min_x, int _max_x, int _min_y, int _i, int _j) :
        min_x(_min_x), max_x(_max_x), min_y(_min_y), i(_i), j(_j) {}
    };
    std::string get_info_file();
    std::string get_directory();
    std::string get_L_file();
    std::string get_I_file();
    std::string get_D_file();
    void construct(std::vector<point> points);
    void rebuild();
    bool file_exists(std::string file_name);
    template <class InputIterator>
    void flush_container_to_file(InputIterator first,
				 InputIterator last, std::string file_name);
    template <class Container>
    void load_file_to_container(Container c, std::string file_name);
    std::vector<point> L;
    std::set<point> I, D;
    std::vector<catalog_item> catalog;
    size_t id, buffer_size, L_buffer_size, epsilon, L_size, I_size, D_size;
    const int NUM_VARIABLES = 6;
  };

  child_structure::child_structure(size_t id) {
    this->id = id;

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
    DEBUG_MSG("Loading L");
    load_file_to_container(L, get_L_file());
    DEBUG_MSG("Loading I");
    load_file_to_container(I, get_I_file());
    I_size = I.size();
    DEBUG_MSG("Loading D");
    load_file_to_container(D, get_D_file());
    D_size = D.size();
  }

  child_structure::child_structure(size_t id, size_t buffer_size,
				   double epsilon, std::vector<point> points) {

    this->id = id;
    
    //TODO use exceptions
    if (file_exists(get_info_file()))
      error(1, EEXIST, "child structure already exists");
    
    this->buffer_size = buffer_size;
    this->epsilon = (size_t)(1000.0*epsilon);
    this->L_buffer_size = (size_t)pow((double)buffer_size, 1.0+this->epsilon/1000.0);
    DEBUG_MSG("constructing child structure " << id <<
              " with B=" << buffer_size << " - e=" << epsilon << " - L_size=" << L_buffer_size);
    
    I_size = 0;
    D_size = 0;
    construct(points);
  }

  child_structure::~child_structure() {
    DEBUG_MSG("destructing child structure " << id);
    DEBUG_MSG("flushing variables to " << get_info_file());

    //check if directory exists and open:
    if (!file_exists(get_directory())) mkdir(get_directory().c_str(), 0700);
    io::buffered_stream<size_t> info_file(NUM_VARIABLES);
    info_file.open(get_info_file());

    info_file.write(id);
    info_file.write(buffer_size);
    info_file.write(epsilon);
    info_file.write(L_size);
    info_file.write(I.size());
    info_file.write(D.size());
    
    info_file.close();

    DEBUG_MSG("Flushing L");
    flush_container_to_file(L.begin(), L.end(), get_L_file());

    DEBUG_MSG("Flushing I");
    flush_container_to_file(I.begin(), I.end(), get_I_file());

    DEBUG_MSG("Flushing D");
    flush_container_to_file(D.begin(), D.end(), get_D_file());
    
  }

  template <class InputIterator>
  void child_structure::flush_container_to_file(InputIterator first, InputIterator last, std::string file_name) {
    io::buffered_stream<point> file(buffer_size);
    file.open(file_name);
    while (first != last) {
      file.write(*first);
      first++;
    }
    file.close();
  }

  template <class Container>
  void child_structure::load_file_to_container(Container c, std::string file_name) {
    io::buffered_stream<point> file(buffer_size);
    file.open(file_name);
    while (!file.eof()) {
      c.insert(c.end(), file.read());
    }
    file.close();
  }

  void child_structure::construct(std::vector<point> points) {
    L = points;
    L_size = points.size();
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
    
    //typedef std::pair<int,int> ii;
    internal::rb_tree<block> intervals;

    intervals.insert(block(-1000000,-1000000,-100000));
    intervals.insert(block(1000000,-1000000,-100000));
    // intervals.insert( ii(-10000000,-100000000) );
    // intervals.insert( ii(10000000,-100000000) );
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
        catalog.insert(catalog.end(), catalog_item(min_x, max_x, min_y, (i+1)/buffer_size-1, (i+1)/buffer_size-1));
        min_x = INF, max_x = -INF, min_y = INF;
        intervals.insert( block((i+1)/buffer_size-1,buffer_size,(i+1)/buffer_size-1));
        DEBUG_MSG("new block: " << (i+1)/buffer_size-1 << ", " << buffer_size);
      }
    }
    int remaining = points.size()%buffer_size;
    intervals.insert(block(remaining==0 ? points.size()/buffer_size-1 : points.size()/buffer_size
                           ,remaining==0 ? buffer_size: remaining,
                           remaining==0 ? points.size()/buffer_size-1 : points.size()/buffer_size));
    DEBUG_MSG("remaining block " << (remaining==0 ? points.size()/buffer_size-1 : points.size()/buffer_size) << " with points " << (remaining==0 ? buffer_size: remaining));

    DEBUG_MSG("writing remaining block to catalog");
    DEBUG_MSG(min_x << " " << max_x << " " << min_y << " " << (remaining==0 ? points.size()/buffer_size-1 : points.size()/buffer_size) << " " << (remaining==0 ? points.size()/buffer_size-1 : points.size()/buffer_size));
    catalog.insert(catalog.end(), catalog_item(min_x, max_x, min_y, remaining==0 ? points.size()/buffer_size-1 : points.size()/buffer_size, remaining==0 ? points.size()/buffer_size-1 : points.size()/buffer_size));

    // if (remaining > 0) {
    //   DEBUG_MSG("remaining block: "<< points.size()/buffer_size << ", " << remaining);
    //   intervals.insert(block( points.size()/buffer_size, remaining));
    // }

    while (!pq.empty()) {
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
          L.push_back(p);
        }
        intervals.erase(succ);
        intervals.erase(pb_belong_to);
        intervals.erase(pred);
        intervals.insert(block(pred.id, pb_belong_to.size-1, succ.right));
      } else if (our_size + pred.size == (int)buffer_size) {
        DEBUG_MSG("collapsing with left neighbour");
        DEBUG_MSG("constructing [" << pred.id << ", " << pb_belong_to.right << "]");
        std::vector<point> out;
        int limit = pb.first.y;
        for (point p : points_in_blocks[pb_belong_to.id])
          if (p.y >= limit) {
            DEBUG_MSG("above: " << p);
            out.push_back(p);
          }
#ifdef DEBUG
          else DEBUG_MSG("below: " << p);
#endif
        for (point p : points_in_blocks[pred.id])
          if (p.y >= limit) {
            DEBUG_MSG("above: " << p);
            out.push_back(p);
          }
#ifdef DEBUG
          else DEBUG_MSG("below: " << p);
#endif
        points_in_blocks[pred.id] = out;
        for (point p : out) L.push_back(p); //TODO: update catalog
        intervals.erase(pb_belong_to);
        intervals.erase(pred);
        intervals.insert(block(pred.id,out.size()-1,pb_belong_to.right));
      } else if (our_size + succ.size == (int)buffer_size) {
        DEBUG_MSG("collapsing with right neighbour");
        DEBUG_MSG("constructing [" << pb_belong_to.id << ", " << succ.right << "]");
        std::vector<point> out;
        int limit = pb.first.y;
        for (point p : points_in_blocks[pb_belong_to.id])
          if (p.y >= limit) {
            DEBUG_MSG("above: " << p);
            out.push_back(p);
          }
#ifdef DEBUG
          else DEBUG_MSG("below: " << p);
#endif
        for (point p : points_in_blocks[succ.id])
          if (p.y >= limit) {
            DEBUG_MSG("above: " << p);
            out.push_back(p);
          }
#ifdef DEBUG
          else DEBUG_MSG("below: " << p);
#endif
        points_in_blocks[pb_belong_to.id] = out;
        for (point p : out) L.push_back(p); //TODO: update catalog
        intervals.erase(succ);
        intervals.erase(pb_belong_to);
        intervals.insert(block(pb_belong_to.id, out.size()-1, succ.right));
      } else {
        DEBUG_MSG("removing element: " << pb.first << " from " << pb_belong_to.id << " and down to " << pb_belong_to.size-1);
        intervals.erase(pb_belong_to);
        intervals.insert(block(pb_belong_to.id, pb_belong_to.size-1,pb_belong_to.right));
      }
    }

  }

  void child_structure::insert(point p) {
    DEBUG_MSG("Insert point " << p);

    DEBUG_MSG(" - deleting from I");
    auto it = I.find(p);
    if (it != I.end()) I.erase(it);

    DEBUG_MSG(" - deleting from D");
    it = D.find(p);
    if (it != D.end()) D.erase(it);

    I.insert(p);

    if (I.size() > buffer_size) rebuild();
  }

  void child_structure::remove(point p) {
    DEBUG_MSG("Remove point " << p);

    DEBUG_MSG(" - deleting from I");
    auto it = I.find(p);
    if (it != I.end()) I.erase(it);

    DEBUG_MSG(" - deleting from D");
    it = D.find(p);
    if (it != D.end()) D.erase(it);

    D.insert(p);

    if (D.size() > buffer_size) rebuild();
  }

  void child_structure::rebuild() {
    //maintain size of L, i.e. L_size
  }
  
  bool child_structure::file_exists(std::string file_name) {
    DEBUG_MSG("Checking if " << file_name << " exists");
    struct stat st;
    return !(stat(file_name.c_str(), &st) == -1);
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
    if (!std::is_sorted(L.begin(), L.begin()+L_size)) {
      DEBUG_MSG("L is not sorted w.r.t x");
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
      if (min_x > max_x) { DEBUG_MSG("min x > max_x"); return false;}
      for (size_t j = i*buffer_size; j < std::min(i*buffer_size+buffer_size,L_size); j++) {
        if (L[j].x < min_x || max_x < L[j].x) {
          DEBUG_MSG("point " << L[j] << " not between " << min_x << " and " << max_x << " in block " << i); return false;}
        if (L[j].y < min_y) { DEBUG_MSG("point " << L[j] << " has y less than min_y for block " << i); return false;}
      }
    }

    // TODO: Check I_size, D_size, L_size is correct w.r.t. files.
    
    return true;
  }
#endif

};

#endif
