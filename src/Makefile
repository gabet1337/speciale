CC = c++ -std=c++0x
DEBUG = -g -Wall -Wall -fsanitize=address
LFLAGS = $(DEBUG)
CFLAGS = $(DEBUG) -c

PROGRAMS = ipst

POINT = common/point.hpp

INTERNAL_PRIORITY_SEARCH_TREE = internal/priority_search_tree.hpp

ipst : $(POINT) $(INTERNAL_PRIORITY_SEARCH_TREE)
	$(CC) $(LFLAGS) $(POINT) $(INTERNAL_PRIORITY_SEARCH_TREE) internal/test_priority_search_tree.cpp -o ipst

clean:
	$(RM) *.o *~ $(PROGRAMS)
