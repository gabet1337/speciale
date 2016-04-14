#ifndef DYNAMIC_PRIORITY_SEARCH_TREE_HPP
#define DYNAMIC_PRIORITY_SEARCH_TREE_HPP
#include "../common/pst_interface.hpp"
#include "../common/point.hpp"
#include "../common/definitions.hpp"
#include <iostream>
#include <queue>
#include <fstream>

namespace internal {

  class DynamicPrioritySearchTree : public common::pst_interface {

    struct node {
      node *left, *right, *parent;
      point key, key_y;
      bool placeholder;
      bool leaf() {
        return left == 0 && right == 0;
      }
      node() : left(0), right(0), key(point(-INF,-INF)), key_y(point(-INF,-INF)),
               placeholder(false) {};
      node(point p) : left(0), right(0), key(p), key_y(point(-INF,-INF)),
                      placeholder(false) {};
    };
    
  public:
    DynamicPrioritySearchTree();
    DynamicPrioritySearchTree(size_t buffer_size, double epsilon);
    ~DynamicPrioritySearchTree();
    void insert(const point &p);
    void remove(const point &p);
    void report(int x1, int x2, int y, const std::string &output_file);
    void print();
  private:
    size_t size;
    node *root;
  };

  DynamicPrioritySearchTree::DynamicPrioritySearchTree() {
    root = new node(point(-INF,-INF));
    size = 0;
  }

  DynamicPrioritySearchTree::~DynamicPrioritySearchTree() {
    //TODO add destructor.
    std::cerr << "destructued" << std::endl;
  }

  void DynamicPrioritySearchTree::insert(const point &p) {

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
        right_child->key = root->key;
      } else {
        left_child->key = root->key;
        right_child->key = p;
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
    
    node *existing_leaf = root;
    node *new_leaf = new node(p);
    new_leaf->placeholder = true;
    
    while (!existing_leaf->leaf()) {
      if (p < existing_leaf->key)
        existing_leaf = existing_leaf->left;
      else
        existing_leaf = existing_leaf->right;
    }

    node *new_internal_node = new node();
    new_internal_node->parent = existing_leaf->parent;
    existing_leaf->parent = new_internal_node;
    new_leaf->parent = new_internal_node;

    if (new_internal_node->parent->left == existing_leaf)
      new_internal_node->parent->left = new_internal_node;
    else
      new_internal_node->parent->right = new_internal_node;
    
    new_internal_node->key = p;
    new_internal_node->key_y = point(-INF,-INF);
    
    if (existing_leaf->key < p) {
      new_internal_node->left = existing_leaf;
      new_internal_node->right = new_leaf;
    } else {
      new_internal_node->left = new_leaf;
      new_internal_node->right = existing_leaf;
    }

    node *n = root;
    point cp = p;
    
    while (!n->leaf()) {
      if (comp_y(cp,n->key_y)) {
        if (cp < n->key)
          n = n->left;
        else
          n = n->right;
      } else {
        std::swap(n->key_y,cp);
      }
      if (n->leaf() && n->key == cp)
        n->placeholder = false;
    }

  }

  void DynamicPrioritySearchTree::remove(const point &p) {

    std::cout << "removing point " << p << std::endl;
    
    node *in_node = 0;
    node *node = root;
    
    while (!node->leaf()) {
      std::cout << "testing if node->key_y " << node->key_y << " == " << p << std::endl;
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
      std::cout << "node with key " << node->key << " is leaf" << std::endl;
      if (node->parent->left == node) {
        std::cout << "deleting left child from parent with key "
                  << node->parent->key << std::endl;
        node->parent->left = 0;
      } else {
        std::cout << "deleting right child from parent with key "
                  << node->parent->key << std::endl;
        node->parent->right = 0;
      }
      delete node;
    }


     if (p == point(20,20)) {
       std::cout << "we have returned!!!" << std::endl;
       return;
     }
    
    if (in_node != 0) {

      while (!in_node->leaf()) {
        point left_cand = point(-INF,-INF);
        point right_cand = point(-INF,-INF);
        if (in_node->left != 0 && !in_node->left->placeholder) {
          if (!in_node->left->leaf())
            left_cand = in_node->left->key_y;
          else
            left_cand = in_node->left->key;
        }
        if (in_node->right != 0 && !in_node->right->placeholder) {
          if (!in_node->right->leaf())
            right_cand = in_node->right->key_y;
          else
            right_cand = in_node->right->key;
        }
        in_node->key_y = std::max(left_cand,right_cand,comp_y);
        if (in_node->key == point(-INF,-INF)) break;
        std::cout << "checking if in_node->key_y == left_cand "
                  << in_node->key_y << " " << left_cand << std::endl;
        if (in_node->key_y == left_cand)
          in_node = in_node->left;
        else
          in_node = in_node->right;
        std::cout << "checking if node " << in_node->key << " is leaf? " << in_node->leaf() <<
          std::endl;
        if (in_node->leaf())
          in_node->placeholder = true;
      }
      
    }
    
  }

  void DynamicPrioritySearchTree::report(int x1, int x2, int y, const std::string &output_file) {
    
  }

  void DynamicPrioritySearchTree::print() {

    std::ofstream dot_file;
    dot_file.open("temp.dot");
    dot_file << "digraph G {" << std::endl;

    std::queue<DynamicPrioritySearchTree::node*> q;
    q.push(root);
    while (!q.empty()) {
      node* x = q.front(); q.pop();
      dot_file << (uintptr_t)x << " [label=\"";

      if (!(x->key_y == point(-INF,-INF)) && !x->leaf())
        dot_file << "key_y: " << x->key_y << "\n";
      else if (!x->leaf())
        dot_file << "key_y: " << "\n";

      if (!x->leaf())
        dot_file << "key_x: ";
      else
        dot_file << "key: ";
      
      if (!(x->key == point(-INF,-INF)))
        dot_file << x->key;

      if (x->placeholder && x->leaf())
        dot_file << "\nph: true";
      else if (x->leaf())
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
