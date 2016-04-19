#include "base_experiment.hpp"
using namespace experiment;

class gerth_fanout_experiment : public base_experiment {

  public:
  gerth_fanout_experiment(const std::string &exp_name) : base_experiment(exp_name) {
  }
  ~gerth_fanout_experiment() {}

  void run_experiment(run_instance instance) {
    stream data = get_data_stream("../data/insert_experiment");
    auto pst = PST_factory(instance.type, instance.buffer_size, instance.epsilon);
    size_t data_read = 0;
    size_t data_size = sizeof(point);
    size_t interval = 10 * 1024 * 1024; //10 mb intervals
    size_t print_interval = 1024 * 1024;
    size_t stop_at = 1024 * 1024 * 500;
    test::clock timer;
    timer.start();
    while (!data.eof()) {
      point p = data.read();
      pst->insert(p);
      data_read += data_size;

      if (data_read % print_interval == 0) {
        std::cout << data_read/print_interval << "MB" << std::endl;
      }

      if (data_read % interval == 0) {
        add_result(instance.id, result::MEASURE::time, data_read/(1024*1024), timer.elapsed());

        save_results(instance);
        if (data_read >= stop_at) break;
      }
    }
    delete pst;
  }

};

int main() {

  gerth_fanout_experiment gfe("gerth_fanout_experiment");
  gfe.add(1, "fanout2", gfe.PST_TYPE::GERTH, 1024*1024, log(2.5)/log(1024.0*1024.0)); //fanout 2
  gfe.add(2, "fanout3", gfe.PST_TYPE::GERTH, 1024*1024, log(3.0)/log(1024.0*1024.0)); // 3
  gfe.add(3, "fanout4",gfe.PST_TYPE::GERTH, 1024*1024, log(4.5)/log(1024.0*1024.0)); // 4
  gfe.add(4, "fanout8",gfe.PST_TYPE::GERTH, 1024*1024, log(8.5)/log(1024.0*1024.0)); // 8
  gfe.add(5, "fanout16",gfe.PST_TYPE::GERTH, 1024*1024, log(16.5)/log(1024.0*1024.0)); // 16
  gfe.add(6, "fanout32",gfe.PST_TYPE::GERTH, 1024*1024, log(32.5)/log(1024.0*1024.0)); // 32
  gfe.add(7, "fanout128",gfe.PST_TYPE::GERTH, 1024*1024, log(128.5)/log(1024.0*1024.0)); // 128
  gfe.run();

  return 0;
}
