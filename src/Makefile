CC = c++ -std=c++0x -O2
DOFF = -DDEBUG
VALID = -DVALIDATE
DEBUG = -g -Wall -fsanitize=address $(DOFF) $(VALID) common/debug.hpp
LFLAGS = $(DEBUG)
CFLAGS = $(DEBUG) -c

PROGRAMS = ipst stream_test ctest1 ctest2 ctest epst

UTILITIES = common/utilities.hpp
POINT = common/point.hpp
RANGE = external/range.hpp
STREAM = stream/stream.hpp
RB = internal/rb_tree.hpp
TEST_LIB = common/test_lib.hpp
INTERNAL_PRIORITY_SEARCH_TREE = internal/priority_search_tree.hpp
EXTERNAL_CHILD_STRUCTURE = external/child_structure.hpp external/child_structure_interface.hpp
EXTERNAL_CHILD_STRUCTURE_STUB = external/child_structure_stub.hpp external/child_structure_interface.hpp
BUFFERED_PST = external/buffered_pst.hpp

ipst : $(POINT) $(INTERNAL_PRIORITY_SEARCH_TREE) internal/test_priority_search_tree.cpp
	$(CC) $(LFLAGS) $(POINT) $(INTERNAL_PRIORITY_SEARCH_TREE) internal/test_priority_search_tree.cpp -o ipst

stream_test : $(STREAM) $(POINT) $(UTILITIES) stream/test_stream.cpp
	$(CC) $(LFLAGS) $(STREAM) $(POINT) $(UTILITIES) stream/test_stream.cpp -o stream_test

ctest1 : $(STREAM) $(POINT) $(RB) $(EXTERNAL_CHILD_STRUCTURE) $(TEST_LIB) $(UTILITIES) external/test_child_structure.cpp
	$(CC) $(LFLAGS) $(STREAM) $(POINT) $(RB) $(EXTERNAL_CHILD_STRUCTURE) $(TEST_LIB) $(UTILITIES) external/test_child_structure.cpp -o ctest1

ctest2 : $(STREAM) $(POINT) $(RB) $(EXTERNAL_CHILD_STRUCTURE_STUB) $(TEST_LIB) $(UTILITIES) external/test_child_structure_stub.cpp
	$(CC) $(LFLAGS) $(STREAM) $(POINT) $(RB) $(EXTERNAL_CHILD_STRUCTURE_STUB) $(TEST_LIB) $(UTILITIES) external/test_child_structure_stub.cpp -o ctest2

ctest : clean ctest1 ctest2
	./ctest1 && ./ctest2

epst : $(POINT) $(RANGE) $(STREAM) $(RB) $(BUFFERED_PST) $(EXTERNAL_CHILD_STRUCTURE) $(UTILITIES) $(EXTERNAL_CHILD_STRUCTURE_STUB) external/test_buffered_pst.cpp
	$(CC) $(LFLAGS) $(RANGE) $(POINT) $(RB) $(STREAM) $(BUFFERED_PST) $(EXTERNAL_CHILD_STRUCTURE) $(EXTERNAL_CHILD_STRUCTURE_STUB) $(UTILITIES) external/test_buffered_pst.cpp -o epst


testall : clean	stream_test ctest epst
	./stream_test && ./ctest && ./epst

clean:
	$(RM) *.o *~ $(PROGRAMS)
