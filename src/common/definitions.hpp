#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

#include <climits>
#include <string>

#define INF INT_MAX

namespace common {
  enum PST_VARIANT {
    ARGE = 1,
    GERTH = 2,
    RTREE = 3,
    MYSQL = 4,
    INTERNAL = 5
  };

  std::string PST_VARIANT_to_string(PST_VARIANT type) {
    switch (type) {
    case PST_VARIANT::ARGE: return "Arge";
    case PST_VARIANT::GERTH: return "Gerth";
    case PST_VARIANT::MYSQL: return "MySQL";
    case PST_VARIANT::RTREE: return "RTree";
    case PST_VARIANT::INTERNAL: return "Internal";
    default: return "invalid";
    }
  }

  enum MEASURE {
    first = 1,
    time = 2, num_ios = 3, L1 = 4,
    L2 = 5, L3 = 6, instr_count = 7,
    page_faults = 8,
    last = 9
  };

  std::string MEASURE_to_string(MEASURE m) {
    switch (m) {
    case MEASURE::time: return "time";
    case MEASURE::num_ios: return "ios";
    case MEASURE::L1: return "L1";
    case MEASURE::L2: return "L2";
    case MEASURE::L3: return "L3";
    case MEASURE::instr_count: return "ins_ct";
    case MEASURE::page_faults: return "pfs";
    default: return "invalid measure";
    }

  }
  
};

#endif
