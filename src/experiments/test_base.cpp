#include "base_experiment.hpp"
using namespace experiment;

class stub : public base_experiment {

public:
  stub(const std::string &exp_name) : base_experiment(exp_name) {}
  ~stub() {}
  void run_experiment(const common::pst_interface& pst) {
    std::cout << "RUNNING EXPERIMENT!" << std::endl;
  }

  void plot() {}

};


int main() {

  stub test("rofl_test");
  test.run();
  
  return 0;

}
