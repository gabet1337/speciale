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
    void truncate();
  private:
    int file_descriptor;
    void fill();
    void sync();
    off_t seek(size_t offset, int whence);
    size_t buffer_size, file_pos, buffer_pos;
    off64_t file_size;
    off_t buffer_start;
    T* buffer;
    std::string file_name;
    bool is_open, b_eof, is_dirty;
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
    file_descriptor = ::open(file_name.c_str(), O_RDWR | O_CREAT | O_LARGEFILE, S_IRWXU);
    if (file_descriptor == -1) { perror(std::string("Error opening file '").append(file_name).append("'").c_str()); exit(errno); }
    is_open = true;
    b_eof = false;
    is_dirty = false;
    this->file_name = file_name;
    file_pos = 0;
    buffer_pos = 0;
    buffer_start = 0;
    struct stat sb;
    fstat(file_descriptor, &sb);
    file_size = sb.st_size;
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
    return b_eof && buffer_pos >= (file_size-buffer_start)/sizeof(T);
  }

  template <typename T>
  void buffered_stream<T>::fill() {
    bool e = eof();
    if (is_dirty) sync();
    if (!e) {
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
      if (buffer_start + bytes_read >= file_size) b_eof = true;
      file_pos += bytes_read;
    } else {
      file_pos = seek(0,SEEK_END);
      buffer_start = file_pos;
    }
    buffer_pos = 0; 
  }

  template <typename T>
  void buffered_stream<T>::sync() {
    off_t cur_pos = seek(0, SEEK_CUR);
    seek(buffer_start, SEEK_SET);
    size_t bytes_written = ::write(file_descriptor, buffer, buffer_pos*sizeof(T));
    if (bytes_written == -1) {
      perror(std::string("Error on syncing buffer for file: '").append(file_name).append("'").c_str());
      exit(errno);
    }
    file_size = std::max((size_t) file_size, buffer_start+bytes_written);
    seek(cur_pos,SEEK_SET);
    is_dirty = false;
  }

  template <typename T>
  off_t buffered_stream<T>::seek(size_t offset, int whence) {
    off_t pos = lseek(file_descriptor,offset,whence);
    if (pos == -1) {
      perror(std::string("Error seeking file: ").append(file_name).append("'").c_str());
      exit(errno);
    }
    return pos;
  }
  
  template <typename T>
  T buffered_stream<T>::read() {
    if (buffer_pos*sizeof(T) >= buffer_size) fill();
    T element = buffer[buffer_pos++];
    return element;
  }

  template <typename T>
  void buffered_stream<T>::write(T item) {
    is_dirty = true;
    if (buffer_pos >= buffer_size/sizeof(T)) fill();
    buffer[buffer_pos++] = item;
  }

  template <typename T>
  size_t buffered_stream<T>::size() {
    return (size_t) file_size;
  }

  template <typename T>
  void buffered_stream<T>::truncate() {
    if (ftruncate(file_descriptor,file_pos) == -1) {
      perror(std::string("Error on truncation file: ").append(file_name).append("'").c_str());
      exit(errno);
    }
    fill(); // clear buffer
  }

};
#endif
