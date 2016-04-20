#include "base_experiment.hpp"
using namespace experiment;

class gerth_buffer_size_experiment : public base_experiment {

  public:
  gerth_buffer_size_experiment(const std::string &exp_name) : base_experiment(exp_name) {
  }
  ~gerth_buffer_size_experiment() {}

  void run_experiment(run_instance instance) {
    stream data = get_data_stream("../data/insert_experiment");
    auto pst = PST_factory(instance.type, instance.buffer_size, instance.epsilon);
    size_t data_read = 0;
    size_t data_size = sizeof(point);
    size_t interval = 10 * 1024 * 1024; //10 mb intervals
    size_t print_interval = 1024 * 1024;
    size_t stop_at = 1024 * 1024 * 500;

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

  gerth_buffer_size_experiment gfe("gerth_buffer_size_experiment");
  gfe.add(1, "buffer1MB", common::PST_VARIANT::GERTH, 1024*1024/8, log(2.5)/log(1024.0*1024.0/8.0)); //fanout 2
  gfe.add(2, "buffer2MB", common::PST_VARIANT::GERTH, 1024*1024/4, log(2.5)/log(1024.0*1024.0/4.0)); //fanout 2
  gfe.add(3, "buffer4MB", common::PST_VARIANT::GERTH, 1024*1024/2, log(2.5)/log(1024.0*1024.0/2.0)); //fanout 2
  gfe.add(4, "buffer8MB", common::PST_VARIANT::GERTH, 1024*1024, log(2.5)/log(1024.0*1024.0)); //fanout 2
  gfe.add(5, "buffer16MB", common::PST_VARIANT::GERTH, 1024*1024*2, log(2.5)/log(1024.0*1024.0*2.0)); //fanout 2
  gfe.run();
  gfe.plot();

  return 0;
}
