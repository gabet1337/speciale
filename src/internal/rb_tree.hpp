#ifndef RB_TREE_HPP

#include <set>

namespace internal {
  
  template <typename T>
  class rb_tree {
  public:
    rb_tree();
    ~rb_tree();
    void insert(T item);
    void erase(T item);
    T predecessor(T item);
    T successor(T item);
  private:
    std::set<T> s;
  };

  template <typename T>
  rb_tree<T>::rb_tree() {}
  
  template <typename T>
  rb_tree<T>::~rb_tree() {}

  template <typename T>
  void rb_tree<T>::insert(T item) {
    s.insert(item);
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

};


#endif
