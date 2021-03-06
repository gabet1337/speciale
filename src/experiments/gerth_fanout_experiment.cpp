#include "base_experiment.hpp"
using namespace experiment;

class gerth_fanout_experiment : public base_experiment {

  public:
  gerth_fanout_experiment(const std::string &exp_name) : base_experiment(exp_name) {
  }
  ~gerth_fanout_experiment() {}

  void run_experiment(run_instance instance) {
    typedef unsigned long long ull;
    stream data = get_data_stream("../data/insert_experiment");
    auto pst = PST_factory(instance.type, instance.buffer_size, instance.epsilon);
    ull data_read = 0;
    ull data_size = sizeof(point);
    ull interval = 25 * 1024 * 1024; //25 mb intervals
    ull print_interval = 1024 * 1024;
    ull stop_at = 1024ULL * 1024ULL * 1024ULL * 5ULL;

    restart_timers();

    while (!data.eof()) {
      point p = data.read();
      pst->insert(p);
      data_read += data_size;

      if (data_read % print_interval == 0) {
        std::cout << data_read/print_interval << "MB" << std::endl;
      }

      if (data_read % interval == 0) {
        measure_everything(instance.id, data_read/(1024*1024));

        save_results(instance);
        if (data_read >= stop_at) break;
      }
    }
    delete pst;
  }

  void plot() {
    base_experiment::plot();
  }

};

int main() {

  gerth_fanout_experiment gfe("gerth_fanout_experiment");
  gfe.add(1, "fanout8", common::PST_VARIANT::GERTH, 1024*1024, log(8.5)/log(1024.0*1024.0));
  gfe.add(2, "fanout16", common::PST_VARIANT::GERTH, 1024*1024, log(16.5)/log(1024.0*1024.0));
  gfe.add(3, "fanout32", common::PST_VARIANT::GERTH, 1024*1024, log(32.5)/log(1024.0*1024.0));
  gfe.add(4, "fanout64", common::PST_VARIANT::GERTH, 1024*1024, log(64.5)/log(1024.0*1024.0));
  gfe.add(5, "fanout128", common::PST_VARIANT::GERTH, 1024*1024, log(128.5)/log(1024.0*1024.0));

  gfe.run();
  gfe.plot();

  return 0;
}
