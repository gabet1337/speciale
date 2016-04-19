CC = c++ -std=c++0x -O2 -Wall -I/usr/include/cppconn -L/usr/lib

PROGRAMS = insert_experiment

MYSQL = -lmysqlcppconn

FILES = ../common/definitions.hpp ../common/point.hpp ../common/pst_interface.hpp ../common/test_lib.hpp ../common/utilities.hpp ../common/debug.hpp ../common/validate.hpp ../external/arge_pst.hpp ../external/buffered_pst.hpp ../external/child_structure.hpp ../external/range.hpp ../external/child_structure_interface.hpp ../internal/boost_r_tree.hpp ../internal/dynamic_pst.hpp ../internal/mysql_pst.hpp ../internal/rb_tree.hpp ../stream/stream.hpp base_experiment.hpp result.hpp

insert_experiment: $(FILES) insert_experiment.cpp
	$(CC) insert_experiment.cpp -o insert_experiment $(MYSQL)

clean:
	$(RM) *.o *~ $(PROGRAMS)
