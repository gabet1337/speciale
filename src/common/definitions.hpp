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
    time = 2, num_ios = 3,
    page_faults = 4,
    //L1 = 5, L2 = 6, L3 = 7, instr_count = 8,
    last = 9
  };

  std::string MEASURE_to_string(MEASURE m) {
    switch (m) {
    case MEASURE::time: return "time";
    case MEASURE::num_ios: return "ios";
    case MEASURE::page_faults: return "pfs";
    // case MEASURE::L1: return "L1";
    // case MEASURE::L2: return "L2";
    // case MEASURE::L3: return "L3";
    // case MEASURE::instr_count: return "ins_ct";
    default: return "invalid measure";
    }
  }

  enum XLABEL {
    input_size,
    input_size_in_thousands,
    input_size_in_millions
  };

  std::string XLABEL_to_string(XLABEL x) {
    switch (x) {
    case XLABEL::input_size: return "N (input size)";
    case XLABEL::input_size_in_thousands: return "N/1000 (input size)";
    case XLABEL::input_size_in_millions: return "N/10^6 (input size)";
    default: return "invalid label";
    }
  }

  std::string MEASURE_to_label(MEASURE m) {
    switch (m) {
    case MEASURE::time: return "Time (s)";
    case MEASURE::num_ios: return "I/Os";
    case MEASURE::page_faults: return "page faults";
    // case MEASURE::L1: return "L1 cache accesses";
    // case MEASURE::L2: return "L2 cache accesses";
    // case MEASURE::L3: return "L3 cache accesses";
    // case MEASURE::instr_count: return "instruction count";
    default: return "invalid measure";
    }
  }
  
};

#endif
