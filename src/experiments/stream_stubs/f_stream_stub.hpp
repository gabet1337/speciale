#ifndef F_STREAM_STUB_HPP
#define F_STREAM_STUB_HPP
#include "../../common/pst_interface.hpp"
#include "../../common/utilities.hpp"
#include "../../stream/old_stream.hpp"

namespace experiment {

  class f_stream_stub : public common::pst_interface {
  public:
    f_stream_stub();
    f_stream_stub(size_t buffer_size, double epsilon);
    ~f_stream_stub();
    void insert(const point &p);
    void remove(const point &p);
    void report(int x1, int x2, int y, const std::string &output_file);
    size_t size();
    void print();
  private:
    io::fistream *input_stream;
    io::fostream *output_stream;
  };

  f_stream_stub::f_stream_stub() {
  }

  f_stream_stub::f_stream_stub(size_t buffer_size, double epsilon) {
    input_stream = new io::fistream(buffer_size);
    output_stream = new io::fostream(buffer_size);
    input_stream->open_stream("../data/insert_experiment");
    output_stream->create_stream("../data/temp_f_out");
  }

  f_stream_stub::~f_stream_stub() {
    input_stream->close_stream();
    output_stream->close_stream();
    delete input_stream;
    delete output_stream;
    util::remove_file("../data/temp_f_out");
  }

  void f_stream_stub::insert(const point &p) {
    output_stream->write_stream(1337);
  }

  void f_stream_stub::remove(const point &p) {
    input_stream->read_next();
  }

  void f_stream_stub::report(int x1, int x2, int y, const std::string &output_file) {

  }

  void f_stream_stub::print() {
  }
  
};

#endif
