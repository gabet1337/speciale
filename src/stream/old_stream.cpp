#include "old_stream.hpp"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <algorithm>
using namespace io;

// read
// INPUT STREAM
//

ristream::ristream(size_t size) {
  buf = 0;
  fd = 0;
  eof = 0;
}

ristream::ristream() {
  buf = 0;
  fd = 0;
  eof = false;
}

ristream::~ristream() {
  close_stream();
}

void ristream::open_stream(const char* file) {
  fd = open(file, O_RDONLY | O_LARGEFILE); //added largefile since we need large files.
  // std::cout << "OPENING " << file << " with fd: " << fd << std::endl;
  if (fd < 0) {perror("Error opening file");exit(errno);}
  read_next();
}

int ristream::read_next() {
  int res = buf;
  fill();
  return res;
}

void ristream::fill() {
  if (read(fd, &buf, sizeof(buf)) == 0) eof = true;
}

bool ristream::end_of_stream() {
  return eof;
}

void ristream::close_stream() {
  // std::cout << "CLOSING fd: " << fd << std::endl;
  close(fd);
  fd = 0;
  buf = 0;
  eof = false;
}

void ristream::use() {
  std::cout  << "Missing Implementation" << std::endl;
  exit(-1);
}

size_t ristream::size() {
  std::cout  << "Missing Implementation" << std::endl;
  exit(-1);
}

//backstream

ristream_back::ristream_back(size_t size) {
  buf = 0;
  fd = 0;
  eof = 0;
}

ristream_back::ristream_back() {
  buf = 0;
  fd = 0;
  eof = false;
}

ristream_back::~ristream_back() {
  close_stream();
}

void ristream_back::open_stream(const char* file) {
  fd = open(file, O_RDONLY | O_LARGEFILE); //added largefile since we need large files.
  // std::cout << "OPENING " << file << " with fd: " << fd << std::endl;
  if (fd < 0) {perror("Error opening file");exit(errno);}
  lseek(fd,0,SEEK_END);
  read_next();
}

int ristream_back::read_next() {
  has_been_used = false;
  int res = buf;
  fill();
  return res;
}

void ristream_back::use() {
  has_been_used = true;
}

void ristream_back::fill() {
  if (lseek(fd, 0, SEEK_CUR) == 0) eof = true;
  lseek(fd,-1*sizeof(int), SEEK_CUR);
  read(fd, &buf, sizeof(buf));
  lseek(fd,-1*sizeof(int), SEEK_CUR);
}

size_t ristream_back::size() {
  if (!has_been_used && eof) return 1;
  if (eof) return 0;
  return lseek(fd,0,SEEK_CUR)/sizeof(int) + 1 + !has_been_used;
}

bool ristream_back::end_of_stream() {
  return eof;
}

void ristream_back::close_stream() {
  // std::cout << "CLOSING fd: " << fd << std::endl;
  close(fd);
  fd = 0;
  buf = 0;
  eof = false;
}

// write
// OUTPUT STREAM
//

rostream::rostream(size_t size) {
  fd = 0;
}

rostream::rostream() {
  fd = 0;
}

rostream::~rostream() {
  close_stream();
}

void rostream::create_stream(const char *file) {
  fd = open(file, O_CREAT | O_LARGEFILE | O_WRONLY | O_TRUNC, S_IRWXU);
  if (fd < 0) {perror("Error creating stream");exit(errno);}
  // std::cout << "CREATED STREAM: " << file << " with fd: " << fd << std::endl;
}

void rostream::create_stream(const char *file, bool from_end) {
  if (!from_end) create_stream(file);
  else {
    fd = open(file, O_CREAT | O_LARGEFILE | O_WRONLY, S_IRWXU);
    if (fd < 0) {perror("Error creating stream");exit(errno);}
    lseek(fd,0,SEEK_END);
  }
}

void rostream::write_stream(int d) {
  write(fd, &d, sizeof(d));
}

void rostream::close_stream() {
  close(fd);
  // std::cout << "CLOSED STREAM: " << fd << std::endl;
  fd = 0;
}

// fread
// INPUT STREAM
//

fistream::fistream() {
  f = 0;
  buf = 0;
  eof = false;
}

fistream::fistream(size_t size) {
  f = 0;
  buf = 0;
  eof = false;
}

fistream::~fistream() {
  close_stream();
}

void fistream::open_stream(const char *file) {
  f = fopen(file, "rb");
  if (f == 0) {
    perror("Error opening file"); exit(errno);
  }
  read_next();
}

