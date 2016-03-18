#include <iostream>
#include "balanced_pst.hpp"
using namespace std;

int main() {
  internal::balanced_pst rb;
  for (int i = 0; i < 100; i++) rb.insert(point(i,i));

  rb.print();

  return 0;
}
