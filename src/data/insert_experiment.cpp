#include "../common/test_lib.hpp"

int main() {

  test::point_data_generator pdg;

  pdg.generate(10ULL*1024ULL*1024ULL*1024ULL, "insert_experiment");

  return 0;
};
