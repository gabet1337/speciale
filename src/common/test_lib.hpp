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
#include <unistd.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <asm/unistd.h>
#include "point.hpp"
#include "../stream/stream.hpp"
#include "definitions.hpp"
#include "utilities.hpp"

namespace test {

  unsigned long long GERTH_NUM_NODE_DEGREE_OVERFLOW = 0;
  unsigned long long GERTH_NUM_INSERT_BUFFER_OVERFLOW = 0;
  unsigned long long GERTH_NUM_POINT_BUFFER_OVERFLOW = 0;
  unsigned long long GERTH_NUM_DELETE_BUFFER_OVERFLOW = 0;
  unsigned long long GERTH_NUM_POINT_BUFFER_UNDERFLOW = 0;

  class clock {
    typedef std::chrono::high_resolution_clock hsc;
    typedef hsc::time_point tp;
  public:
    void start();
    void stop();
    long long elapsed();
    long long elapsed_ms();
    long long count_ms();
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

  long long clock::elapsed_ms() {
    auto n = hsc::now();
    using namespace std;
    using namespace std::chrono;
    return duration_cast<milliseconds>(n-s).count();
  }

  long long clock::count_ms() {
    using namespace std;
    using namespace std::chrono;
    return duration_cast<milliseconds>(e-s).count();
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
    gnuplot() { for (int i = 0; i < 256; i++) available[i] = true;}
    ~gnuplot() {}
    enum KEY_POS { BOTTOM_LEFT, BOTTOM_RIGHT, TOP_LEFT, TOP_RIGHT };
    std::string get_key_pos() {
      switch (key_position) {
      case KEY_POS::BOTTOM_LEFT: return "bottom left";
      case KEY_POS::BOTTOM_RIGHT: return "bottom right";
      case KEY_POS::TOP_LEFT: return "top left";
      case KEY_POS::TOP_RIGHT: return "top right";
      default: return "top left";
      }
    }
    typedef common::PST_VARIANT STYLE;
    void set_font(std::string type, size_t size) { font = type; font_size = size; }
    void set_output(std::string file) { output = file; }
    void add_line(std::string name, STYLE style_line, std::string data_file, int line1, int line2) {
      // std::string line = "'"+data_file+"' u "+std::to_string(line1)+":"+std::to_string(line2)
      //   + " t '"+name+"' w lp ls "+std::to_string(style_line);
      std::string line = "'"+data_file+"' u "+std::to_string(line1)+":"+std::to_string(line2)
        + " t '"+name+"' w lp ls "+std::to_string(get_available_style(style_line));
            
      plot_lines.push_back(line);
    }
    void add_function(std::string name, STYLE style_line, std::string function) {
      plot_lines.push_back(function+" t '"+name+"' w lp ls "+std::to_string(style_line));
    }
    int get_available_style(STYLE style) {
      int res = style;
      if (available[res]) {
        available[res] = false;
        return res;
      } else {
        for (res = res+1; res != style && !available[res]; ) if (res+1 == STYLE::end) res = 1; else res++;
        available[res] = false;
        return res;
      }
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
      ss << "set style line 6 lc rgb '#00ced1' pt 5 ps 1 lt 1 lw 2 # --- darkturquoise" << std::endl;
      ss << "set style line 7 lc rgb '#ff00ff' pt 7 ps 1 lt 1 lw 2 # --- magenta" << std::endl;
      ss << "set style line 8 lc rgb '#87ceeb' pt 8 ps 1 lt 1 lw 2 # --- skyblue" << std::endl;
      ss << "set key " << get_key_pos() << std::endl;
      ss << "set xlabel '" << xlabel << "'" << std::endl;
      ss << "set ylabel '" << ylabel << "'" << std::endl;
      if (xlower != xupper && ylower != yupper) {
        ss << "set xrange [" << xlower << ":" << xupper << "]" << std::endl;
        ss << "set yrange [" << ylower << ":" << yupper << "]" << std::endl;
      } else {
        ss << "#set xrange [" << xlower << ":" << xupper << "]" << std::endl;
        ss << "#set yrange [" << ylower << ":" << yupper << "]" << std::endl;
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

    void output_plot(std::string script) {
      int r = system((std::string("chmod +x ")+script).c_str());
      r = system((std::string("./")+script).c_str());
      //r = system((std::string("rm -rf ")+script).c_str());
      r++;
    }

  private:
    bool available[256];
    std::string output = "output.eps";
    std::string font = "Verdana";
    std::string xlabel = "N (input size in MB)";
    std::string ylabel = "s (running time in seconds)";
    size_t font_size = 11;
    long long xlower = 0,xupper = 0,ylower = 0,yupper = 0;
    KEY_POS key_position = KEY_POS::TOP_LEFT;
    std::vector<std::string> plot_lines;
  };

  class point_data_generator {

  public:
    point_data_generator() {}
    ~point_data_generator() {}
    void generate(unsigned long long bytes, const std::string &output_file) {
      util::remove_file(output_file);
      io::buffered_stream<point> of(4096);
      of.open(output_file);
      random r;
      unsigned long long data_size = sizeof(point);
      unsigned long long data_written = 0;
      while (data_written < bytes) {
        of.write(point(r.next(INF-1), r.next(INF-1)));
        data_written+=data_size;
      }
      of.close();
    }
  };

  class pagefaults {
  public:
    pagefaults();
    void start();
    void stop();
    uint64_t count();
    uint64_t elapsed();
  private:
    long perf_event_open(perf_event_attr*, pid_t, int, int, long unsigned int);
    struct perf_event_attr pe_attr_page_faults;
    int page_faults_fd;
    uint64_t page_faults_count;
  };

  pagefaults::pagefaults() {
    memset(&pe_attr_page_faults, 0, sizeof(pe_attr_page_faults));
    pe_attr_page_faults.size = sizeof(pe_attr_page_faults);
    pe_attr_page_faults.type = PERF_TYPE_SOFTWARE;
    pe_attr_page_faults.config = PERF_COUNT_SW_PAGE_FAULTS;
    pe_attr_page_faults.disabled = 1;
    pe_attr_page_faults.exclude_kernel = 1;
    page_faults_fd = perf_event_open(&pe_attr_page_faults, 0, -1, -1, 0);
    if (page_faults_fd ==  -1) {
      printf("perf_event_open failed for page faults %s\n", strerror(errno));
      return;
    }
  }

  long pagefaults::perf_event_open(struct perf_event_attr *hw_event,
				   pid_t pid,
				   int cpu,
				   int group_fd,
				   unsigned long flags) {
    int ret = syscall(__NR_perf_event_open, hw_event, pid, cpu,
		      group_fd, flags);
    return ret;
  }

  void pagefaults::start() {
    ioctl(page_faults_fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(page_faults_fd, PERF_EVENT_IOC_ENABLE, 0);
  }

  void pagefaults::stop() {
    ioctl(page_faults_fd, PERF_EVENT_IOC_DISABLE, 0);
    int r = read(page_faults_fd, &page_faults_count, sizeof(page_faults_count));
    r++;
  }

  uint64_t pagefaults::count() {
    return page_faults_count;
  }

  uint64_t pagefaults::elapsed() {
    int r =read(page_faults_fd, &page_faults_count, sizeof(page_faults_count));
    r++;
    return page_faults_count;
  }

  class proc_io {

  public:
    proc_io() { start = 0; }
    ~proc_io() {}
    uint64_t total_ios() {
      return io::STREAM_NUM_IOS - start;
    }
    void restart() {
      start = io::STREAM_NUM_IOS;
    }
  private:
    size_t start;
  };

  class gerth_num_point_buffer_overflow {
  public:
    gerth_num_point_buffer_overflow() { start = 0; }
    ~gerth_num_point_buffer_overflow() {}
    uint64_t elapsed() {
      return GERTH_NUM_POINT_BUFFER_OVERFLOW - start;
    }
    void restart() {
      start = GERTH_NUM_POINT_BUFFER_OVERFLOW;
    }
  private:
    size_t start;
  };

  class gerth_num_insert_buffer_overflow {
  public:
    gerth_num_insert_buffer_overflow() { start = 0; }
    ~gerth_num_insert_buffer_overflow() {}
    uint64_t elapsed() {
      return GERTH_NUM_INSERT_BUFFER_OVERFLOW - start;
    }
    void restart() {
      start = GERTH_NUM_INSERT_BUFFER_OVERFLOW;
    }
  private:
    size_t start;
  };

  class gerth_num_delete_buffer_overflow {
  public:
    gerth_num_delete_buffer_overflow() { start = 0; }
    ~gerth_num_delete_buffer_overflow() {}
    uint64_t elapsed() {
      return GERTH_NUM_DELETE_BUFFER_OVERFLOW - start;
    }
    void restart() {
      start = GERTH_NUM_DELETE_BUFFER_OVERFLOW;
    }
  private:
    size_t start;
  };

  class gerth_num_point_buffer_underflow {
  public:
    gerth_num_point_buffer_underflow() { start = 0; }
    ~gerth_num_point_buffer_underflow() {}
    uint64_t elapsed() {
      return GERTH_NUM_POINT_BUFFER_UNDERFLOW - start;
    }
    void restart() {
      start = GERTH_NUM_POINT_BUFFER_UNDERFLOW;
    }
  private:
    size_t start;
  };

  class gerth_num_node_degree_overflow {
  public:
    gerth_num_node_degree_overflow() { start = 0; }
    ~gerth_num_node_degree_overflow() {}
    uint64_t elapsed() {
      return GERTH_NUM_NODE_DEGREE_OVERFLOW - start;
    }
    void restart() {
      start = GERTH_NUM_NODE_DEGREE_OVERFLOW;
    }
  private:
    size_t start;
  };
  
  class beeper {

  public:
    beeper() {}
    ~beeper() {}
    void star_wars() {
      beep(300,500);
      mssleep(50);
      beep(300,500);
      mssleep(50);
      beep(300,500);
      mssleep(50);
 
      beep(250,500);
      mssleep(50);
 
      beep(350,250);
      beep(300,500);
      mssleep(50);
 
      beep(250,500);
      mssleep(50);
 
      beep(350,250);
      beep(300,500);
      mssleep(50);

      mssleep(5000);
    }

    void mario() {
      Beep(480,200);
      Beep(1568,200);
      Beep(1568,200);
      Beep(1568,200);
      Beep(739.99,200);
      Beep(783.99,200);
      Beep(783.99,200);
      Beep(783.99,200);
      Beep(369.99,200);
      Beep(392,200);
      Beep(369.99,200);
      Beep(392,200);
      Beep(392,400);
      Beep(196,400);
      Beep(739.99,200);
      Beep(783.99,200);
      Beep(783.99,200);
      Beep(739.99,200);
      Beep(783.99,200);
      Beep(783.99,200);
      Beep(739.99,200);
      Beep(83.99,200);
      Beep(880,200);
      Beep(830.61,200);
      Beep(880,200);
      Beep(987.77,400);
      Beep(880,200);
      Beep(783.99,200);
      Beep(698.46,200);
      Beep(739.99,200);
      Beep(783.99,200);
      Beep(783.99,200);
      Beep(739.99,200);
      Beep(783.99,200);
      Beep(783.99,200);
      Beep(739.99,200);
      Beep(783.99,200);
      Beep(880,200);
      Beep(830.61,200);
      Beep(880,200);
      Beep(987.77,400);
      mssleep(200);
      Beep(1108,10);
      Beep(1174.7,200);
      Beep(1480,10);
      Beep(1568,200);
      mssleep(200);
      Beep(739.99,200);
      Beep(783.99,200);
      Beep(783.99,200);
      Beep(739.99,200);
      Beep(783.99,200);
      Beep(783.99,200);
      Beep(739.99,200);
      Beep(783.99,200);
      Beep(880,200);
      Beep(830.61,200);
      Beep(880,200);
      Beep(987.77,400);
      Beep(880,200);
      Beep(783.99,200);
      Beep(698.46,200);
      Beep(659.25,200);
      Beep(698.46,200);
      Beep(784,200);
      Beep(880,400);
      Beep(784,200);
      Beep(698.46,200);
      Beep(659.25,200);
      Beep(587.33,200);
      Beep(659.25,200);
      Beep(698.46,200);
      Beep(784,400);
      Beep(698.46,200);
      Beep(659.25,200);
      Beep(587.33,200);
      Beep(523.25,200);
      Beep(587.33,200);
      Beep(659.25,200);
      Beep(698.46,400);
      Beep(659.25,200);
      Beep(587.33,200);
      Beep(493.88,200);
      Beep(523.25,200);
      mssleep(400);
      Beep(349.23,400);
      Beep(392,200);
      Beep(329.63,200);
      Beep(523.25,200);
      Beep(493.88,200);
      Beep(466.16,200);
      Beep(440,200);
      Beep(493.88,200);
      Beep(523.25,200);
      Beep(880,200);
      Beep(493.88,200);
      Beep(880,200);
      Beep(1760,200);
      Beep(440,200);
      Beep(392,200);
      Beep(440,200);
      Beep(493.88,200);
      Beep(783.99,200);
      Beep(440, 200);
      Beep(783.99,200);
      Beep(1568,200);
      Beep(392,200);
      Beep(349.23,200);
      Beep(392,200);
      Beep(440,200);
      Beep(698.46,200);
      Beep(415.2,200);
      Beep(698.46,200);
      Beep(1396.92,200);
      Beep(349.23,200);
      Beep(329.63,200);
      Beep(311.13,200);
      Beep(329.63,200);
      Beep(659.25,200);
      Beep(698.46,400);
      Beep(783.99,400);
      Beep(440,200);
      Beep(493.88,200);
      Beep(523.25,200);
      Beep(880,200);
      Beep(493.88,200);
      Beep(880,200);
      Beep(1760,200);
      Beep(440,200);
      Beep(392,200);
      Beep(440,200);
      Beep(493.88,200);
      Beep(783.99,200);
      Beep(440,200);
      Beep(783.99,200);
      Beep(1568,200);
      Beep(392,200);
      Beep(349.23,200);
      Beep(392,200);
      Beep(440,00);
      Beep(698.46,200);
      Beep(659.25,200);
      Beep(698.46,200);
      Beep(739.99,200);
      Beep(783.99,200);
      Beep(392,200);
      Beep(392,200);
      Beep(392,200);
      Beep(392,200);
      Beep(196,200);
      Beep(196,200);
      Beep(196,200);
      Beep(185,200);
      Beep(196,200);
      Beep(185,200);
      Beep(196,200);
      Beep(207.65,200);
      Beep(220,200);
      Beep(233.08,200);
      Beep(246.94,200);
      mssleep(5000);
    }
  private:
    void Beep(int freq, int duration) { beep(freq, duration); }
    void beep(int freq, int duration) {
      std::string s = "beep -f " + std::to_string(freq) + " -l " + std::to_string(duration);
      int r = system(s.c_str());
      r++;
    }
    void mssleep(int ms) {
      usleep(ms*1000);
    }

  };
  static void drop_cache() {
    int r = system("echo 3 | tee /proc/sys/vm/drop_caches");
    r++;
  }
};

#endif
