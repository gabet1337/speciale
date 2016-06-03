#include "base_experiment.hpp"
#include "../common/definitions.hpp"

using namespace experiment;

class query_experiment_report_50mb : public base_experiment {

  public:
  query_experiment_report_50mb(const std::string &exp_name) : base_experiment(exp_name) {}
  ~query_experiment_report_50mb() {}

  void run_experiment(run_instance instance) {
    typedef unsigned long long ull;
    stream data = get_data_stream("../data/query_experiment_report_50mb");
    auto pst = PST_factory(instance.type, instance.buffer_size, instance.epsilon);
    ull data_read = 0;
    ull data_size = sizeof(point);
    ull interval = 10 * 1024 * 1024; // 10 mb intervals
    ull print_interval = 1024 * 1024;
    //test::clock exp_time;
    //exp_time.start();
    long long one_day = 60*60*24;

    while (!data.eof()) {
      point p = data.read();
      pst->insert(p);
      data_read += data_size;
      // if (exp_time.elapsed() > one_day) {
      //   //std::cout << "TIMES UP!" << std::endl;
      //   //measure_everything(instance.id, data_read/(1024*1024));
      //   //save_results(instance);
      //   //break;
      // }

      if (data_read % print_interval == 0) {
        std::cout << "inserted " << data_read/(1024*1024) << "Mb" << std::endl;
      }
      if (data_read > 1024*1024*1024) {
        std::cout << "INSERTED " << data_read/(1024*1024) << "Mb. TIMES UP!" << std::endl;
        break;
      }
    }

    pst->create_index();
    util::remove_directory("../data/test/query_result_1");
    pst->report(-INF, INF, -INF, "../data/test/query_result_1");
    util::remove_directory("../data/test/query_result_1");
    
    // Start reporting
    for (size_t i = 42949672; i < INF; i += 42949672) {
      util::remove_directory(std::string("../data/test/query_result_").
                             append(std::to_string(i/42949672)));
      restart_timers();
      for (int j = 0; j < 1; j++) {
        pst->report(966367641, 1181116005, INF-i, std::string("../data/test/query_result_").
                    append(std::to_string(i/42949672)));
        util::remove_directory(std::string("../data/test/query_result_").
                               append(std::to_string(i/42949672)));
      }
      std::cout << "reported " << i/42949672 << " Mb" << std::endl;
      measure_everything(instance.id, i/42949672);
      save_results(instance);

      // if (exp_time.elapsed() > one_day) {
      //   std::cout << "TIMES UP!" << std::endl;
      //   //measure_everything(instance.id, data_read/(1024*1024));
      //   //save_results(instance);
      //   break;
      // }
      
    }
    
    delete pst;

  }

};

int main() {

  query_experiment_report_50mb qe("query_experiment_report_50mb");
  //qe.add(1, "Gerth", common::PST_VARIANT::GERTH, GERTH_BUFFER_SIZE, GERTH_EPSILON);
  //qe.add(3, "Internal", common::PST_VARIANT::INTERNAL, INTERNAL_BUFFER_SIZE, INTERNAL_EPSILON);
  //qe.add(4, "Boost R-tree", common::PST_VARIANT::RTREE, RTREE_BUFFER_SIZE, RTREE_EPSILON);
  // qe.add(2, "Arge", common::PST_VARIANT::ARGE, ARGE_BUFFER_SIZE, ARGE_EPSILON);
  //qe.add(6, "libspatial", common::PST_VARIANT::SPATIAL, SPATIAL_BUFFER_SIZE, SPATIAL_EPSILON);
  qe.add(5, "MySQL", common::PST_VARIANT::MYSQL, MYSQL_BUFFER_SIZE, MYSQL_EPSILON);
  qe.run();
  qe.plot();

  return 0;
}
