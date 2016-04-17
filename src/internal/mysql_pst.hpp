#ifndef MYSQL_PST_HPP
#define MYSQL_PST_HPP
#include "../common/pst_interface.hpp"
#include "../common/point.hpp"
#include "../stream/stream.hpp"
#include <string>
#include <iostream>
#include <vector>

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
    std::string get_values(const std::vector<point> &buffer);
    void flush_insert_buffer();
    void flush_delete_buffer();
    size_t buffer_size;
    Driver *driver;
    Connection *con;
    Statement *stmt;
    ResultSet *res;
    std::vector<point> insert_buffer;
    std::vector<point> delete_buffer;
  };


  mysql_pst::mysql_pst() {
    buffer_size = 4096;
    driver = get_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "root", "thesis2016");
    con->setSchema("TESTDB");
    stmt = con->createStatement();

    //create a table for use:
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(x int, y int) ENGINE=InnoDB");
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
    insert_buffer.push_back(p);
    if (insert_buffer.size() > buffer_size)
      flush_insert_buffer();
  }

  std::string mysql_pst::get_values(const std::vector<point> &buffer) {
    std::string result = "";
    point p;
    for (size_t i = 0; i < buffer.size(); i++) {
      p = buffer[i];
      result += "("+std::to_string(p.x)+","+std::to_string(p.y)+")";
      if (i != buffer.size()-1) result += ", ";
      else result += ";";
    }
    return result;
  }

  void mysql_pst::flush_insert_buffer() {
    if (insert_buffer.empty()) return;
    stmt->execute("INSERT INTO test VALUES "+ get_values(insert_buffer));
    insert_buffer.clear();
  }

  void mysql_pst::flush_delete_buffer() {
    if (delete_buffer.empty()) return;
    std::string values = get_values(delete_buffer);
    values[values.size()-1] = ')';
    values += ";";
    stmt->execute("DELETE FROM test WHERE (x,y) IN ( "+ values);
    delete_buffer.clear();
  }

  void mysql_pst::remove(const point &p) {
    delete_buffer.push_back(p);
    if (delete_buffer.size() > buffer_size)
      flush_delete_buffer();
  }

  void mysql_pst::report(int x1, int x2, int y, const std::string &output_file) {
    flush_insert_buffer();
    flush_delete_buffer();
    io::buffered_stream<point> output(buffer_size);
    output.open(output_file);
    std::string x1s = std::to_string(x1);
    std::string x2s = std::to_string(x2);
    std::string ys = std::to_string(y);
    std::string q = "select * from test where x >= " + x1s + " AND " + x2s + " >= x AND y >= " + ys;
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
