#ifndef DYNAMIC_PRIORITY_SEARCH_TREE_HPP
#define DYNAMIC_PRIORITY_SEARCH_TREE_HPP
#include "../common/pst_interface.hpp"
#include "../common/point.hpp"
#include "../common/definitions.hpp"
#include "../common/utilities.hpp"
#include <iostream>
#include <queue>
#include <fstream>
#include <queue>
#include <error.h>

namespace internal {

  class dynamic_pst : public common::pst_interface {

    struct node {
      node *left, *right, *parent;
      point key, key_y;
      bool placeholder;
      bool leaf() {
        return left == 0 && right == 0;
      }
      node() : left(0), right(0), key(point(-INF,-INF)), key_y(point(-INF,-INF)),
               placeholder(false) {};
      node(point p) : left(0), right(0), key(p), key_y(p),
                      placeholder(false) {};
    };
    
  public:
    dynamic_pst();
    dynamic_pst(size_t buffer_size, double epsilon);
    ~dynamic_pst();
    void insert(const point &p);
    void remove(const point &p);
    void report(int x1, int x2, int y, const std::string &output_file);
    void print();
  private:
    void construct(std::vector<point> points);
    size_t size, buffer_size;
    node *root;
  };

  dynamic_pst::dynamic_pst() : dynamic_pst(4096, 0.0) {}

  dynamic_pst::dynamic_pst(size_t _buffer_size, double epsilon) {
    root = new node(point(-INF,-INF));
    size = 0;
    buffer_size = _buffer_size;
  }

  dynamic_pst::~dynamic_pst() {
    std::queue<node*> q;
    q.push(root);
    while (!q.empty()) {
      node* n = q.front(); q.pop();
      if (n->left != 0)
        q.push(n->left);
      if (n->right != 0)
        q.push(n->right);
      delete n;
    }
  }

  void dynamic_pst::insert(const point &p) {

    if (size++ == 0) {
      root->key = p;
      root->key_y = p;
      return;
    }

    if (root->leaf()) {

      node *left_child = new node();
      node *right_child = new node();
      left_child->parent = root;
      right_child->parent = root;
      
      if (p < root->key) {
        left_child->key = p;
        left_child->key_y = p;
        right_child->key = root->key;
        right_child->key_y = root->key;
      } else {
        left_child->key = root->key;
        left_child->key_y = root->key;
        right_child->key = p;
        right_child->key_y = p;
        root->key = p;
      }

      if (comp_y(root->key_y,p))
        root->key_y = p;

      if (root->key_y == left_child->key)
        left_child->placeholder = true;
      else
        right_child->placeholder = true;
      
      root->left = left_child;
      root->right = right_child;

      return;
    } 

    point cp = p;
    node *existing_leaf = root;
    node *new_leaf = new node(p);
    new_leaf->placeholder = true;
    
    while (!existing_leaf->leaf()) {
      if (p < existing_leaf->key)
        existing_leaf = existing_leaf->left;
      else
        existing_leaf = existing_leaf->right;
    }

    if (new_leaf->key_y < existing_leaf->key_y && !existing_leaf->placeholder) {
      new_leaf->placeholder = false;
      existing_leaf->placeholder = true;
      cp = existing_leaf->key_y;
    }

    node *new_internal_node = new node();
    new_internal_node->parent = existing_leaf->parent;
    existing_leaf->parent = new_internal_node;
    new_leaf->parent = new_internal_node;

    if (new_internal_node->parent->left == existing_leaf)
      new_internal_node->parent->left = new_internal_node;
    else
      new_internal_node->parent->right = new_internal_node;
    
    new_internal_node->key = std::max(p,existing_leaf->key);
    new_internal_node->key_y = point(-INF,-INF);
    
    if (existing_leaf->key < p) {
      new_internal_node->left = existing_leaf;
      new_internal_node->right = new_leaf;
    } else {
      new_internal_node->left = new_leaf;
      new_internal_node->right = existing_leaf;
    }

    node *n = root;
    
    while (!n->leaf()) {
      if (cp.y <= n->key_y.y || (cp.y == n->key_y.y && cp.x <= n->key_y.x))  {
        if (cp < n->key)
          n = n->left;
        else
          n = n->right;
      } else {
        std::swap(n->key_y,cp);
      }
    }
    if (n->leaf()) {
      if (n->key == cp)
        n->placeholder = false;
    }
  }

