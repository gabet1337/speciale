#ifndef TEST_LIB
#define TEST_LIB
#include <chrono>
#include <random>
#include <limits.h>
#include <exception>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include "point.hpp"
#include "../stream/stream.hpp"
#include "definitions.hpp"
#include "utilities.hpp"

namespace test {
  class clock {
    typedef std::chrono::high_resolution_clock hsc;
    typedef hsc::time_point tp;
  public:
    void start();
    void stop();
    long long elapsed();
    long long count();
  private:
    tp s,e;
  };

  long long clock::elapsed() {
    auto n = hsc::now();
    using namespace std;
    using namespace std::chrono;
    return duration_cast<seconds>(n-s).count();
  }

  void clock::start() {
    s = hsc::now();
  }

  void clock::stop() {
    e = hsc::now();
  }

  long long clock::count() {
    using namespace std;
    using namespace std::chrono;
    return duration_cast<seconds>(e-s).count();
  }

  class random {

  public:
    random();
    ~random();
    int next();
    int next(int below);
  private:
    long count;
    std::random_device *rd;
    std::mt19937 *gen;
    std::uniform_int_distribution<int> *dis;
  };

  random::random() {
    rd = new std::random_device();
    gen = new std::mt19937((*rd)());
    dis = new std::uniform_int_distribution<int>(0, INT_MAX);
    count = 0;
  }

  random::~random() {
    delete rd;
    delete gen;
    delete dis;
  }

  int random::next() {
    if (count > 1000000) {
      count = 0;
      delete rd;
      delete gen;
      delete dis;
      rd = new std::random_device();
      gen = new std::mt19937((*rd)());
      dis = new std::uniform_int_distribution<int>(0, INT_MAX);
    }
    return (*dis)((*gen));
  }

  int random::next(int below) {
    if (!below) return 0;
    return next()%below;
  }

  class counter {
  public:
    counter();
    ~counter();
    void inc();
    void dec();
    long long count();
  private:
    long long c;
  };
  counter::counter() { c = 0; }
  counter::~counter() {}
  void counter::inc() { c++; }
  void counter::dec() { c--; }
  long long counter::count() { return c; }

  class gnuplot {
  public:
    gnuplot() {}
    ~gnuplot() {}
    enum STYLE { ARGE = 1, GERTH = 2, RTREE = 3, MYSQL = 4, INTERNAL = 5 };
    enum KEY_POS { BOTTOM_LEFT, BOTTOM_RIGHT, TOP_LEFT, TOP_RIGHT };
    std::string get_key_pos() {
      switch (key_position) {
      case KEY_POS::BOTTOM_LEFT: return "bottom left";
      case KEY_POS::BOTTOM_RIGHT: return "bottom right";
      case KEY_POS::TOP_LEFT: return "top left";
      case KEY_POS::TOP_RIGHT: return "top right";
      default: return "bottom right";
      }
    }

    void set_font(std::string type, size_t size) { font = type; font_size = size; }
    void set_output(std::string file) { output = file; }
    void add_line(std::string name, STYLE style_line, std::string data_file, int line1, int line2) {
      std::string line = "'"+data_file+"' u "+std::to_string(line1)+":"+std::to_string(line2)
        + " t '"+name+"' w lp ls "+std::to_string(style_line);
      plot_lines.push_back(line);
    }
    void add_function(std::string name, STYLE style_line, std::string function) {
      plot_lines.push_back(function+" t '"+name+"' w lp ls "+std::to_string(style_line));
    }
    void set_xlabel(std::string label) { xlabel = label; }
    void set_ylabel(std::string label) { ylabel = label; }
    void set_xrange(long long lower, long long upper) { xlower = lower; xupper = upper;}
    void set_yrange(long long lower, long long upper) { ylower = lower; yupper = upper;}
    std::string generate_script() {
      std::stringstream ss;
      ss << "#!/usr/bin/gnuplot" << std::endl;
      ss << "set terminal postscript eps enhanced color font '" << font <<"," << font_size<<"'"<<std::endl;
      ss << "set output '" << output << "'" << std::endl;
      ss << "set style line 11 lc rgb '#808080' lt 1" << std::endl;
      ss << "set border 3 back ls 11" << std::endl;
      ss << "set tics nomirror" << std::endl;
      ss << "set style line 12 lc rgb '#808080' lt 0 lw 1" << std::endl;
      ss << "set grid back ls 12" << std::endl;

      ss << "set style line 1 lc rgb '#8b1a0e' pt 1 ps 1 lt 1 lw 2 # --- red" << std::endl;
      ss << "set style line 2 lc rgb '#5e9c36' pt 6 ps 1 lt 1 lw 2 # --- green" << std::endl;
      ss << "set style line 3 lc rgb '#88419d' pt 2 ps 1 lt 1 lw 2 # --- purple" << std::endl;
      ss << "set style line 4 lc rgb '#225ea8' pt 3 ps 1 lt 1 lw 2 # --- blue" << std::endl;
      ss << "set style line 5 lc rgb '#000000' pt 4 ps 1 lt 1 lw 2 # --- black" << std::endl;
      ss << "set key " << get_key_pos() << std::endl;
      ss << "set xlabel '" << xlabel << "'" << std::endl;
      ss << "set ylabel '" << ylabel << "'" << std::endl;
      if (xlower != xupper && ylower != yupper) {
        ss << "set xrange [" << xlower << ":" << xupper << "]" << std::endl;
        ss << "set yrange [" << ylower << ":" << yupper << "]" << std::endl;
      }

      ss << "plot ";
      for (auto it = plot_lines.begin(); it != plot_lines.end(); it++) {
        if (it != plot_lines.begin()) ss << ", \\" << std::endl;
        ss << *it;
      }
      return ss.str();
    }

    void output_script(std::string output_file) {
      std::ofstream file;
      file.open(output_file);
      file << generate_script();
      file.close();
    }

    void output_plot() {
      std::ofstream file;
      file.open("hjafbs781g5.sh");
      file << generate_script();
      file.close();
      int r = system("chmod +x hjafbs781g5.sh");
      r = system("./temp.sh");
      r = system("rm -rf hjafbs781g5.sh");
      r++;
    }

  private:
    std::string output = "output.eps";
    std::string font = "Verdana";
    std::string xlabel = "N (input size)";
    std::string ylabel = "s (running time in seconds)";
    size_t font_size = 9;
    long long xlower = 0,xupper = 0,ylower = 0,yupper = 0;
    KEY_POS key_position = KEY_POS::BOTTOM_RIGHT;
    std::vector<std::string> plot_lines;
  };

  class point_data_generator {

  public:
    point_data_generator() {}
    ~point_data_generator() {}
    void generate(size_t bytes, const std::string &output_file) {
      util::remove_file(output_file);
      io::buffered_stream<point> of(4096);
      of.open(output_file);
      random r;
      size_t data_size = sizeof(point);
      size_t data_written = 0;
      while (data_written < bytes) {
        of.write(point(r.next(INF-1), r.next(INF-1)));
        data_written+=data_size;
      }
      of.close();
    }
  };

  
};

#endif
