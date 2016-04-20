#ifndef RESULT_HPP
#define RESULT_HPP
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include "../common/definitions.hpp"
namespace experiment {
  
  class result {
    public:
    typedef common::MEASURE MEASURE;
    result() {
      for (int i = MEASURE::first+1; i < MEASURE::last; i++)
        result_map[i] = std::map<size_t, size_t>();
    }
    
    ~result() {}

    void add_measure(MEASURE m, size_t input_size, size_t result) {
      result_map[m][input_size] = result;
      input_sizes.insert(input_size);
    }

    std::string get_results() {
      std::string result = "#input\t";
      for (int i = MEASURE::first+1; i < MEASURE::last; i++) {
        result += common::MEASURE_to_string(static_cast<MEASURE>(i));
        result += "\t";
      }
      for (auto s : input_sizes) {
        result += "\n";
        result += std::to_string(s) + "\t";
        for (int i = MEASURE::first+1; i < MEASURE::last; i++) {
          result += std::to_string(result_map[i][s]) + "\t";
        }
      }
      return result;
    }

    private:
    std::map<int, std::map<size_t, size_t> > result_map;
    std::set<size_t> input_sizes;
  };


};
#endif
