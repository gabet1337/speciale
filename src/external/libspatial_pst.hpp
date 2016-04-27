#ifndef LIBSPATIAL_PST_HPP
#define LIBSPATIAL_PST_HPP

#include "../common/point.hpp"
#include "../common/utilities.hpp"
#include "../common/definitions.hpp"
#include "../common/pst_interface.hpp"
#include "../stream/stream.hpp"
#include <math.h>
#include <spatialindex/SpatialIndex.h>
#include <spatialindex/capi/ObjVisitor.h>
namespace ext {
  using namespace SpatialIndex;
  
  class libspatial_pst : public common::pst_interface {

  public:
    libspatial_pst();
    libspatial_pst(size_t buffer_size);
    libspatial_pst(size_t buffer_size, double epsilon);
    ~libspatial_pst();
    void insert(const point &p);
    void remove(const point &p);
    void report(int x1, int x2, int y, const std::string &output_file);
    void print();
  private:
    size_t buffer_size;
    
    class stream_visitor : public IVisitor {
    private:
      io::buffered_stream<point> *stream = 0;
    public:
      stream_visitor(size_t buffer_size, const std::string &output_file) {
        stream = new io::buffered_stream<point>(buffer_size);
        stream->open(output_file);
      }

      ~stream_visitor() {
        stream->close();
        delete stream;
      }

      void visitNode(const INode& n) { }

      void visitData(const IData& d) {
        IShape* s = 0;
        d.getShape(&s);
        Point p;
        s->getCenter(p);
        delete s;
        double* coords = p.m_pCoords;
        stream->write(point((int)coords[0], (int)coords[1]));
      }

      void visitData(std::vector<const IData*>& v) { }

    };

    ISpatialIndex* tree;
    IStorageManager* diskfile;
    StorageManager::IBuffer* file;
    std::string baseName = "libspatialDSM";

    int64_t combine(int64_t a, int64_t b) {
      int64_t times = 1;
      if ( b != 0 ) {
        times = (int64_t)pow(10.0, (double)((int64_t)log10((double)b)) + 1.0);
      }
      return a * times + b;
    }
  };

  libspatial_pst::libspatial_pst() : libspatial_pst(4096) {}

  libspatial_pst::libspatial_pst(size_t buffer_size) {

    diskfile = StorageManager::createNewDiskStorageManager(baseName, buffer_size);
    file = StorageManager::createNewRandomEvictionsBuffer(*diskfile, 100, false);
    id_type indexIdentifier;
    tree = RTree::createNewRTree(*file, 0.7, 100, 100, 2, SpatialIndex::RTree::RV_RSTAR, indexIdentifier);
    this->buffer_size = buffer_size;
  }

  libspatial_pst::libspatial_pst(size_t buffer_size, double epsilon)
    : libspatial_pst(buffer_size)
  {}

  libspatial_pst::~libspatial_pst() {
    delete tree;
    delete file;
    delete diskfile;
    util::remove_file(baseName+".dat");
    util::remove_file(baseName+".idx");
  }

  void libspatial_pst::insert(const point &p) {
    double coords[] = {(double)p.x, (double)p.y};
    uint8_t* pData = 0;
    uint32_t nDataLength = 0;
    SpatialIndex::IShape* shape = new SpatialIndex::Point(coords,2);
    int64_t id = combine(p.x, p.y);
    tree->insertData(nDataLength, pData, *shape, id);
    delete shape;
  }

  void libspatial_pst::remove(const point &p) {
    double coords[] = {(double)p.x, (double)p.y};
    SpatialIndex::IShape* shape = new SpatialIndex::Point(coords,2);
    tree->deleteData(*shape, combine(p.x, p.y));
    delete shape;
  }

  void libspatial_pst::report(int x1, int x2, int y, const std::string &output_file) {
    stream_visitor visitor(buffer_size, output_file);
    double plow[2] = {(double)x1,(double)y};
    double phigh[2] = {(double)x2,(double)INF};
    Region r(plow, phigh, 2);
    tree->intersectsWithQuery(r, visitor);
  }

  void libspatial_pst::print() { }
};
#endif
