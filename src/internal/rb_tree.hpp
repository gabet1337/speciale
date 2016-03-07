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
    void insert(const T &item);
    void insert(typename std::set<T>::iterator it, const T &item);
    void erase(const T &item);
    void clear();
    size_t size();
    bool empty();
    T predecessor(const T &item);
    T successor(const T &item);
    T belong_to(const T &item);
    typename std::set<T>::iterator belong_to_iterator(const T &item);
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
  bool rb_tree<T>::empty() {
    return s.empty();
  }
  
  template <typename T>
  void rb_tree<T>::insert(const T &item) {
    s.insert(item);
  }

  template <typename T>
  void rb_tree<T>::insert(typename std::set<T>::iterator it, const T &item) {
    s.insert(it, item);
  }

  template <typename T>
  size_t rb_tree<T>::size() {
    return s.size();
  }
  
  template <typename T>
  void rb_tree<T>::erase(const T &item) {
    s.erase(s.find(item));
  }

  template <typename T>
  T rb_tree<T>::predecessor(const T &item) {
    return *(--(s.lower_bound(item)));
  }

  template <typename T>
  T rb_tree<T>::successor(const T &item) {
    return *(s.upper_bound(item));
  }

  template <typename T>
  T rb_tree<T>::belong_to(const T &item) {
    auto it = s.lower_bound(item);
    if (*it == item) return *it;
    if (it == s.begin()) return *it;
    return *(--it);
  }

  template <typename T>
  typename std::set<T>::iterator rb_tree<T>::belong_to_iterator(const T &item) {
    auto it = s.lower_bound(item);
    if (*it == item) return it;
    if (it == s.begin()) return it;
    return (--it);
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
