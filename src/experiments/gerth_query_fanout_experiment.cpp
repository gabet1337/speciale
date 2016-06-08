#include "base_experiment.hpp"
using namespace experiment;

class gerth_query_fanout_experiment : public base_experiment {

  public:
  gerth_query_fanout_experiment(const std::string &exp_name) : base_experiment(exp_name) {
  }
  ~gerth_query_fanout_experiment() {}

  void run_experiment(run_instance instance) {
    typedef unsigned long long ull;
    stream data = get_data_stream("../data/query_experiment");
    auto pst = PST_factory(instance.type, instance.buffer_size, instance.epsilon);
    ull data_read = 0;
    ull data_size = sizeof(point);
    ull interval = 25 * 1024 * 1024; // 25 MB intervals
    ull print_interval = 1024 * 1024;
    ull stop_at = 512ULL * 1024ULL * 1024ULL * 1ULL; // 1 GB

    restart_timers();

    while (!data.eof()) {
      point p = data.read();
      pst->insert(p);
      data_read += data_size;
      if (data_read % print_interval == 0) {
        std::cout << "inserted " << data_read/(1024*1024) << "Mb" << std::endl;
      }
      if (data_read % interval == 0) {
	pst->report(107374182, 214748364, 0, "query_result1");
	pst->report(472446402, 837518622, 429496729, "query_result2");
	pst->report(1009317314, 1245540515, 1288490188, "query_result3");
	pst->report(1460288879, 1760936590, 858993458, "query_result4");
	pst->report(1868310772, 2040109464, 1717986917, "query_result5");
	util::remove_directory("query_result1");
	util::remove_directory("query_result2");
	util::remove_directory("query_result3");
	util::remove_directory("query_result4");
	util::remove_directory("query_result5");
        restart_timers();
        for (int i = 0; i < 1; i++) {
          pst->report(107374182, 214748364, 0, "query_result1");
          pst->report(472446402, 837518622, 429496729, "query_result2");
          pst->report(1009317314, 1245540515, 1288490188, "query_result3");
          pst->report(1460288879, 1760936590, 858993458, "query_result4");
          pst->report(1868310772, 2040109464, 1717986917, "query_result5");
          util::remove_directory("query_result1");
          util::remove_directory("query_result2");
          util::remove_directory("query_result3");
          util::remove_directory("query_result4");
          util::remove_directory("query_result5");
        }
        measure_everything(instance.id, data_read/(1024*1024));
        save_results(instance);
      }
      
      if (data_read >= stop_at) break;
    
    }
    delete pst;
  }

  void plot() {
    base_experiment::plot();
  }

};

int main() {

  gerth_query_fanout_experiment gfe("gerth_query_fanout_experiment");
  //gfe.add(1, "fanout2", common::PST_VARIANT::GERTH, 1024*1024, log(2.5)/log(1024.0*1024.0));
  //gfe.add(2, "fanout4", common::PST_VARIANT::GERTH, 1024*1024, log(4.5)/log(1024.0*1024.0));
  gfe.add(3, "fanout16", common::PST_VARIANT::GERTH, 1024*1024/16, log(16.5)/log(1024.0*1024.0/16.0));
  //gfe.add(4, "fanout16", common::PST_VARIANT::GERTH, 1024*1024, log(16.5)/log(1024.0*1024.0));
  //gfe.add(5, "fanout32", common::PST_VARIANT::GERTH, 1024*1024, log(32.5)/log(1024.0*1024.0));
  gfe.run();
  gfe.plot();

  return 0;
}
