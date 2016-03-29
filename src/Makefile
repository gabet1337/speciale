CC = c++ -std=c++0x -O2
DOFF = -DDEBUG
VALID = -DVALIDATE
DEBUG = -g -Wall -fsanitize=address $(DOFF) $(VALID) common/debug.hpp common/validate.hpp
LFLAGS = $(DEBUG)
CFLAGS = $(DEBUG) -c

PROGRAMS = ipst stream_test ctest1 ctest2 ctest epst

DEFINITIONS = common/definitions.hpp
UTILITIES = common/utilities.hpp
POINT = common/point.hpp
RANGE = external/range.hpp
STREAM = stream/stream.hpp
RB = internal/rb_tree.hpp
TEST_LIB = common/test_lib.hpp
INTERNAL_PRIORITY_SEARCH_TREE = internal/balanced_pst.hpp
EXTERNAL_CHILD_STRUCTURE = external/child_structure.hpp external/child_structure_interface.hpp
EXTERNAL_CHILD_STRUCTURE_STUB = external/child_structure_stub.hpp external/child_structure_interface.hpp
BUFFERED_PST = external/buffered_pst.hpp external/pst_interface.hpp
EXTERNAL_PST = external/arge_pst.hpp external/pst_interface.hpp

ipst : $(POINT) $(INTERNAL_PRIORITY_SEARCH_TREE) internal/test_priority_search_tree.cpp
	$(CC) $(LFLAGS) $(POINT) $(INTERNAL_PRIORITY_SEARCH_TREE) internal/test_priority_search_tree.cpp -o ipst

stream_test : $(STREAM) $(POINT) $(UTILITIES) stream/test_stream.cpp
	$(CC) $(LFLAGS) $(STREAM) $(POINT) $(UTILITIES) stream/test_stream.cpp -o stream_test

ctest1 : $(STREAM) $(POINT) $(RB) $(DEFINITIONS) $(EXTERNAL_CHILD_STRUCTURE) $(TEST_LIB) $(UTILITIES) external/test_child_structure.cpp
	$(CC) $(LFLAGS) $(STREAM) $(DEFINITIONS) $(POINT) $(RB) $(EXTERNAL_CHILD_STRUCTURE) $(TEST_LIB) $(UTILITIES) external/test_child_structure.cpp -o ctest1

ctest2 : $(STREAM) $(POINT) $(RB) $(DEFINITIONS) $(EXTERNAL_CHILD_STRUCTURE_STUB) $(TEST_LIB) $(UTILITIES) external/test_child_structure_stub.cpp
	$(CC) $(LFLAGS) $(STREAM) $(DEFINITIONS) $(POINT) $(RB) $(EXTERNAL_CHILD_STRUCTURE_STUB) $(TEST_LIB) $(UTILITIES) external/test_child_structure_stub.cpp -o ctest2

ctest : clean ctest1 ctest2
	./ctest1 && ./ctest2

epst : $(POINT) $(RANGE) $(STREAM) $(RB) $(DEFINITIONS) $(BUFFERED_PST) $(EXTERNAL_CHILD_STRUCTURE) $(UTILITIES) $(EXTERNAL_CHILD_STRUCTURE_STUB) external/test_buffered_pst.cpp
	$(CC) $(LFLAGS) $(RANGE) $(POINT) $(DEFINITIONS) $(RB) $(STREAM) $(BUFFERED_PST) $(EXTERNAL_CHILD_STRUCTURE) $(EXTERNAL_CHILD_STRUCTURE_STUB) $(UTILITIES) external/test_buffered_pst.cpp -o epst

bpst : $(POINT) $(INTERNAL_PRIORITY_SEARCH_TREE) internal/test_balanced_pst.cpp
	$(CC) $(LFLAGS) $(POINT) $(INTERNAL_PRIORITY_SEARCH_TREE) internal/test_balanced_pst.cpp -o bpst

apst : $(POINT) $(EXTERNAL_PST) $(STREAM) $(DEFINITIONS) $(EXTERNAL_CHILD_STRUCTURE) $(UTILITIES) external/test_arge_pst.cpp
	$(CC) $(LFLAGS) $(POINT) $(EXTERNAL_PST) $(STREAM) $(DEFINITIONS) $(EXTERNAL_CHILD_STRUCTURE) $(UTILITIES) external/test_arge_pst.cpp -o apst

testall : clean	stream_test ctest epst
	./stream_test && ./ctest && ./epst

clean:
	$(RM) *.o *~ $(PROGRAMS)
