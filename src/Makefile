CC = c++ -std=c++0x -O2
DOFF = -DDEBUG
DEBUG = -g -Wall -fsanitize=address $(DOFF)
LFLAGS = $(DEBUG)
CFLAGS = $(DEBUG) -c

PROGRAMS = ipst stream_test

POINT = common/point.hpp
STREAM = stream/stream.hpp
INTERNAL_PRIORITY_SEARCH_TREE = internal/priority_search_tree.hpp

ipst : $(POINT) $(INTERNAL_PRIORITY_SEARCH_TREE)
	$(CC) $(LFLAGS) $(POINT) $(INTERNAL_PRIORITY_SEARCH_TREE) internal/test_priority_search_tree.cpp -o ipst

stream_test : $(STREAM) $(POINT) stream/test_stream.cpp
	$(CC) $(LFLAGS) $(STREAM) $(POINT) stream/test_stream.cpp -o stream_test

clean:
	$(RM) *.o *~ $(PROGRAMS)
