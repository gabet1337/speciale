#include "base_experiment.hpp"
using namespace experiment;

class stream_input_speed_experiment : public base_experiment {
  typedef unsigned long long ull;
  public:
  stream_input_speed_experiment(const std::string &exp_name) : base_experiment(exp_name) {
  }
  ~stream_input_speed_experiment() {}

  void run_experiment(run_instance instance) {
    auto pst = PST_factory(instance.type, instance.buffer_size, instance.epsilon);
    ull data_read = 0;
    ull data_size = sizeof(int);
    ull interval = 64 * 1024 * 1024; //64 mb intervals
    ull print_interval = 1024 * 1024 * 50;
    ull stop_at = 1024ULL * 1024ULL * 1024ULL * 5ULL; //1GB!
    restart_timers();
    point p(0,0);
    while (true) {
      pst->remove(p);
      data_read += data_size;

      if (data_read % print_interval == 0) {
        std::cout << data_read/(1024*1024) << "MB" << std::endl;
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

  stream_input_speed_experiment sise("stream_input_speed_experiment");
  sise.add(1, "mmap", common::PST_VARIANT::MMAP_STREAM, 4096, 1);
  sise.add(2, "buffered", common::PST_VARIANT::BUFFERED_STREAM, 4096, 1);
  sise.add(3, "file", common::PST_VARIANT::FILE_STREAM, 4096, 1);
  //sise.add(4, "rw", common::PST_VARIANT::READ_WRITE_STREAM, 4096, 1);
  sise.run();
  sise.plot();

  return 0;
}