int fistream::read_next() {
  int res = buf;
  if (fread(&buf, sizeof(int), 1, f) == 0)
    eof = true;
  return res;
}

bool fistream::end_of_stream() {
  return eof;
}

void fistream::close_stream() {
  if (f == 0) return;
  fclose(f);
  f = 0;
}

size_t fistream::size() {
  std::cout << "fistream Missing implementation size"<< std::endl;
  exit(-1);
}

void fistream::use() {
  std::cout << "fistream Missing implementation use" <<std::endl;
  exit(-1);
}

/////// fistream back

fistream_back::fistream_back() {
  f = 0;
}

fistream_back::fistream_back(size_t size) {
  f = 0;
}

fistream_back::~fistream_back() {
  close_stream();
}

void fistream_back::open_stream(const char *file) {
  has_been_used = true;
  f = fopen(file, "rb");
  if (f == 0) {
    perror("Error opening file"); exit(errno);
  }
  buf = 0;
  eof = false;
  fseek(f, 0, SEEK_END);
  fill();
}

int fistream_back::read_next() {
  has_been_used = false;
  int res = buf;
  fill();
  return res;
}

void fistream_back::fill() {
  if (ftell(f) == 0) eof = true;
  fseek(f,-1*sizeof(int),SEEK_CUR);
  if (fread(&buf, sizeof(int), 1, f) == 0)
    eof = true;
  fseek(f,-1*sizeof(int),SEEK_CUR);
}

bool fistream_back::end_of_stream() {
  return eof;
}

void fistream_back::close_stream() {
  if (f == 0) return;
  fclose(f);
  f = 0;
}

size_t fistream_back::size() {
  long pos = ftell(f);
  return pos/sizeof(int) + !has_been_used + 1;
}

void fistream_back::use() {
  has_been_used = true;
}

// fwrite
// OUTPUT STREAM
//

fostream::fostream() {
  f = 0;
}

fostream::fostream(size_t size) {
  f = 0;
}

fostream::~fostream() {
  close_stream();
}

void fostream::create_stream(const char *file) {
  f = fopen(file, "wb");
}

void fostream::create_stream(const char *file, bool from_end) {
  f = fopen(file, "a");
}

void fostream::write_stream(int d) {
  fwrite(&d, sizeof(d), 1, f);
}

void fostream::close_stream() {
  if (f == 0) return;
  fclose(f);
  f = 0;
}

// read with buffer
// OUTPUT STREAM
//


bistream::bistream(int B) {
  this->B = B;
  fd = -1;
}

bistream::~bistream() {
  close_stream();
}

void bistream::open_stream(const char *file) {
  eof = false;
  index = B;
  _size = 0;
  fd = open(file, O_RDONLY | O_LARGEFILE);
  struct stat sb;
  fstat(fd,&sb);
  off64_t t = sb.st_size;
  fileSize = (long) t;
  fileSize /= sizeof(int);
  buf = new int[B];
  fill();
}

void bistream::fill() {
  //read in from file
  if (eof) return;
  if ( (_size = read(fd, buf, B*sizeof(int)) / sizeof(int)) < B)
    eof = true;
  fileSize -= _size;
  if (fileSize < 1) eof = true;
  index = 0;
}

int bistream::read_next() {
  if (index >= _size) {
    fill();
  }
  int b = buf[index++];
  return b;
}

bool bistream::end_of_stream() {
  return eof && index >= _size;
}

void bistream::close_stream() {
  if (fd == -1) return;
  close(fd);
  delete[] buf;
  fd = -1;
}

size_t bistream::size() {
  std::cout << "Missing implemetnation" << std::endl;
  exit(-1);
}

void bistream::use() {
  std::cout << "Missing implemetnation" << std::endl;
  exit(-1);
}

/////////////bi stream back

bistream_back::bistream_back(size_t B) {
  this->B = B;
  eof = false;
  index = B;
  _size = 0;
  fd = -1;
}

bistream_back::~bistream_back() {
  close_stream();
}

void bistream_back::open_stream(const char *file) {
  has_been_used = true;
  fd = open(file, O_RDONLY | O_LARGEFILE);
  struct stat sb;
  fstat(fd,&sb);
  off64_t t = sb.st_size;
  fileSize = (long) t;
  buf = new int[B];
  lseek(fd,0,SEEK_END);
  fill();
}

