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
    external_priority_search_tree();
    ~external_priority_search_tree();

    void insert(const point &p);
    void remove(const point &p);
    void report(int x1, int x2, int y, const std::string &output_file);
    void print();


  };


};

#endif
