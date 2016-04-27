CC = c++ -std=c++0x -O2 -D_FILE_OFFSET_BITS=64
DOFF = -DDEBUG common/debug.hpp 
VALID = -DVALIDATE common/validate.hpp
DEBUG = -g -Wall -fsanitize=address $(DOFF) $(VALID) 
LFLAGS = $(DEBUG)
CFLAGS = $(DEBUG) -c

PROGRAMS = ipst stream_test ctest1 ctest2 ctest epst apst rtree mpst dpst testlib testexp libspatial_pst

DEFINITIONS = common/definitions.hpp
UTILITIES = common/utilities.hpp
POINT = common/point.hpp
RANGE = external/range.hpp
STREAM = stream/stream.hpp
RB = internal/rb_tree.hpp
TEST_LIB = common/test_lib.hpp
EXTERNAL_CHILD_STRUCTURE = external/child_structure.hpp external/child_structure_interface.hpp
EXTERNAL_CHILD_STRUCTURE_STUB = external/child_structure_stub.hpp external/child_structure_interface.hpp
BUFFERED_PST = external/buffered_pst.hpp common/pst_interface.hpp
EXTERNAL_PST = external/arge_pst.hpp common/pst_interface.hpp
RTREE = internal/boost_r_tree.hpp common/pst_interface.hpp

stream_test : $(STREAM) $(POINT) $(UTILITIES) stream/test_stream.cpp
	$(CC) $(LFLAGS) $(STREAM) $(POINT) $(UTILITIES) stream/test_stream.cpp -o stream_test

ctest1 : $(STREAM) $(POINT) $(RB) $(DEFINITIONS) $(EXTERNAL_CHILD_STRUCTURE) $(TEST_LIB) $(UTILITIES) external/test_child_structure.cpp
	$(CC) $(LFLAGS) $(STREAM) $(DEFINITIONS) $(POINT) $(RB) $(EXTERNAL_CHILD_STRUCTURE) $(TEST_LIB) $(UTILITIES) external/test_child_structure.cpp -o ctest1

ctest2 : $(STREAM) $(POINT) $(RB) $(DEFINITIONS) $(EXTERNAL_CHILD_STRUCTURE_STUB) $(TEST_LIB) $(UTILITIES) external/test_child_structure_stub.cpp
	$(CC) $(LFLAGS) $(STREAM) $(DEFINITIONS) $(POINT) $(RB) $(EXTERNAL_CHILD_STRUCTURE_STUB) $(TEST_LIB) $(UTILITIES) external/test_child_structure_stub.cpp -o ctest2

ctest : clean ctest1 ctest2
	./ctest1 && ./ctest2

epst : $(POINT) $(RANGE) $(STREAM) $(RB) $(DEFINITIONS) $(BUFFERED_PST) $(EXTERNAL_CHILD_STRUCTURE) $(UTILITIES) $(EXTERNAL_CHILD_STRUCTURE_STUB) external/test_buffered_pst.cpp
	$(CC) $(LFLAGS) external/test_buffered_pst.cpp -o epst

apst : $(POINT) $(EXTERNAL_PST) $(STREAM) $(DEFINITIONS) $(EXTERNAL_CHILD_STRUCTURE) $(UTILITIES) external/test_arge_pst.cpp
	$(CC) $(LFLAGS) external/test_arge_pst.cpp -o apst


mpst:	internal/test_mysql_pst.cpp internal/mysql_pst.hpp common/pst_interface.hpp $(STREAM)
	$(CC) $(LFLAGS) -I/usr/include/cppconn internal/test_mysql_pst.cpp -L/usr/lib -lmysqlcppconn -o mpst

dpst:	internal/test_dynamic_pst.cpp internal/dynamic_pst.hpp common/pst_interface.hpp $(STREAM)
	$(CC) $(LFLAGS) internal/test_dynamic_pst.cpp -o dpst

rtree : $(RTREE) $(STREAM) internal/test_boost_r_tree.cpp
	$(CC) $(LFLAGS) internal/test_boost_r_tree.cpp -o rtree

libspatial_tree: $(STREAM) $(UTILITIES) $(DEFINITIONS) $(POINT) external/libspatial_pst.hpp external/test_libspatial_pst.cpp
	$(CC) $(LFLAGS) external/test_libspatial_pst.cpp -lspatialindex_c -lspatialindex -o libspatial_pst

testlib: $(TEST_LIB) common/test_test_lib.cpp
	$(CC) $(LFLAGS) common/test_test_lib.cpp -o testlib

testexp : $(TEST_LIB) experiments/base_experiment.hpp experiments/test_base.cpp experiments/result.hpp
	$(CC) $(LFLAGS) -I/usr/include/cppconn experiments/test_base.cpp -L/usr/lib -lmysqlcppconn -o testexp

testall : clean	stream_test ctest epst
	./stream_test && ./ctest && ./epst

clean:
	$(RM) *.o *~ $(PROGRAMS)
