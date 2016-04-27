#include "base_experiment.hpp"
using namespace experiment;

class arge_buffer_size_experiment : public base_experiment {

  public:
  arge_buffer_size_experiment(const std::string &exp_name) : base_experiment(exp_name) {
  }
  ~arge_buffer_size_experiment() {}

  void run_experiment(run_instance instance) {
    stream data = get_data_stream("../data/insert_experiment");
    auto pst = PST_factory(instance.type, instance.buffer_size, instance.epsilon);
    size_t data_read = 0;
    size_t data_size = sizeof(point);
    size_t interval = 5 * 1024 * 1024; //10 mb intervals
    size_t print_interval = 1024*64;
    size_t stop_at = 1024 * 1024 * 100;

    restart_timers();

    while (!data.eof()) {
      point p = data.read();
      pst->insert(p);
      data_read += data_size;

      if (data_read % print_interval == 0) {
        std::cout << data_read/1024 << "KB" << std::endl;
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

  arge_buffer_size_experiment gfe("arge_buffer_size_experiment");
  //gfe.add(1, "buffer1024", common::PST_VARIANT::ARGE, 1024/8, 1);
  // gfe.add(2, "buffer2048", common::PST_VARIANT::ARGE, 1024/4, 1);
  gfe.add(3, "buffer4096", common::PST_VARIANT::ARGE, 1024/2, 1);
  gfe.add(4, "buffer8192", common::PST_VARIANT::ARGE, 1024, 1);
  gfe.add(5, "buffer16384", common::PST_VARIANT::ARGE, 1024*2, 1);
  gfe.add(6, "buffer32768", common::PST_VARIANT::ARGE, 1024*4, 1);
  gfe.run();
  gfe.plot();

  return 0;
}
