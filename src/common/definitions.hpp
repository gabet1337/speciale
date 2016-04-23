#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

#include <climits>
#include <string>

#define INF INT_MAX

#define GERTH_BUFFER_SIZE 1024*1024*4
#define GERTH_FANOUT 2.0
#define GERTH_EPSILON log(GERTH_FANOUT+0.5)/log((double)GERTH_BUFFER_SIZE)
#define ARGE_BUFFER_SIZE 1024*1024*8
#define ARGE_EPSILON 1
#define INTERNAL_BUFFER_SIZE 4096
#define INTERNAL_EPSILON 1
#define MYSQL_BUFFER_SIZE 4096
#define MYSQL_EPSILON 1
#define RTREE_BUFFER_SIZE 4096
#define RTREE_EPSILON 1


namespace common {
  enum PST_VARIANT {
    start = 0,
    ARGE = 1,
    GERTH = 2,
    RTREE = 3,
    MYSQL = 4,
    INTERNAL = 5,
    READ_WRITE_STREAM = 6,
    MMAP_STREAM = 7,
    BUFFERED_STREAM = 8,
    FILE_STREAM = 9,
    end = 10
  };

  std::string PST_VARIANT_to_string(PST_VARIANT type) {
    switch (type) {
    case PST_VARIANT::ARGE: return "Arge";
    case PST_VARIANT::GERTH: return "Gerth";
    case PST_VARIANT::MYSQL: return "MySQL";
    case PST_VARIANT::RTREE: return "RTree";
    case PST_VARIANT::INTERNAL: return "Internal";
    case PST_VARIANT::MMAP_STREAM: return "mmap stream";
    case PST_VARIANT::READ_WRITE_STREAM: return "read write stream";
    case PST_VARIANT::FILE_STREAM: return "file stream";
    case PST_VARIANT::BUFFERED_STREAM: return "buffered stream";
    default: return "invalid";
    }
  }

  enum MEASURE {
    first = 1,
    time = 2, num_ios = 3,
    page_faults = 4,
    time_ms = 5,
    //L1 = 5, L2 = 6, L3 = 7, instr_count = 8,
    last = 6
  };

  std::string MEASURE_to_string(MEASURE m) {
    switch (m) {
    case MEASURE::time: return "time";
    case MEASURE::time_ms: return "timems";
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
    case XLABEL::input_size_in_thousands: return "N (input size in Kb)";
    case XLABEL::input_size_in_millions: return "N (input size in Mb)";
    default: return "invalid label";
    }
  }

  std::string MEASURE_to_label(MEASURE m) {
    switch (m) {
    case MEASURE::time: return "Time (s)";
    case MEASURE::time_ms: return "Time (ms)";
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
