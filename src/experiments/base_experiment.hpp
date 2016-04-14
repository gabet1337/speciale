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
#include "../stream/stream.hpp"
#include "../external/arge_pst.hpp"
#include "../external/buffered_pst.hpp"
#include "../internal/mysql_pst.hpp"
#include "../internal/boost_r_tree.hpp"
//#include "../internal/dynamic_pst.hpp"

namespace experiment {

  class base_experiment {
  protected:
    typedef ext::external_priority_search_tree arge_pst;
    typedef ext::buffered_pst gerth_pst;
    typedef internal::boost_r_tree rtree;
    typedef internal::mysql_pst mysql_pst;

    std::string name;
    std::string start_time;
    enum PST_TYPE { ARGE = 1, GERTH = 2, MYSQL = 3, RTREE = 4, INTERNAL = 5 };
    std::string get_file_name(PST_TYPE type);
    std::string get_directory();
    const std::string current_date_time();

    bool ignored[256];

  public:
    base_experiment(const std::string &experiment_name);
    virtual ~base_experiment();
    void run();
    void ignore(PST_TYPE type);
    virtual void plot();
    virtual void run_experiment(const common::pst_interface& pst) = 0;

  };

  base_experiment::base_experiment(const std::string &experiment_name) {
    start_time = current_date_time();
    name = experiment_name;
    int r = system(("mkdir -p "+get_directory()).c_str());
    for (int i = 0; i < 256; i++) ignored[i] = false;
  }

  base_experiment::~base_experiment() { }

  void base_experiment::run() {

    if (!ignored[PST_TYPE::ARGE]) {
      run_experiment(arge_pst(1337, 0.5));
    }

  }

  void base_experiment::plot() {
    
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
    return name+"/"+start_time;
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


};


#endif
