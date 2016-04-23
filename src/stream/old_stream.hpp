#ifndef OLD_STREAM_HPP
#define OLD_STREAM_HPP
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

namespace io {

  class Iistream {
  public:
    virtual ~Iistream() {}
    virtual void open_stream(const char *file) = 0;
    virtual int read_next() = 0;
    virtual bool end_of_stream() = 0;
    virtual void close_stream() = 0;
    virtual off64_t size() = 0;
    virtual void use() = 0;
  };

  class Iostream {
  public:
    virtual ~Iostream() {}
    virtual void create_stream(const char *file) = 0;
    virtual void create_stream(const char *file, bool from_end) = 0;
    virtual void write_stream(int d) = 0;
    virtual void close_stream() = 0;
  };

  class ristream : public Iistream {
  public:
    ristream();
    ristream(size_t size);
    ~ristream();
    virtual void open_stream(const char *file);
    virtual int read_next();
    virtual bool end_of_stream();
    virtual void close_stream();
    virtual off64_t size();
    virtual void use();
  private:
    int fd,buf;
    bool eof;
    void fill();
  };

  class rostream : public Iostream {
  public:
    rostream();
    rostream(size_t size);
    ~rostream();
    virtual void create_stream(const char *file);
    virtual void create_stream(const char *file, bool from_end);
    virtual void write_stream(int d);
    virtual void close_stream();
  private:
    int fd;
  };

  class fistream : public Iistream {
  public:
    fistream();
    fistream(size_t size);
    ~fistream();
    virtual void open_stream(const char *file);
    virtual int read_next();
    virtual bool end_of_stream();
    virtual void close_stream();
    virtual off64_t size();
    virtual void use();
  private:
    FILE *f;
    int buf;
    bool eof;
  };

  class fistream_back : public Iistream {
  public:
    fistream_back();
    fistream_back(size_t size);
    ~fistream_back();
    virtual void open_stream(const char *file);
    virtual int read_next();
    virtual bool end_of_stream();
    virtual void close_stream();
    virtual off64_t size();
    virtual void use();
  private:
    void fill();
    bool has_been_used;
    FILE *f;
    int buf;
    bool eof;  
  };

  class fostream : public Iostream {
  public:
    fostream();
    fostream(size_t size);
    ~fostream();
    virtual void create_stream(const char *file);
    virtual void create_stream(const char *file, bool from_end);
    virtual void write_stream(int d);
    virtual void close_stream();
  private:
    FILE *f;
  };

  class bistream : public Iistream {
  public:
    bistream(int B);
    ~bistream();
    virtual void open_stream(const char *file);
    virtual int read_next();
    virtual bool end_of_stream();
    virtual void close_stream();
    virtual off64_t size();
    virtual void use();
  private:
    void fill();
    int _size,fd,B,*buf, index;
    bool eof;
    off64_t fileSize;
  };

  class bostream : public Iostream {
  public:
    bostream(int b);
    bostream();
    ~bostream();
    virtual void create_stream(const char *file);
    virtual void create_stream(const char *file, bool from_end);
    virtual void write_stream(int d);
    virtual void close_stream();
  private:
    void flush();
    int B,*buf, index,size,fd;
  };

  class mistream : public Iistream {
  public:
    mistream(int b);
    ~mistream();
    virtual void open_stream(const char *file);
    virtual int read_next();
    virtual bool end_of_stream();
    virtual void close_stream();
    virtual off64_t size();
    virtual void use();
  private:
    void fill();
    size_t bSize,buf_size;
    int *buf;
    size_t buf_pos;
    off64_t length;
    int fd;
    bool eof;
    off64_t offset; // must be multiple of page_size returned by sysconf(_SC_PAGE_SIZE)
    off64_t fileSize;
  };

  class mostream : public Iostream {
  public:
    mostream(int b);
    mostream();
    ~mostream();
    virtual void create_stream(const char *file);
    virtual void create_stream(const char *file, bool from_end);
    virtual void write_stream(int d);
    virtual void close_stream();
  private:
    void flush();
    size_t bSize;
    int *buf, *wbuf;
    int fd;
    size_t buf_pos;
    off64_t fileSize;
  };

}
#endif
