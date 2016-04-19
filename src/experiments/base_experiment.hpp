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

    std::string name;
    std::string start_time;
    enum PST_TYPE { ARGE = 1, GERTH = 2, MYSQL = 3, RTREE = 4, INTERNAL = 5 };
    std::string get_file_name(PST_TYPE type);
    std::string get_directory();
    const std::string current_date_time();
    stream get_data_stream(const std::string &file);
    bool ignored[256];
    result results[256];
    void add_result(PST_TYPE type, result::MEASURE m, size_t input_size, size_t res);
    void save_results(PST_TYPE type);
    std::string PST_TYPE_to_string(PST_TYPE type);
  public:
    base_experiment(const std::string &experiment_name);
    virtual ~base_experiment();
    void run();
    void ignore(PST_TYPE type);
    virtual void plot();
    virtual void run_experiment(common::pst_interface& pst, PST_TYPE type) = 0;
  };

  base_experiment::base_experiment(const std::string &experiment_name) {

    start_time = current_date_time();
    name = experiment_name;
    int r = system(("mkdir -p "+get_directory()).c_str());
    std::cout << "Creating result directory: " << get_directory() << std::endl;
    r++;
    for (int i = 0; i < 256; i++) ignored[i] = false;
    std::cout << "Cleaning up folders..." << std::endl;
    for (int i = 0; i < 10000; i++) {
      util::remove_directory(std::to_string(i));
      util::remove_directory("c_"+std::to_string(i));
    }
  }

  base_experiment::~base_experiment() { }

  void base_experiment::run() {

    if (!ignored[PST_TYPE::GERTH]) {
      gerth_pst pst(1024*1024*50, 0.0547);
      pst.set_global_rebuild_configuration(gerth_pst::global_rebuild_configuration(INF,0.5));
      run_experiment(pst, PST_TYPE::GERTH);
      save_results(PST_TYPE::GERTH);
    }

    if (!ignored[PST_TYPE::ARGE]) {
      arge_pst pst(1337, 0.5);
      run_experiment(pst, PST_TYPE::ARGE);
    }

    if (!ignored[PST_TYPE::MYSQL]) {
      mysql_pst pst(1337, 0.5);
      run_experiment(pst, PST_TYPE::MYSQL);
    }

    if (!ignored[PST_TYPE::RTREE]) {
      rtree pst(1337, 0.5);
      run_experiment(pst, PST_TYPE::RTREE);
    }

    if (!ignored[PST_TYPE::INTERNAL]) {
      dyn_pst pst(1337, 0.5);
      run_experiment(pst, PST_TYPE::INTERNAL);
    }
  }

  void base_experiment::save_results(PST_TYPE type) {
    std::ofstream of;
    of.open(get_file_name(type));
    of << results[type].get_results();
    of.close();
  }

  void base_experiment::plot() {
    
  }

  void base_experiment::add_result
  (PST_TYPE type, result::MEASURE m, size_t input_size, size_t res) {
    results[type].add_measure(m, input_size, res);
  }

  base_experiment::stream base_experiment::get_data_stream(const std::string &file) {
    stream res(1337);
    res.open(file);
    return res;
  }

  void base_experiment::ignore(PST_TYPE type) {
    ignored[type] = true;
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

  std::string base_experiment::get_file_name(PST_TYPE type) {
    switch (type) {
    case PST_TYPE::ARGE: return get_directory()+"/arge_"+name;
    case PST_TYPE::GERTH: return get_directory()+"/gerth_"+name;
    case PST_TYPE::MYSQL: return get_directory()+"/mysql_"+name;
    case PST_TYPE::RTREE: return get_directory()+"/rtree_"+name;
    case PST_TYPE::INTERNAL: return get_directory()+"/internal_"+name;
    default: return "invalid";
    }
  }

  std::string base_experiment::PST_TYPE_to_string(PST_TYPE type) {
    switch (type) {
    case PST_TYPE::ARGE: return "Arge";
    case PST_TYPE::GERTH: return "Gerth";
    case PST_TYPE::MYSQL: return "MySQL";
    case PST_TYPE::RTREE: return "RTree";
    case PST_TYPE::INTERNAL: return "Internal";
    default: return "invalid";
    }
  }

};


#endif
