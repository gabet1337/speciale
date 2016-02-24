#ifndef RB_TREE_HPP
#define RB_TREE_HPP
#include <set>
#include <sys/types.h>

namespace internal {

  template <typename T>
  class rb_tree {
  public:
    rb_tree();
    ~rb_tree();
    void insert(T item);
    void erase(T item);
    void clear();
    size_t size();
    T predecessor(T item);
    T successor(T item);
    T belong_to(T item);
    typename std::set<T>::iterator begin();
    typename std::set<T>::iterator end();
  private:
    std::set<T> s;
  };

  template <typename T>
  rb_tree<T>::rb_tree() {}
  
  template <typename T>
  rb_tree<T>::~rb_tree() {}

  template <typename T>
  void rb_tree<T>::clear() {
    s.clear();
  }
  
  template <typename T>
  void rb_tree<T>::insert(T item) {
    s.insert(item);
  }

  template <typename T>
  size_t rb_tree<T>::size() {
    return s.size();
  }
  
  template <typename T>
  void rb_tree<T>::erase(T item) {
    s.erase(s.find(item));
  }

  template <typename T>
  T rb_tree<T>::predecessor(T item) {
    return *(--(s.lower_bound(item)));
  }

  template <typename T>
  T rb_tree<T>::successor(T item) {
    return *(s.upper_bound(item));
  }

  template <typename T>
  T rb_tree<T>::belong_to(T item) {
    auto it = s.lower_bound(item);
    if (*it == item) return *it;
    if (it == s.begin()) return *it;
    return *(--it);
  }

  template <typename T>
  typename std::set<T>::iterator rb_tree<T>::begin() {
    return s.begin();
  }

  template <typename T>
  typename std::set<T>::iterator rb_tree<T>::end() {
    return s.end();
  }

};


#endif
