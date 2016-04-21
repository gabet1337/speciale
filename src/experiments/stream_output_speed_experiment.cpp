#include "base_experiment.hpp"
using namespace experiment;

class stream_output_speed_experiment : public base_experiment {

  public:
  stream_output_speed_experiment(const std::string &exp_name) : base_experiment(exp_name) {
  }
  ~stream_output_speed_experiment() {}

  void run_experiment(run_instance instance) {
    auto pst = PST_factory(instance.type, instance.buffer_size, instance.epsilon);
    size_t data_written = 0;
    size_t data_size = sizeof(int);
    size_t interval = 16 * 1024 * 1024; //32 mb intervals
    size_t print_interval = 1024 * 1024 * 50;
    size_t stop_at = 1024 * 1024 * 1024; //1GB!
    restart_timers();
    point p(0,0);
    while (true) {
      pst->insert(p);
      data_written += data_size;

      if (data_written % print_interval == 0) {
        std::cout << data_written/(1024*1024) << "MB" << std::endl;
      }

      if (data_written % interval == 0) {
        measure_everything(instance.id, data_written/(1024*1024));

        save_results(instance);
        if (data_written >= stop_at) break;
      }
    }
    delete pst;
  }

  void plot() {
    base_experiment::plot();
  }

};

int main() {

  stream_output_speed_experiment sose("stream_output_speed_experiment");
  sose.add(1, "mmap", common::PST_VARIANT::MMAP_STREAM, 4096, 1);
  sose.add(2, "buffered", common::PST_VARIANT::BUFFERED_STREAM, 4096, 1);
  sose.add(3, "file", common::PST_VARIANT::FILE_STREAM, 4096, 1);
  // sose.add(4, "rw", common::PST_VARIANT::READ_WRITE_STREAM, 4096, 1);
  sose.run();
  sose.plot();

  return 0;
}
