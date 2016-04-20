#ifndef BASE_EXPERIMENT_HPP
#define BASE_EXPERIMENT_HPP

#include <string>
#include <algorithm>
#include <map>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include "../common/point.hpp"
#include "../common/pst_interface.hpp"
#include "../common/test_lib.hpp"
#include "../common/utilities.hpp"
#include "../common/definitions.hpp"
#include "../stream/stream.hpp"
#include "../external/arge_pst.hpp"
#include "../external/buffered_pst.hpp"
#include "../internal/mysql_pst.hpp"
#include "../internal/boost_r_tree.hpp"
#include "../internal/dynamic_pst.hpp"
#include "result.hpp"

namespace experiment {

  class base_experiment {
  protected:
    typedef ext::external_priority_search_tree arge_pst;
    typedef ext::buffered_pst gerth_pst;
    typedef internal::boost_r_tree rtree;
    typedef internal::mysql_pst mysql_pst;
    typedef internal::dynamic_pst dyn_pst;
    typedef io::buffered_stream<point> stream;
    typedef common::PST_VARIANT PST_TYPE;

    struct run_instance {
      size_t id;
      std::string name;
      PST_TYPE type;
      size_t buffer_size;
      double epsilon;
      run_instance(size_t i, std::string n, PST_TYPE t, size_t bs, double eps) :
        id(i), name(n), type(t), buffer_size(bs), epsilon(eps) {}
    };

    std::string name;
    std::string start_time;

    
    std::string get_file_name(run_instance instance);
    std::string get_directory();
    const std::string current_date_time();
    stream get_data_stream(const std::string &file);
    result results[256];
    void add_result(size_t id, result::MEASURE m, size_t input_size, size_t res);
    void save_results(run_instance instance);
    std::vector<run_instance> run_instances;
    common::pst_interface * PST_factory(PST_TYPE type, size_t buffer_size, double epsilon);
    void plot_with_size(test::gnuplot &gp, run_instance instance, result::MEASURE m);
    std::string get_working_directory();
  public:
    base_experiment(const std::string &experiment_name);
    virtual ~base_experiment();
    void run();
    void add(size_t id, std::string name, PST_TYPE type, size_t buffer_size, double epsilon);
    virtual void plot();
    virtual void run_experiment(run_instance instance) = 0;
  };

  base_experiment::base_experiment(const std::string &experiment_name) {
    start_time = current_date_time();
    name = experiment_name;
    int r = system(("mkdir -p "+get_directory()).c_str());
    std::cout << "Creating result directory: " << get_directory() << std::endl;
    r++;
    std::cout << "Cleaning up folders..." << std::endl;
    for (int i = 0; i < 10000; i++) {
      util::remove_directory(std::to_string(i));
      util::remove_directory("c_"+std::to_string(i));
    }
  }

  base_experiment::~base_experiment() { }

  void base_experiment::add(size_t id, std::string name, PST_TYPE type, size_t buffer_size, double epsilon) {
    run_instances.push_back(run_instance(id, name, type, buffer_size, epsilon));
  }

  void base_experiment::run() {

    for (auto ri : run_instances) {
      std::cout << "Running " << ri.name << std::endl
                << "type: " << common::PST_VARIANT_to_string(ri.type) << std::endl
                << "buffer size: " << ri.buffer_size << std::endl
                << "epsilon: " <<  ri.epsilon << std::endl;
      run_experiment(ri);
    }
  }

  common::pst_interface * base_experiment::PST_factory(PST_TYPE type, size_t buffer_size, double epsilon) {
    switch (type) {
    case PST_TYPE::GERTH: return new gerth_pst(buffer_size, epsilon);
    case PST_TYPE::ARGE: return new arge_pst(buffer_size, epsilon);
    case PST_TYPE::MYSQL: return new mysql_pst(buffer_size, epsilon);
    case PST_TYPE::RTREE: return new rtree(buffer_size, epsilon);
    case PST_TYPE::INTERNAL: return new dyn_pst(buffer_size, epsilon);
    default: return 0;
    };
  }

  void base_experiment::save_results(run_instance instance) {
    std::ofstream of;
    of.open(get_file_name(instance));
    of << results[instance.id].get_results() << std::endl;
    of.close();
  }

  void base_experiment::plot_with_size(test::gnuplot &gp, run_instance instance, result::MEASURE m) {
    gp.add_line(instance.name, instance.type, get_working_directory()+"/"+get_file_name(instance), 1, m);
  }

  void base_experiment::plot() {
    for (int i = common::MEASURE::first+1; i < common::MEASURE::last; i++) {
      test::gnuplot gp;
      std::string output = common::MEASURE_to_string(static_cast<common::MEASURE>(i));
      gp.set_output(get_directory()+"/"+output);
      gp.set_xlabel(common::XLABEL_to_string(common::XLABEL::input_size));
      gp.set_ylabel(common::MEASURE_to_label(static_cast<common::MEASURE>(i)));
      for (auto ri : run_instances) plot_with_size(gp, ri, static_cast<common::MEASURE>(i));
      gp.output_script(get_directory()+"/plot_"+output+".sh");
      gp.output_plot(get_directory()+"/plot_"+output+".sh");
    }
  }

  void base_experiment::add_result
  (size_t id, result::MEASURE m, size_t input_size, size_t res) {
    results[id].add_measure(m, input_size, res);
  }

  base_experiment::stream base_experiment::get_data_stream(const std::string &file) {
    stream res(1337);
    res.open(file);
    return res;
  }

  const std::string base_experiment::current_date_time() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
  }

  std::string base_experiment::get_directory() {
    return name+"_results/"+start_time;
  }

  std::string base_experiment::get_file_name(run_instance instance) {
    switch (instance.type) {
    case PST_TYPE::ARGE: return get_directory()+"/arge_"+instance.name;
    case PST_TYPE::GERTH: return get_directory()+"/gerth_"+instance.name;
    case PST_TYPE::MYSQL: return get_directory()+"/mysql_"+instance.name;
    case PST_TYPE::RTREE: return get_directory()+"/rtree_"+instance.name;
    case PST_TYPE::INTERNAL: return get_directory()+"/internal_"+instance.name;
    default: return "invalid";
    }
  }

  std::string base_experiment::get_working_directory() {
    char* dir = get_current_dir_name();
    std::string res = std::string(dir);
    delete dir;
    return res;
  }

};


#endif
