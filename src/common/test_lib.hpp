#ifndef TEST_LIB
#define TEST_LIB
#include <chrono>
#include <random>
#include <limits.h>
namespace test {
  class clock {
    typedef std::chrono::high_resolution_clock hsc;
    typedef hsc::time_point tp;
  public:
    void start();
    void stop();
    long long count();
  private:
    tp s,e;
  };

  void clock::start() {
    s = hsc::now();
  }

  void clock::stop() {
    e = hsc::now();
  }

  long long clock::count() {
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
};

#endif