  void dynamic_pst::remove(const point &p) {

    node *in_node = 0;
    node *node = root;
    
    while (!node->leaf()) {
      if (node->key_y == p) {
        node->key_y = point(INF,INF);
        in_node = node;
      }
      if (p < node->key)
        node = node->left;
      else
        node = node->right;
    }

    if (node->leaf() && node->key == p) {
      if (node->parent->left == node) {
        node->parent->left = 0;
      } else {
        node->parent->right = 0;
      }
      delete node;
    }

    if (in_node != 0) {

      while (!in_node->leaf()) {
        point left_cand = point(-INF,-INF);
        point right_cand = point(-INF,-INF);
        if (in_node->left != 0 && !in_node->left->placeholder)
          left_cand = in_node->left->key_y;
        if (in_node->right != 0 && !in_node->right->placeholder)
          right_cand = in_node->right->key_y;
        in_node->key_y = std::max(left_cand,right_cand,comp_y);
        if (in_node->key == point(-INF,-INF)) break;
        if (in_node->key_y == left_cand)
          in_node = in_node->left;
        else
          in_node = in_node->right;
        if (in_node->leaf())
          in_node->placeholder = true;
      }
      
    }
    
  }

  void dynamic_pst::report(int x1, int x2, int y,
                                         const std::string &output_file) {

    if (util::file_exists(output_file))
      error(1, ECANCELED, "Output file exists. Aborting.");

    io::buffered_stream<point> result(buffer_size);
    result.open(output_file);

    if (x2 < x1) {
      DEBUG_MSG("x2 < x1 for x1: " << x1 << " x2: " <<x2);
      result.close();
      return;
    }

    
    std::queue<node*> q;
    q.push(root);
    
    while (!q.empty()) {
      
      node* n = q.front(); q.pop();

      if (n->key_y > point(-INF,-INF) && !n->placeholder && util::in_range(n->key_y, x1, x2, y))
        result.write(n->key_y);

      if (point(x1,y) <= n->key && n->left != 0)
        q.push(n->left);

      if (n->key <= point(x2,y) && n->right != 0)
        q.push(n->right);
      
    }

    result.close();
    
  }

  void dynamic_pst::construct(std::vector<point> points) {

    std::vector<dynamic_pst::node*> layer_i_plus_1, layer_i;

    for (size_t i = 0; i < points.size() / 2; i++) {

      node* node = new dynamic_pst::node();

      if (i*2+1 < points.size()) {
        
        dynamic_pst::node* leaf_left = new dynamic_pst::node();
        dynamic_pst::node* leaf_right = new dynamic_pst::node();
        
        leaf_left->parent = node;
        leaf_left->key = points[i*2];
        leaf_left->key_y = leaf_left->key; 
        
        leaf_right->parent = node;
        leaf_right->key = points[i*2+1];
        leaf_right->key_y = leaf_right->key;
        
        node->left = leaf_left;
        node->right = leaf_right;
        
        node->key = leaf_right->key; // std::max(leaf_left->key, leaf_right->key);
        node->key_y = std::max(leaf_left->key_y, leaf_right->key_y, comp_y);
        
      } else {
        
        dynamic_pst::node* leaf = new dynamic_pst::node();
        leaf->key = points[i*2];
        leaf->key_y = leaf->key;
        leaf->parent = node;
        node->key = leaf->key;
        node->key_y = leaf->key_y;
        node->left = 0;
        node->right = leaf;
        
      }
      
      layer_i.push_back(node);
    }

    while (layer_i.size() > 2) {
      std::swap(layer_i, layer_i_plus_1);
      layer_i.clear();
      for (size_t i = 0; i < layer_i_plus_1.size() / 2; i++) {
        dynamic_pst::node* node = new dynamic_pst::node();
        dynamic_pst::node* leaf_left = layer_i_plus_1[i*2];
        dynamic_pst::node* leaf_right = layer_i_plus_1[i*2+1];
        
        node->key = leaf_right->key;
        node->key_y = std::max(leaf_left->key_y, leaf_right->key_y, comp_y);
        node->left = leaf_left;
        node->right = leaf_right;
        
      }
    }
    
  }

  void dynamic_pst::print() {

    std::ofstream dot_file;
    dot_file.open("temp.dot");
    dot_file << "digraph G {" << std::endl;

    std::queue<dynamic_pst::node*> q;
    q.push(root);
    while (!q.empty()) {
      node* x = q.front(); q.pop();
      dot_file << (uintptr_t)x << " [label=\"";

      if (!(x->key_y == point(-INF,-INF)))
        dot_file << "key_y: " << x->key_y << "\n";
      else
        dot_file << "key_y: " << "\n";

      dot_file << "key_x: ";
            
      if (!(x->key == point(-INF,-INF)))
        dot_file << x->key;

      if (x->placeholder)
        dot_file << "\nph: true";
      else
        dot_file << "\nph: false";

      if (x->parent != 0)
        dot_file << "\nparent: " << x->parent->key;
      
      dot_file << "\"]" << std::endl;
      
      if (x->left != 0)
	dot_file << (uintptr_t)x << " -> " << (uintptr_t)x->left << std::endl;
      if (x->right != 0)
	dot_file << (uintptr_t)x << " -> " << (uintptr_t)x->right << std::endl;
      if (x->left != 0)
        q.push(x->left);
      if (x->right != 0)
        q.push(x->right);
    }
    

    dot_file << "}" << std::endl;
    dot_file.close();
    int r = system("dot -Tpng -o tree.png temp.dot");
    r++;
  }
    
}

#endif
