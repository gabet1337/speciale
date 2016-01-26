#ifndef PRIORITY_SEARCH_TREE_HPP
#define PRIORITY_SEARCH_TREE_HPP
#include "../common/point.hpp"
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <iostream>

class PrioritySearchTree {

public:
  // PrioritySearchTree();
  PrioritySearchTree(std::vector<Point> points);
  ~PrioritySearchTree();
  std::vector<Point> report(int xl, int xr, int yb);

  void print();

private:
  struct Node {
    Point root;
    int key;
    Node* left, *right;
  };
  Node* root;
  Node* construct(std::vector<Point> points);
  void reportSubtree(Node* n, int y, std::vector<Point> &result);
  void print(Node* n);
};

PrioritySearchTree::~PrioritySearchTree() {}

PrioritySearchTree::PrioritySearchTree(std::vector<Point> points) {
  std::sort(points.begin(), points.end());
  root = construct(points);
}

PrioritySearchTree::Node* PrioritySearchTree::construct(std::vector<Point> points) {
  if (points.empty()) return 0;
  PrioritySearchTree::Node* v = new PrioritySearchTree::Node();
  // key is the x value of the median of the points
  // root of subtree is the point with max y value
  auto pmin = std::max_element(points.begin(), points.end(),
			       [] (const Point &p1, const Point &p2) -> bool {
				 return p1.y < p2.y;
			       });
  v->root = *pmin;
  size_t pos = pmin - points.begin();
  size_t mid = (points.size()-1)/2;
  if (points.size() == 1) v->key = points[0].x;
  else if (points.size() == 2) {
    if (pos == 0) v->key = points[1].x;
    else v->key = points[0].x;
  } else {
    if (pos == mid) v->key = points[mid-1].x;
    if (pos < mid) v->key = points[mid+1].x;
    else v->key = points[mid].x;
  }
  std::vector<Point> left,right;
  // TODO: If many points have x value equal to median
  // then split this correctly!
  for (auto p = points.begin(); p != points.end(); p++) {
    if (p == pmin) continue;
    if (v->key < p->x) right.push_back(*p);
    else left.push_back(*p);
  }
  v->left = construct(left);
  v->right = construct(right);
  return v;
}

std::vector<Point> PrioritySearchTree::report(int xl, int xr, int yb) {
  std::vector<Point> result;
  if (xl > xr) return result;

  //search with xr and xl. Let vsplit be the node where the search paths split
  PrioritySearchTree::Node *left = root, *right = root;
  while (left && left == right) {
    int x = left->root.x, y = left->root.y;
    if (xl <= x && x <= xr && y >= yb) result.push_back(left->root);
    if (xl > left->key) left = left->right;
    else left = left->left;
    if (xr > right->key) right = right->right;
    else right = right->left;
  }
  
  // if (left) reportSubtree(left->right, yb, result);
  while (left && left->root.y >= yb) {
    if (xl <= left->root.x && left->root.x <= xr && left->root.y >= yb)
      result.push_back(left->root);
    if (left->left && !(left->right)) {
      left = left->left;
    } else if (!(left->left) && left->right) {
      left = left->right;
    } else if (xl > left->key) left = left->right;
    else {
      reportSubtree(left->right, yb, result);
      left = left->left;
    }
  }

  // if (right) reportSubtree(right->left, yb, result);
  while (right && right->root.y >= yb) {
    if (xl <= right->root.x && right->root.x <= xr && right->root.y >= yb)
      result.push_back(right->root);
    if (right->left && !right->right) {
      right = right->left;
    } else if (!right->left && right->right) {
      right = right->right;
    } else if (xr > right->key) {
      reportSubtree(right->left, yb, result);
      right = right->right;
    } else right = right->left;
  }

  return result;
}

void PrioritySearchTree::reportSubtree(PrioritySearchTree::Node* n, int y, std::vector<Point> &result) {
  if (n && n->root.y >= y) {
    result.push_back(n->root);
    reportSubtree(n->left, y, result);
    reportSubtree(n->right, y, result);
  }
}

void PrioritySearchTree::print() {
  puts("digraph {");
  print(root);
  puts("}");
}

void PrioritySearchTree::print(PrioritySearchTree::Node* n) {
  if (n) {
    printf("%llu [label=\"%d\\n(%d,%d)\"]\n",
	   (unsigned long long int)n, n->key, n->root.x, n->root.y);
  }
  if (n->left) {printf("%llu -> %llu [label=\"l\"]\n",
		       (unsigned long long int)n,
		       (unsigned long long int)n->left);
    print(n->left);
  }
  if (n->right) { printf("%llu -> %llu [label=\"r\"]\n",
			 (unsigned long long int)n,
			 (unsigned long long int)n->right);
    print(n->right);
  }
}

#endif
