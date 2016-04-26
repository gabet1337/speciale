#ifndef STREAM_HPP
#define STREAM_HPP
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <algorithm>
#include <iostream>
#include "../common/debug.hpp"

namespace io {
  template <typename T>
  class buffered_stream {
  public:
    buffered_stream(size_t num_elements);
    ~buffered_stream();
    void open(std::string file_name);
    void close();
    void write(T item);
    void sync();
    off64_t seek(off64_t offset, int whence);
    T read();
    off64_t size();
    bool eof();
    void truncate();
  private:
    int file_descriptor;
    void fill();
    bool should_refill();
    inline off64_t buffer_pos();
    off64_t buffer_size;
    off64_t file_size;
    off64_t file_pos;
    off64_t buffer_start;
    T* buffer;
    std::string file_name;
    bool is_open, b_eof, is_dirty;
  };

  template <typename T>
  buffered_stream<T>::buffered_stream(size_t num_elements) {
    this->buffer_size = num_elements * sizeof(T);
    buffer = new T[num_elements]();
    is_open = false;
  }

  template <typename T>
  buffered_stream<T>::~buffered_stream() {
    if (is_open) close();
    delete [] buffer;
  }

  template <typename T>
  void buffered_stream<T>::open(std::string file_name) {
    file_descriptor = ::open(file_name.c_str(), O_RDWR | O_CREAT | O_LARGEFILE, S_IRWXU);
    if (file_descriptor == -1) {
      perror(std::string("Error opening file '").append(file_name).append("'").c_str());
      exit(errno);
    }
    is_open = true;
    is_dirty = false;
    this->file_name = file_name;
    file_pos = 0;
    // buffer_pos = 0;
    buffer_start = 0;
    struct stat sb;
    fstat(file_descriptor, &sb);
    file_size = sb.st_size;
    b_eof = file_size == file_pos;
    posix_fadvise(file_descriptor, 0, 0, POSIX_FADV_SEQUENTIAL);
    posix_fadvise(file_descriptor, 0, 0, POSIX_FADV_WILLNEED);
    fill();
  }

  template <typename T>
  void buffered_stream<T>::close() {
    if (!is_open) {
      error(1, ENOENT, "Error closing file");
    }
    sync();
    if (::close(file_descriptor) == -1) {
      perror(std::string("Error closing file '").append(file_name).append("'").c_str());
      exit(errno);
    }
    is_open = false;
  }

  template <typename T>
  bool buffered_stream<T>::eof() {
    return b_eof && file_pos >= file_size;
  }

  template <typename T>
  void buffered_stream<T>::fill() {
    bool e = eof();
    if (is_dirty) sync();
    if (!e) {
      buffer_start = seek(0,SEEK_CUR);
      if (buffer_start == (off64_t)-1) {
        perror(std::string("Error seeking file: ").append(file_name).append("'").c_str());
        exit(errno);
      }
      ssize_t bytes_read = ::read(file_descriptor, buffer, buffer_size);
      if (bytes_read == -1) {
        perror(std::string("Error reading from file '").append(file_name).append("'").c_str());
        exit(errno);
      }
      if (bytes_read < buffer_size) b_eof = true; else b_eof = false;
      if (buffer_start+bytes_read >= file_size) b_eof = true; else b_eof = false;
    } else {
      file_pos = seek(0,SEEK_END);
      buffer_start = file_pos;
    }
  }

  template <typename T>
  void buffered_stream<T>::sync() {
    is_dirty = false;
    off64_t elems = buffer_pos();
    off64_t cur_pos = seek(0, SEEK_CUR);
    seek(buffer_start, SEEK_SET);
    ssize_t bytes_written = ::write(file_descriptor, buffer, elems*sizeof(T));
    if (bytes_written == (ssize_t)-1) {
      perror(std::string("Error on syncing buffer for file: '").append(file_name).append("'").c_str());
      exit(errno);
    }
    seek(cur_pos,SEEK_SET);
    file_size = std::max(file_size, file_pos);
  }

  template <typename T>
  off64_t buffered_stream<T>::seek(off64_t offset, int whence) {
    if (is_dirty) sync();
    file_pos = lseek64(file_descriptor, offset, whence);
    if (file_pos == (off64_t)-1) {
      perror(std::string("Error seeking file: ").append(file_name).append("'").c_str());
      exit(errno);
    }
    return file_pos;
  }
  
  template <typename T>
  T buffered_stream<T>::read() {
    if (eof()) {
      error(1, ENOTTY, "reading beyond end of file");
    }
    if (should_refill()) fill();
    T element = buffer[buffer_pos()];
    file_pos+=sizeof(T);
    return element;
  }

  template <typename T>
  void buffered_stream<T>::write(T item) {
    if (should_refill()) fill();
    is_dirty = true;
    buffer[buffer_pos()] = item;
    file_pos+=sizeof(T);
  }

  template <typename T>
  inline off64_t buffered_stream<T>::buffer_pos() {
    return (file_pos-buffer_start)/sizeof(T);
  }

  template <typename T>
  bool buffered_stream<T>::should_refill() {
    if ( !(buffer_start <= file_pos &&
           file_pos < buffer_start+buffer_size) ) return true;
    if (buffer_pos() >= buffer_size/(off64_t)sizeof(T)) return true;
    return false;
  }
  

  template <typename T>
  off64_t buffered_stream<T>::size() {
    return std::max(file_size, file_pos)/sizeof(T);
  }

  template <typename T>
  void buffered_stream<T>::truncate() {
    if (ftruncate(file_descriptor,file_pos) == -1) {
      perror(std::string("Error on truncating file: ").append(file_name).append("'").c_str());
      exit(errno);
    }
    file_size = file_pos;
    fill();
  }
};
#endif
