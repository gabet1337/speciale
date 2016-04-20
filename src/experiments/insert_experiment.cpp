#include "base_experiment.hpp"
using namespace experiment;

class insert_experiment : public base_experiment {

  public:
  insert_experiment(const std::string &exp_name) : base_experiment(exp_name) {}
  ~insert_experiment() {}

  void run_experiment(run_instance instance) {
    stream data = get_data_stream("../data/insert_experiment");
    auto pst = PST_factory(instance.type, instance.buffer_size, instance.epsilon);
    size_t data_read = 0;
    size_t data_size = sizeof(point);
    size_t interval = 100 * 1024 * 1024; //100 mb intervals
    size_t print_interval = 1024 * 1024;
    test::clock timer;
    timer.start();
    while (!data.eof()) {
      point p = data.read();
      pst->insert(p);
      data_read += data_size;
      if (data_read % interval == 0) {
        add_result(type, result::MEASURE::time, data_read/(1024*1024), timer.elapsed());

        save_results(instance);
      }

      if (data_read % print_interval == 0) {
        std::cout << data_read/print_interval << "MB" << std::endl;
      }
    }
    delete pst;
  }

};

int main() {

  insert_experiment ie("insert_experiment");
  ie.run();

  return 0;
}
