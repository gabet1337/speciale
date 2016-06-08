DEPENDENCIES:
=============

- Linux machine (tested on ubuntu)
- [Boost 1.60 or newer](http://www.boost.org/users/history/version_1_60_0.html)
- [libspatialindex](https://github.com/libspatialindex/libspatialindex/wiki/1.-Getting-Started)
- [MySQL 5.7 or newer](http://dev.mysql.com/doc/refman/5.7/en/installing.html)
  - There must be a table called TESTDB to run tests: 'CREATE DATABASE IF NOT EXISTS TESTDB'
- libmysqlcppconn-dev (can be installed using 'sudo apt-get install libmysqlcppconn-dev')
- gcc c++ compiler (tested on 5.3.1-14 but might work on older)

HOW TO COMPILE UNIT TESTS:
==========================

Use the Makefile located in src/ folder.

Targets:
--------

all targets should me made with DOFF= to avoid debugging output

- stream_test (unit test of the stream implementation)
- ctest includes:
  - ctest1 (unit test of the child structure/query data structure)
  - ctest2 (unit test of the child structure stub)

- epst (test of the External Memory Buffered Priority Search Tree by Brodal)
- apst (test of the External Memory Priority Search Tree by Arge et al.)
- mpst (test of the MySQL Priority Search Tree)
- dpst (test of the Internal Memory Priority Search Tree by McCreight)
- rtree (test of the Boost R-Tree)
- libspatial_tree (test of the libspatialindex R*Tree)
- testlib (test of the test library)

HOW TO COMPILE EXPERIMENTS:
===========================

Use the Makefile located in src/experiments folder

It is important that you generate necessary test data before hand:
Compile and run the programs located in src/data with -std=c++11

There is a shell script called delfol.sh you can run if you feel like you have too many folders.

Targets:
--------

The target are somewhat self explanatory

- insert_experiment
- delete_experiment
- query_experiment
- query_experiment_report_50mb
- arge_buffer_size_experiment
- gerth_buffer_size_experiment
- gerth_fanout_experiment
- gerth_query_fanout_experiment
- stream_input_speed_experiment
- stream_output_speed_experiment