void bistream_back::fill() {
  //read in from file
  if (eof) return;
  if (B*sizeof(int) <= fileSize)
    lseek(fd, -1*(B*sizeof(int)), SEEK_CUR);
  else {
    lseek(fd, -1*fileSize, SEEK_CUR);
    eof = true;
  }
  _size = read(fd, buf, std::min((long long)fileSize,(long long)(B*sizeof(int))));
  lseek(fd, -1*std::min((long long)fileSize,(long long)(B*sizeof(int))), SEEK_CUR);

  fileSize -= _size;
  if (fileSize < 1) eof = true;
  index = _size/sizeof(int)-1;
}

int bistream_back::read_next() {
  has_been_used = false;
  if (index < 0) {
    fill();
  }
  int b = buf[index--];
  return b;
}

bool bistream_back::end_of_stream() {
  return eof && index < 0;
}

void bistream_back::close_stream() {
  if (fd == -1) return;
  close(fd);
  delete[] buf;
  fd = -1;
}

size_t bistream_back::size() {
  return fileSize/sizeof(int) + index + !has_been_used + 1;
}

void bistream_back::use() {
  has_been_used = true;
}


// write with buffer
// OUTPUT STREAM
//

bostream::bostream() {
  std::cout << "Should not use bostream without buffer!!!" << std::endl;
  exit(-1);
}

bostream::bostream(int B) {
  this->B = B;
  index = 0;
  fd = -1;
}

bostream::~bostream() {
  close_stream();
}

void bostream::create_stream(const char *file) {
  fd = open(file, O_CREAT | O_WRONLY | O_LARGEFILE | O_TRUNC, S_IRWXU);
  if (fd < 0) {perror("Error creating stream");exit(errno);}
  buf = new int[B];
}

void bostream::create_stream(const char *file, bool from_end) {
  if (!from_end) create_stream(file);
  else {
    fd = open(file, O_CREAT | O_LARGEFILE | O_WRONLY, S_IRWXU);
    if (fd < 0) {perror("Error creating stream");exit(errno);}
    lseek(fd,0,SEEK_END);
  }
  buf = new int[B];
}

void bostream::write_stream(int d) {
  if (index == B) {
    flush();
  }
  buf[index++] = d;
}

void bostream::flush() {
  write(fd, buf, index*sizeof(int));
  index = 0;
}

void bostream::close_stream() {
  if (fd == -1) return;
  //first flush the stream
  flush();
  delete[] buf;
  index = 0;
  close(fd);
  fd = -1;
}

// mmap
// INPUT STREAM
//

mistream::mistream(int b) {
  fd = -1;
  b*= sizeof(int);
  long page_size = sysconf(_SC_PAGESIZE);
  if (b%page_size != 0)
    bSize = b + page_size - (b%page_size);
  else bSize = b;
  // std::cout << "bSize: " << bSize << std::endl;
}

mistream::~mistream() {
  close_stream();
}

void mistream::open_stream(const char *file) {
  // std::cout << file << std::endl;
  fd = open(file, O_RDONLY | O_LARGEFILE);
  //b should be a multiple of the page size
  struct stat sb;
  fstat(fd, &sb);
  fileSize = sb.st_size;
  eof = false;
  buf_pos = 0;
  offset = 0;
  buf = 0;
  fill();
}

int mistream::read_next() {
  // std::cout << buf_pos << " " << buf_size << " " << eof << std::endl;
  if (buf_pos == buf_size) fill();
  return buf[buf_pos++];
}

void mistream::fill() {
  if (eof) return;
  //first unmap the previous buffer
  if (buf != 0) {
    munmap(buf, length);
    buf = 0;
  }
    
  //fill the buffer using mmap
  length = bSize;
  if (fileSize <= offset + length) {
    eof = true;
    length = fileSize - offset;
  }    

  buf = (int*)mmap(0, length, PROT_READ, MAP_SHARED, fd, offset);
  offset += length;
  buf_size = length/sizeof(int);
  buf_pos = 0;
}

bool mistream::end_of_stream() {
  return eof && buf_pos >= buf_size;
}

void mistream::close_stream() {
  if (fd == -1) return;
  munmap(buf, length);
  close(fd);
  fd = -1;
//  delete[] buf;

}

size_t mistream::size() {
  std::cout << "Missing implementaiton" << std::endl;
  exit(-1);
}

void mistream::use() {
  std::cout << "Missing implemetnatipon" << std::endl;
  exit(-1);
}

///////mistream back

mistream_back::mistream_back(size_t b) {
  fd = -1;
  b *= sizeof(int);
  long page_size = sysconf(_SC_PAGESIZE);
  if (b%page_size != 0)
    bSize = b + page_size - (b%page_size);
  else bSize = b;
}

