#ifndef STREAM_HPP
#define STREAM_HPP

#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <error.h>
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
    off_t seek(size_t offset, int whence);
    T read();
    size_t tell();
    size_t size();
    bool eof();
    void truncate();
  private:
    int file_descriptor;
    void fill();
    bool should_refill();
    inline size_t buffer_pos();
    size_t buffer_size, file_pos;//, buffer_pos;
    off64_t file_size;
    off_t buffer_start;
    T* buffer;
    std::string file_name;
    bool is_open, b_eof, is_dirty;
  };

  template <typename T>
  buffered_stream<T>::buffered_stream(size_t num_elements) {
    this->buffer_size = num_elements * sizeof(T);
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
    file_descriptor = ::open(file_name.c_str(), O_RDWR | O_CREAT | O_LARGEFILE, S_IRWXU);
    if (file_descriptor == -1) { perror(std::string("Error opening file '").append(file_name).append("'").c_str()); exit(errno); }
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
    DEBUG_MSG("e " << eof());
    DEBUG_MSG("bs " << buffer_start);
    if (is_dirty) sync();
    if (!e) {
      DEBUG_MSG("fill");
      buffer_start = seek(0,SEEK_CUR);
      if (buffer_start == -1) {
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
      DEBUG_MSG(file_size << " " << b_eof << " " << file_pos << " " << bytes_read);
      // file_pos += bytes_read;
    } else {
      file_pos = seek(0,SEEK_END);
      buffer_start = file_pos;
    }
    // buffer_pos = 0; 
  }

  template <typename T>
  void buffered_stream<T>::sync() {
    DEBUG_MSG("sync");
    DEBUG_MSG("bfs " << buffer_start);
    is_dirty = false;
    size_t elems = buffer_pos();
    off_t cur_pos = seek(0, SEEK_CUR);
    DEBUG_MSG("cp " << cur_pos);
    seek(buffer_start, SEEK_SET);
    size_t bytes_written = ::write(file_descriptor, buffer, elems*sizeof(T));
    if (bytes_written == -1) {
      perror(std::string("Error on syncing buffer for file: '").append(file_name).append("'").c_str());
      exit(errno);
    }
    seek(cur_pos,SEEK_SET);
    DEBUG_MSG("fp " << file_pos);
    file_size = std::max((size_t) file_size, file_pos);
  }

  template <typename T>
  off_t buffered_stream<T>::seek(size_t offset, int whence) {
    DEBUG_MSG("seek to " << offset << " from " << whence);
    if (is_dirty) sync();
    file_pos = lseek(file_descriptor, offset, whence);
    if (file_pos == -1) {
      perror(std::string("Error seeking file: ").append(file_name).append("'").c_str());
      exit(errno);
    }
    DEBUG_MSG("fps " << file_pos << " " << buffer_start << " ");
    return file_pos;
  }
  
  template <typename T>
  T buffered_stream<T>::read() {
    if (eof()) {
      error(1, ENOTTY, "reading beyond end of file");
    }
    DEBUG_MSG("should refill " << should_refill());
    if (should_refill()) fill();
    DEBUG_MSG("buffer _start " << buffer_start);
    DEBUG_MSG((file_pos - buffer_start)/sizeof(T));
    T element = buffer[buffer_pos()];
    DEBUG_MSG("read " << element);
    file_pos+=sizeof(T);
    return element;
  }

  template <typename T>
  void buffered_stream<T>::write(T item) {
    DEBUG_MSG("write " << item);
    if (should_refill()) fill();
    is_dirty = true;
    buffer[buffer_pos()] = item;
    file_pos+=sizeof(T);
  }

  template <typename T>
  inline size_t buffered_stream<T>::buffer_pos() {
    return (file_pos-buffer_start)/sizeof(T);
  }

  template <typename T>
  bool buffered_stream<T>::should_refill() {
    if ( !(buffer_start <= file_pos &&
           file_pos < buffer_start+buffer_size) ) return true;
    if (buffer_pos() >= buffer_size/sizeof(T)) return true;
    return false;
  }
  

  template <typename T>
  size_t buffered_stream<T>::size() {
    return std::max((size_t)file_size, file_pos);
  }

  template <typename T>
  void buffered_stream<T>::truncate() {
    DEBUG_MSG("truncate " << file_pos << " " << file_size);
    if (ftruncate(file_descriptor,file_pos) == -1) {
      perror(std::string("Error on truncating file: ").append(file_name).append("'").c_str());
      exit(errno);
    }
    file_size = file_pos;
    fill();
    
    DEBUG_MSG("truncate2 " << file_pos << " " << file_size);
  }

};
#endif
