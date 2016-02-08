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
    if (b_eof) return true;
    return buffer_start >= file_size;
  }

  template <typename T>
  void buffered_stream<T>::fill() {
    std::cout << "eof at this point: " << eof() << std::endl;
    //std::cout << "file_pos: " << file_pos << std::endl;
    //std::cout << "buffer_pos: " << buffer_pos << std::endl;
    //std::cout << "file_size: " << file_size << std::endl;
    if (is_dirty) sync();
    if (!eof()) {
      buffer_start = lseek(file_descriptor,0,SEEK_CUR);
      //std::cout << "buffer_start: " << buffer_start << std::endl;
      if (buffer_start == -1) {
        perror(std::string("Error seeking file: ").append(file_name).append("'").c_str());
        exit(errno);
      }
      ssize_t bytes_read = ::read(file_descriptor, buffer, buffer_size);
      if (bytes_read == -1) {
        perror(std::string("Error reading from file '").append(file_name).append("'").c_str());
        exit(errno);
      }
      if (bytes_read < buffer_size) b_eof = true;
      file_pos += bytes_read;
      std::cout << "trolololo" << std::endl;
    } else {
      file_pos = seek(0,SEEK_END);
      buffer_start = file_pos;
      std::cout << "file_pos at this point: " << file_pos << std::endl;
    }
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
    is_dirty = true;
    if (buffer_pos >= buffer_size/sizeof(T)) fill();
    buffer[buffer_pos++] = item;
  }

  template <typename T>
  void buffered_stream<T>::sync() {
    std::cout << "eof: " << eof() << std::endl;
    std::cout << "file_pos: " << file_pos << std::endl;
    std::cout << "buffer_pos: " << buffer_pos << std::endl;
    std::cout << "file_size: " << file_size << std::endl;

    off_t cur_pos = seek(0, SEEK_CUR);
    //std::cout << "current_pos: " << cur_pos << std::endl;
    std::cout << "buffer_start: " << buffer_start << std::endl;
    seek(buffer_start, SEEK_SET);
    size_t bytes_written = ::write(file_descriptor, buffer, buffer_pos*sizeof(T));
    if (bytes_written == -1) {
      perror(std::string("Error on syncing buffer for file: '").append(file_name).append("'").c_str());
      exit(errno);
    }
    file_size = std::max((size_t) file_size, buffer_start+bytes_written);
    std::cout << "file_size at this point: " << file_size << std::endl;
    std::cout << seek(cur_pos,SEEK_SET) << std::endl;
    std::cout << "bytes_written: " << bytes_written << std::endl;
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
  size_t buffered_stream<T>::size() {
    return (size_t) file_size;
  }

};
#endif
