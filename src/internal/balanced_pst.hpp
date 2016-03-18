#ifndef BALANCED_PST_HPP
#define BALANCED_PST_HPP
#include <cstddef>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdint.h>
#include <queue>
#include "../common/point.hpp"
#define RED 1
#define BLACK 0
namespace internal {

  class balanced_pst {

    struct node {
      bool color;
      point key;
      node *left, *right, *p;
      node(point _key) : key(_key) {}
    };

  public:
    balanced_pst();
    ~balanced_pst();
    void insert(const point &v);
    void erase(const point &v);
    size_t size();
    bool empty();
    void print();
    
  private:
    size_t _size;
    node *root, *sentinel;
    void rotate_left(node* x);
    void rotate_right(node *x);
    void insert_fixup(node *z);
    void transplant(node *u, node *v);
    void erase_fixup(node *x);
    node* min(node* st);
    node* find(const point &v);
    void destruct(node* st);
  };

  balanced_pst::balanced_pst() {
    sentinel = new node(point(0,0));
    sentinel->color = BLACK;
    sentinel->p = sentinel;
    sentinel->right = sentinel;
    sentinel->left = sentinel;
    root = sentinel;
    _size = 0;
  }

  size_t balanced_pst::size() {
    return _size;
  }

  void balanced_pst::destruct(node *st) {
    if (st->left != sentinel)
      destruct(st->left);
    if (st->right != sentinel)
      destruct(st->right);
    delete st;
  }

  balanced_pst::~balanced_pst() {
    if (root != sentinel)
      destruct(root);
    delete sentinel;
  }

  void balanced_pst::rotate_left(node *x) {
    node *y = x->right;
    x->right = y->left;
    if (y->left != sentinel) y->left->p = x;
    y->p = x->p;
    if (x->p == sentinel) root = y;
    else if (x == x->p->left) x->p->left = y;
    else x->p->right = y;
    y->left = x;
    x->p = y;
  }

  void balanced_pst::rotate_right(node *y) {
    node *x = y->left;
    y->left = x->right;
    if (x->right != sentinel) x->right->p = y;
    x->p = y->p;
    if (y->p == sentinel) root = x;
    else if (y == y->p->right) y->p->right = x;
    else y->p->left = x;
    x->right = y;
    y->p = x;
  }

  void balanced_pst::insert(const point &v) {
    node* z = new node(v);
    node* y = sentinel;
    node* x = root;
    while (x != sentinel) {
      y = x;
      if (z->key < x->key) x = x->left;
      else x = x->right;
    }
    z->p = y;
    if (y == sentinel) root = z;
    else if (z->key < y->key) y->left = z;
    else y->right = z;
    z->left = sentinel;
    z->right = sentinel;
    z->color = RED;
    insert_fixup(z);
    _size++;
  }

  void balanced_pst::insert_fixup(node* z) {
    while (z->p->color == RED) {
      if (z->p == z->p->p->left) {
        node* y = z->p->p->right;
        if (y->color == RED) {
          z->p->color = BLACK;
          y->color = BLACK;
          z->p->p->color = RED;
          z = z->p->p;
        } else {
          if (z == z->p->right) {
            z = z->p;
            rotate_left(z);
          }
          z->p->color = BLACK;
          z->p->p->color = RED;
          rotate_right(z->p->p);
        }
      } else {
        node* y = z->p->p->left;
        if (y->color == RED) {
          z->p->color = BLACK;
          y->color = BLACK;
          z->p->p->color = RED;
          z = z->p->p;
        } else {
          if (z == z->p->left) {
            z = z->p;
            rotate_right(z);
          }
          z->p->color = BLACK;
          z->p->p->color = RED;
          rotate_left(z->p->p);
        }
      }
    }
    root->color = BLACK;
  }

  balanced_pst::node* balanced_pst::find(const point &v) {
    node *x = root;
    while (x->key != v) {
      if (v < x->key) x = x->left;
      else x = x->right;
    }
    return x;
  }

  void balanced_pst::erase(const point &v) {
    node* z = find(v);
    node* y = z;
    node* x;
    bool y_original_color = y->color;
    if (z->left == sentinel) {
      x = z->right;
      transplant(z,z->right);
    } else if (z->right == sentinel) {
      x = z->left;
      transplant(z,z->left);
    } else {
      y = min(z->right);
      y_original_color = y->color;
      x = y->right;
      if (y->p == z) x->p = y;
      else {
        transplant(y,y->right);
        y->right = z->right;
        y->right->p = y;
      }
      transplant(z,y);
      y->left = z->left;
      y->left->p = y;
      y->color = z->color;
    }
    if (y_original_color == BLACK)
      erase_fixup(x);
    _size--;
    delete z;
  }

  void balanced_pst::erase_fixup(node* x) {
    while (x != root && x->color == BLACK) {
      if (x == x->p->left) {
        node* w = x->p->right;
        if (w->color == RED) {
          w->color = BLACK;
          x->p->color = RED;
          rotate_left(x->p);
          w = x->p->right;
        }
        if (w->left->color == BLACK && w->right->color == BLACK) {
          w->color = RED;
          x = x->p;
        } else {
          if (w->right->color == BLACK) {
            w->left->color = BLACK;
            w->color = RED;
            rotate_right(w);
            w = x->p->right;
          }
          w->color = x->p->color;
          x->p->color = BLACK;
          w->right->color = BLACK;
          rotate_left(x->p);
          x = root;
        }
      } else {
        node* w = x->p->left;
        if (w->color == RED) {
          w->color = BLACK;
          x->p->color = RED;
          rotate_right(x->p);
          w = x->p->left;
        }
        if (w->right->color == BLACK && w->left->color == BLACK) {
          w->color = RED;
          x = x->p;
        } else {
          if (w->left->color == BLACK) {
            w->right->color = BLACK;
            w->color = RED;
            rotate_left(w);
            w = x->p->left;
          }
          w->color = x->p->color;
          x->p->color = BLACK;
          w->left->color = BLACK;
          rotate_right(x->p);
          x = root;
        }
      }
    }
    x->color = BLACK;
  }

  void balanced_pst::transplant(node* u, node* v) {
    if (u->p == sentinel) root = v;
    else if (u == u->p->left) u->p->left = v;
    else u->p->right = v;
    v->p = u->p;
  }

  balanced_pst::node* balanced_pst::min(node *st) {
    while (st->left != sentinel) st = st->left;
    return st;
  }

  // predecessor_queue
  bool balanced_pst::empty() {
    return (_size == 0);
  }
  
  void balanced_pst::print() {
    std::ofstream dot_file;
    dot_file.open("temp.dot");
    dot_file << "digraph G {" << std::endl;

    std::queue<balanced_pst::node*> q;
    q.push(root);
    while (!q.empty()) {
      node* x = q.front(); q.pop();
      dot_file << (uintptr_t)x << " [label=\"" << x->key << "\"]" << std::endl;
      if (x->left != sentinel)
	dot_file << (uintptr_t)x << " -> " << (uintptr_t)x->left << std::endl;
      if (x->right != sentinel)
	dot_file << (uintptr_t)x << " -> " << (uintptr_t)x->right << std::endl;
      if (x->left != sentinel)
        q.push(x->left);
      if (x->right != sentinel)
        q.push(x->right);
    }
    

    dot_file << "}" << std::endl;
    dot_file.close();
    int r = system("dot -Tpng -o tree.png temp.dot");
    r++;
  }
  

};

#endif