mistream_back::~mistream_back() {
  close_stream();
}

void mistream_back::open_stream(const char *file) {
  has_been_used = true;
  fd = open(file, O_RDONLY | O_LARGEFILE);
  //b should be a multiple of the page size
  struct stat sb;
  fstat(fd, &sb);
  fileSize = sb.st_size;
  eof = false;
  buf_pos = (fileSize%bSize)/sizeof(int);
  offset = fileSize - (fileSize%bSize);
  buf = 0;
  fill();
}

int mistream_back::read_next() {
  has_been_used = false;
  if (buf_pos < 0) fill();
  return buf[buf_pos--];
}

void mistream_back::fill() {
  if (eof) return;
  //first unmap the previous buffer
  if (buf != 0) {
    munmap(buf, length);
    buf = 0;
  }
  //fill the buffer using mmap
  length = bSize;
  if (fileSize <= offset + length) {
    length = fileSize - offset;
  }
  
  buf = (int*)mmap(0, length, PROT_READ, MAP_SHARED, fd, offset);
  offset -= bSize;
  fileSize -= length;
  if (fileSize == 0) eof = true;
  buf_size = length/sizeof(int);
  buf_pos = buf_size-1;
}

bool mistream_back::end_of_stream() {
  return eof && buf_pos < 0;
}

void mistream_back::close_stream() {
  if (fd == -1) return;
  munmap(buf, length);
  close(fd);
  fd = -1;
}

size_t mistream_back::size() {
  return fileSize/sizeof(int) + buf_pos + !has_been_used + 1;
}

void mistream_back::use() {
  has_been_used = true;
}


//////////////////////

mostream::mostream(int b) {
  long page_size = sysconf(_SC_PAGESIZE);
  b *= sizeof(int);
  if (b%page_size != 0)
    bSize = b + page_size - (b%page_size);
  else bSize = b;
  bSize /= sizeof(int);
  fd = -1;
}
mostream::mostream() {
  std::cout << "Should not open mostream without buffer size" << std::endl;
  exit(-1);
}

mostream::~mostream() {
  close_stream();
}

void mostream::create_stream(const char *file) {
  fd = open(file, O_CREAT | O_TRUNC | O_RDWR | O_LARGEFILE, S_IRWXU);
  buf_pos = 0;
  fileSize = 0;  
  buf = new int[bSize];
}

void mostream::create_stream(const char *file, bool from_end) {
  if (!from_end) create_stream(file);
  else {
    fd = open(file, O_CREAT | O_LARGEFILE | O_RDWR, S_IRWXU);
    if (fd < 0) {perror("Error creating stream");exit(errno);}
    buf = new int[bSize];
    buf_pos = 0;
    struct stat statbuf;
    stat(file, &statbuf);
    fileSize = statbuf.st_size;
    int p = sysconf(_SC_PAGESIZE);
    int bytes_to_map = fileSize % p;
    fileSize -= bytes_to_map;
    int *temp_buf = (int*)mmap(0, bytes_to_map, PROT_READ | PROT_WRITE, MAP_SHARED, fd, fileSize);
    for (int i = 0; i < bytes_to_map/sizeof(int); i++) {
      buf[i] = temp_buf[i];
      buf_pos++;
    }
    munmap(temp_buf, bytes_to_map);
    ftruncate(fd,fileSize);
  }


}

void mostream::flush() {
  //write to disk - mmap and munmap file
  int numBytes = 0;
  if (buf_pos == bSize) numBytes = buf_pos*sizeof(int);
  else numBytes = (buf_pos)*sizeof(int);
  lseek(fd, numBytes, SEEK_END);
  write(fd, "", 1);
  wbuf = (int*)mmap(0, numBytes, PROT_WRITE, MAP_SHARED, fd, fileSize);
  for (size_t i = 0; i < buf_pos; i++) {
    int k = buf[i];
    wbuf[i] = k;
  }
  munmap(wbuf, numBytes);
  if (buf_pos != bSize)
    ftruncate(fd, lseek(fd,0,SEEK_END)-1);
  int p = sysconf(_SC_PAGESIZE);
  if (numBytes % p != 0)
    numBytes += p - (numBytes % p);
  wbuf = 0;
  buf_pos = 0;
  fileSize += numBytes;
}

void mostream::write_stream(int d) {
  if (buf_pos == bSize) flush();
  buf[buf_pos++] = d;
}

void mostream::close_stream() {
  if (fd == -1) return;
  if (buf_pos != 0) flush();
  close(fd);
  delete[] buf;
  buf = 0;
  fd = -1;
}
