CC = c++ -std=c++0x -O2
DOFF = -DDEBUG
VALID = -DVALIDATE
DEBUG = -g -Wall -fsanitize=address $(DOFF) $(VALID) common/debug.hpp
LFLAGS = $(DEBUG)
CFLAGS = $(DEBUG) -c

PROGRAMS = ipst stream_test ctest

POINT = common/point.hpp
STREAM = stream/stream.hpp
INTERNAL_PRIORITY_SEARCH_TREE = internal/priority_search_tree.hpp
EXTERNAL_CHILD_STRUCTURE = external/child_structure.hpp

ipst : $(POINT) $(INTERNAL_PRIORITY_SEARCH_TREE)
	$(CC) $(LFLAGS) $(POINT) $(INTERNAL_PRIORITY_SEARCH_TREE) internal/test_priority_search_tree.cpp -o ipst

stream_test : $(STREAM) $(POINT) stream/test_stream.cpp
	$(CC) $(LFLAGS) $(STREAM) $(POINT) stream/test_stream.cpp -o stream_test

ctest : $(STREAM) $(POINT) $(EXTERNAL_CHILD_STRUCTURE) external/test_child_structure.cpp
	$(CC) $(LFLAGS) $(POINT) $(STREAM) $(EXTERNAL_CHILD_STRUCTURE) external/test_child_structure.cpp -o ctest

clean:
	$(RM) *.o *~ $(PROGRAMS)
