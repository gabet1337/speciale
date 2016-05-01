#include "base_experiment.hpp"
using namespace experiment;

class query_experiment : public base_experiment {

  public:
  query_experiment(const std::string &exp_name) : base_experiment(exp_name) {}
  ~query_experiment() {}

  void run_experiment(run_instance instance) {
    typedef unsigned long long ull;
    stream data = get_data_stream("../data/query_experiment");
    auto pst = PST_factory(instance.type, instance.buffer_size, instance.epsilon);
    ull data_read = 0;
    ull data_size = sizeof(point);
    ull interval = 10 * 1024 * 1024; // 10 mb intervals
    ull print_interval = 1024 * 1024;
    test::clock exp_time;
    exp_time.start();
    long long one_day = 60*60*24;

    while (!data.eof()) {
      point p = data.read();
      pst->insert(p);
      data_read += data_size;
      if (exp_time.elapsed() > one_day) {
        std::cout << "TIMES UP!" << std::endl;
        //measure_everything(instance.id, data_read/(1024*1024));
        //save_results(instance);
        break;
      }
      if (data_read % interval == 0) {
        restart_timers();
        pst->report(107374182, 214748364, 0, "query_result1");
        pst->report(472446402, 837518622, 429496729, "query_result2");
        pst->report(1009317314, 1245540515, 1288490188, "query_result3");
        pst->report(1460288879, 1760936590, 858993458, "query_result4");
        pst->report(1868310772, 2040109464, 1717986917, "query_result5");
        measure_everything(instance.id, data_read/(1024*1024));
        save_results(instance);
        util::remove_directory("query_result1");
        util::remove_directory("query_result2");
        util::remove_directory("query_result3");
        util::remove_directory("query_result4");
        util::remove_directory("query_result5");
      }

      if (data_read % print_interval == 0) {
        std::cout << data_read/(1024*1024) << "MB" << std::endl;
      }
    }
    delete pst;
  }

};

int main() {

  query_experiment qe("query_experiment");
  qe.add(5, "MySQL", common::PST_VARIANT::MYSQL, MYSQL_BUFFER_SIZE, MYSQL_EPSILON);
  qe.add(3, "Internal", common::PST_VARIANT::INTERNAL, INTERNAL_BUFFER_SIZE, INTERNAL_EPSILON);
  qe.add(4, "Boost R-tree", common::PST_VARIANT::RTREE, RTREE_BUFFER_SIZE, RTREE_EPSILON);
  qe.add(1, "Gerth", common::PST_VARIANT::GERTH, GERTH_BUFFER_SIZE, GERTH_EPSILON);
  qe.add(2, "Arge", common::PST_VARIANT::ARGE, ARGE_BUFFER_SIZE, ARGE_EPSILON);
  qe.add(6, "libspatial", common::PST_VARIANT::SPATIAL, SPATIAL_BUFFER_SIZE, SPATIAL_EPSILON);
  qe.run();
  qe.plot();

  return 0;
}
