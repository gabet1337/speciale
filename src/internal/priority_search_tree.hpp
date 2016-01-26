#ifndef PRIORITY_SEARCH_TREE_HPP
#define PRIORITY_SEARCH_TREE_HPP
#include "../common/point.hpp"
#include <algorithm>
#include <vector>
#include <stdio.h>

class PrioritySearchTree {

public:
  // PrioritySearchTree();
  PrioritySearchTree(std::vector<Point> points);
  ~PrioritySearchTree();
  void print();

private:
  struct Node {
    Point root;
    int key;
    Node* left, *right;
  };
  Node* root;
  Node* construct(std::vector<Point> points);
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
  v->key = points[(points.size()-1)/2].x;
  // root of subtree is the point with max y value
  auto pmin = std::min_element(points.begin(), points.end(),
			       [] (const Point &p1, const Point &p2) -> bool {
				 return p1.y < p2.y;
			       });
  v->root = *pmin;
  std::vector<Point> left,right;
  for (auto p = points.begin(); p != points.end(); p++) {
    if (p == pmin) continue;
    if (v->key < p->x) right.push_back(*p);
    else left.push_back(*p);
  }
  v->left = construct(left);
  v->right = construct(right);
  return v;
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
  if (n->left) {printf("%llu -> %llu\n",
		       (unsigned long long int)n,
		       (unsigned long long int)n->left);
    print(n->left);
  }
  if (n->right) { printf("%llu -> %llu\n",
			 (unsigned long long int)n,
			 (unsigned long long int)n->right);
    print(n->right);
  }
}

#endif
