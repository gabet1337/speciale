#ifndef SYS_STREAM_STUB_HPP
#define SYS_STREAM_STUB_HPP
#include "../../common/pst_interface.hpp"
#include "../../common/utilities.hpp"
#include "../../stream/old_stream.hpp"

namespace experiment {

  class sys_stream_stub : public common::pst_interface {
  public:
    sys_stream_stub();
    sys_stream_stub(size_t buffer_size, double epsilon);
    ~sys_stream_stub();
    void insert(const point &p);
    void remove(const point &p);
    void report(int x1, int x2, int y, const std::string &output_file);
    size_t size();
    void print();
  private:
    io::ristream *input_stream;
    io::rostream *output_stream;
  };

  sys_stream_stub::sys_stream_stub() {
  }

  sys_stream_stub::sys_stream_stub(size_t buffer_size, double epsilon) {
    input_stream = new io::ristream(buffer_size);
    output_stream = new io::rostream(buffer_size);
    input_stream->open_stream("../data/insert_experiment");
    output_stream->create_stream("../data/temp_sys_out");
  }

  sys_stream_stub::~sys_stream_stub() {
    input_stream->close_stream();
    output_stream->close_stream();
    delete input_stream;
    delete output_stream;
    util::remove_file("../data/temp_sys_out");
  }

  void sys_stream_stub::insert(const point &p) {
    output_stream->write_stream(1337);
  }

  void sys_stream_stub::remove(const point &p) {
    input_stream->read_next();
  }

  void sys_stream_stub::report(int x1, int x2, int y, const std::string &output_file) {

  }

  void sys_stream_stub::print() {
  }
  
};

#endif
