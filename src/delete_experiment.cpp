#include "base_experiment.hpp"
using namespace experiment;

class delete_experiment : public base_experiment {

  public:
  delete_experiment(const std::string &exp_name) : base_experiment(exp_name) {}
  ~delete_experiment() {}

  void run_experiment(run_instance instance) {
    typedef unsigned long long ull;
    stream data = get_data_stream("../data/insert_experiment");
    auto pst = PST_factory(instance.type, instance.buffer_size, instance.epsilon);
    ull data_read = 0;
    ull data_size = sizeof(point);
    ull interval = 10 * 1024 * 1024; //10 mb intervals
    ull print_interval = 1024 * 1024;
    test::clock exp_time;
    exp_time.start();
    long long one_day = 60*60*24;
    restart_timers();
    while (!data.eof()) {
      point p = data.read();
      pst->insert(p);
      data_read += data_size;
      if (exp_time.elapsed() > one_day) {
        std::cout << "TIMES UP! NO MORE INSERTING IS DONE..." << std::endl;
        break;
      }
      if (data_read % print_interval == 0) {
	std::cout << data_read/(1024*1024) << "MB" << std::endl;
      }
    }
    data.seek(0, SEEK_SET);
    ull data_left = data_read;
    while (data_left > 0) {
      point p = data.read();
      pst->remove(p);
      data_left -= data_size;
      if (data_left % print_interval == 0) {
	std::cout << data_left/(1024*1024) << "MB LEFT" << std::endl;
      }
    }
    data.close();
  }
  
  delete pst;
};
  

int main() {

  delete_experiment de("delete_experiment");
  de.add(5, "MySQL", common::PST_VARIANT::MYSQL, MYSQL_BUFFER_SIZE, MYSQL_EPSILON);
  de.add(3, "Internal", common::PST_VARIANT::INTERNAL, INTERNAL_BUFFER_SIZE, INTERNAL_EPSILON);
  de.add(4, "Boost R-tree", common::PST_VARIANT::RTREE, RTREE_BUFFER_SIZE, RTREE_EPSILON);
  de.add(1, "Gerth", common::PST_VARIANT::GERTH, GERTH_BUFFER_SIZE, GERTH_EPSILON);
  de.add(2, "Arge", common::PST_VARIANT::ARGE, ARGE_BUFFER_SIZE, ARGE_EPSILON);
  de.add(6, "libspatial", common::PST_VARIANT::SPATIAL, SPATIAL_BUFFER_SIZE, SPATIAL_EPSILON);
  de.run();
  de.plot();

  return 0;
}
