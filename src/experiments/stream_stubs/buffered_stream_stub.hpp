#ifndef BUFFERED_STREAM_STUB_HPP
#define BUFFERED_STREAM_STUB_HPP
#include "../../common/pst_interface.hpp"
#include "../../common/utilities.hpp"
#include "../../stream/stream.hpp"

namespace experiment {

  class buffered_stream_stub : public common::pst_interface {
  public:
    buffered_stream_stub();
    buffered_stream_stub(size_t buffer_size, double epsilon);
    ~buffered_stream_stub();
    void insert(const point &p);
    void remove(const point &p);
    void report(int x1, int x2, int y, const std::string &output_file);
    size_t size();
    void print();
  private:
    io::buffered_stream<int> *input_stream;
    io::buffered_stream<int> *output_stream;
  };

  buffered_stream_stub::buffered_stream_stub() {
  }

  buffered_stream_stub::buffered_stream_stub(size_t buffer_size, double epsilon) {
    input_stream = new io::buffered_stream<int>(buffer_size);
    output_stream = new io::buffered_stream<int>(buffer_size);
    input_stream->open("../data/insert_experiment");
    output_stream->open("../data/temp_buffered_out");
  }

  buffered_stream_stub::~buffered_stream_stub() {
    input_stream->close();
    output_stream->close();
    delete input_stream;
    delete output_stream;
    util::remove_file("../data/temp_buffered_out");
  }

  void buffered_stream_stub::insert(const point &p) {
    output_stream->write(1337);
  }

  void buffered_stream_stub::remove(const point &p) {
    input_stream->read();
  }

  void buffered_stream_stub::report(int x1, int x2, int y, const std::string &output_file) {

  }

  void buffered_stream_stub::print() {
  }
  
};

#endif
