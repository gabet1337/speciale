#ifndef MYSQL_PST_HPP
#define MYSQL_PST_HPP
#include "../common/pst_interface.hpp"
#include "../common/point.hpp"
#include "../stream/stream.hpp"
#include <string>
#include <iostream>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

namespace internal {
  using namespace sql;
  class mysql_pst : public common::pst_interface {

  public:
    mysql_pst();
    mysql_pst(size_t buffer_size, double epsilon);
    ~mysql_pst();
    void insert(const point &p);
    void remove(const point &p);
    void report(int x1, int x2, int y, const std::string &output_file);
    void print();
  private:
    size_t buffer_size;
    Driver *driver;
    Connection *con;
    Statement *stmt;
    ResultSet *res;
  };


  mysql_pst::mysql_pst() {
    buffer_size = 4096;
    driver = get_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "root", "thesis2016");
    con->setSchema("TESTDB");
    stmt = con->createStatement();

    //create a table for use:
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(pt POINT)");
  }

  mysql_pst::mysql_pst(size_t buffer_size, double epsilon) : mysql_pst() {
    this->buffer_size = buffer_size;
  }

  mysql_pst::~mysql_pst() {
    // delete res;
    // delete stmt;
    // delete con;
  }

  void mysql_pst::insert(const point &p) {
    stmt->execute("INSERT INTO test VALUES (GeomFromText('point("+std::to_string(p.x)+" "+std::to_string(p.y)+")'));");
  }

  void mysql_pst::remove(const point &p) {
    stmt->execute("DELETE FROM test WHERE st_equals(GeomFromText('point("+std::to_string(p.x)+" "+std::to_string(p.y)+")'), pt);");
  }

  void mysql_pst::report(int x1, int x2, int y, const std::string &output_file) {
    io::buffered_stream<point> output(buffer_size);
    output.open(output_file);
    std::string x1s = std::to_string(x1);
    std::string x2s = std::to_string(x2);
    std::string ys = std::to_string(y);
    std::string inf = std::to_string(2147483647);
    std::string box = x1s+" "+ys+", "+x2s+" "+ys+", "+x2s+" "+inf+", "+x1s+" "+inf+", "+x1s+" "+ys;
    stmt->execute("SET @box = st_geomfromtext('POLYGON(("+box+"))');");
    std::string q = "SELECT X(pt), Y(pt) FROM test WHERE st_intersects(pt,@box);";
    res = stmt->executeQuery(q);
    while (res->next()) {
      output.write(point(res->getInt(1), res->getInt(2)));
    }
    output.close();
  }

  void mysql_pst::print() {

  }


};

#endif
