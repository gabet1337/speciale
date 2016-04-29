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
    size_t interval = 10 * 1024 * 1024; //10 mb intervals
    size_t print_interval = 1024 * 1024;
    test::clock exp_time;
    exp_time.start();
    long long one_day = 60*60*24;
    restart_timers();
    while (!data.eof()) {
      point p = data.read();
      pst->insert(p);
      data_read += data_size;
      if (exp_time.elapsed() > one_day) {
        std::cout << "TIMES UP!" << std::endl;
        measure_everything(instance.id, data_read/(1024*1024));
        save_results(instance);
        break;
      }
      if (data_read % interval == 0) {
        measure_everything(instance.id, data_read/(1024*1024));
        save_results(instance);
      }

      if (data_read % print_interval == 0) {
        std::cout << data_read/(1024*1024) << "MB" << std::endl;
      }
    }
    delete pst;
  }

};

int main() {

  insert_experiment ie("insert_experiment");
  ie.add(3, "Internal", common::PST_VARIANT::INTERNAL, INTERNAL_BUFFER_SIZE, INTERNAL_EPSILON);
  ie.add(4, "Boost R-tree", common::PST_VARIANT::RTREE, RTREE_BUFFER_SIZE, RTREE_EPSILON);
  ie.add(5, "MySQL", common::PST_VARIANT::MYSQL, MYSQL_BUFFER_SIZE, MYSQL_EPSILON);
  ie.add(1, "Gerth", common::PST_VARIANT::GERTH, GERTH_BUFFER_SIZE, GERTH_EPSILON);
//  ie.add(2, "Arge", common::PST_VARIANT::ARGE, ARGE_BUFFER_SIZE, ARGE_EPSILON);
  ie.run();
  ie.plot();

  return 0;
}
