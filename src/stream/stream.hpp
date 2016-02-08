#ifndef STREAM_HPP
#define STREAM_HPP

#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <error.h>

namespace io {
  template <typename T>
  class buffered_stream {
  public:
    buffered_stream(size_t num_elements);
    ~buffered_stream();
    void open(std::string file_name);
    void close();
    void write(T item);
    T read();
    void seek();
    size_t tell();
    size_t size();
    bool empty();
    bool eof();
    void sync();
  private:
    int file_descriptor;
    void fill();
    size_t buffer_size, file_pos, buffer_pos;
    off64_t file_size;
    T* buffer;
    std::string file_name;
    bool is_open;
    bool b_eof;
  };

  template <typename T>
  buffered_stream<T>::buffered_stream(size_t num_elements) {
    this->buffer_size = num_elements*sizeof(T);
    buffer = new T[num_elements];
    is_open = false;
  }

  template <typename T>
  buffered_stream<T>::~buffered_stream() {
    if (is_open) close();
    delete [] buffer;
  }

  template <typename T>
  void buffered_stream<T>::open(std::string file_name) {
    file_descriptor = ::open(file_name.c_str(), O_RDWR | O_CREAT | O_LARGEFILE);
    //todo write file name
    if (file_descriptor == -1) { perror(std::string("Error opening file '").append(file_name).append("'").c_str()); exit(errno); }
    is_open = true;
    b_eof = false;
    this->file_name = file_name;
    file_pos = 0;
    buffer_pos = buffer_size/sizeof(T);
    struct stat sb;
    fstat(file_descriptor, &sb);
    file_size = sb.st_size;
  }

  template <typename T>
  void buffered_stream<T>::close() {
    if (!is_open) {
      error(1, ENOENT, "Error closing file");
    }
    if (::close(file_descriptor) == -1) {
      perror(std::string("Error closing file '").append(file_name).append("'").c_str());
      exit(errno);
    }
    is_open = false;
  }

  template <typename T>
  bool buffered_stream<T>::eof() {
    if (b_eof) return true;
    return file_pos >= file_size;
  }

  template <typename T>
  void buffered_stream<T>::fill() {
    if (eof()) return;
    ssize_t bytes_read = ::read(file_descriptor, buffer, buffer_size);
    if (bytes_read == -1) {
      perror(std::string("Error reading from file '").append(file_name).append("'").c_str());
      exit(errno);
    }
    if (bytes_read < buffer_size) b_eof = true;
    file_pos += bytes_read;
    buffer_pos = 0;
  }

  template <typename T>
  T buffered_stream<T>::read() {
    if (buffer_pos*sizeof(T) >= buffer_size) fill();
    T element = buffer[buffer_pos++];
    return element;
  }

  template <typename T>
  void buffered_stream<T>::write(T item) {
    
  }

};
#endif
