#ifndef RESULT_HPP
#define RESULT_HPP
#include <map>
#include <set>
#include <vector>
#include <algorithm>
namespace experiment {
  
  class result {
    public:
    enum MEASURE {
      first,
      time, num_ios, L1, L2, L3, instr_count,
      page_faults,
      last
    };
    result() {
      for (int i = first+1; i < last; i++)
        result_map[i] = std::map<size_t, size_t>();
    }
    
    ~result() {}

    void add_measure(MEASURE m, size_t input_size, size_t result) {
      result_map[m][input_size] = result;
      input_sizes.insert(input_size);
    }

    std::string get_results() {
      std::string result = "#input\t";
      for (int i = first+1; i < last; i++) {
        result += MEASURE_to_string(static_cast<MEASURE>(i));
        result += "\t";
      }
      for (auto s : input_sizes) {
        result += "\n";
        result += std::to_string(s) + "\t";
        for (int i = first+1; i < last; i++) {
          result += std::to_string(result_map[i][s]) + "\t";
        }
      }
      return result;
    }

    std::string MEASURE_to_string(MEASURE m) {
      switch (m) {
      case MEASURE::time: return "time";
      case MEASURE::num_ios: return "ios";
      case MEASURE::L1: return "L1";
      case MEASURE::L2: return "L2";
      case MEASURE::L3: return "L3";
      case MEASURE::instr_count: return "ins ct";
      case MEASURE::page_faults: return "pfs";
      default: return "invalid measure";
      }

    }

    private:
    std::map<int, std::map<size_t, size_t> > result_map;
    std::set<size_t> input_sizes;
  };


};
#endif
