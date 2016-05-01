CC = c++ -std=c++0x -D_FILE_OFFSET_BITS=64 -O2 -Wall -I/usr/include/cppconn -L/usr/lib

PROGRAMS = insert_experiment gerth_fanout_experiment stream_input_speed_experiment gerth_buffer_size_experiment stream_output_speed_experiment

MYSQL = -lmysqlcppconn -lspatialindex_c -lspatialindex

STREAM_STUBS = stream_stubs/buffered_stream_stub.hpp stream_stubs/f_stream_stub.hpp stream_stubs/mmap_stream_stub.hpp stream_stubs/sys_stream_stub.hpp ../stream/old_stream.hpp ../stream/old_stream.cpp

FILES = ../common/definitions.hpp ../common/point.hpp ../common/pst_interface.hpp ../common/test_lib.hpp ../common/utilities.hpp ../common/debug.hpp ../common/validate.hpp ../external/arge_pst.hpp ../external/buffered_pst.hpp ../external/child_structure.hpp ../external/range.hpp ../external/child_structure_interface.hpp ../internal/boost_r_tree.hpp ../internal/dynamic_pst.hpp ../internal/mysql_pst.hpp ../internal/rb_tree.hpp ../stream/stream.hpp base_experiment.hpp result.hpp $(STREAM_STUBS)

insert_experiment: $(FILES) insert_experiment.cpp
	$(CC) ../stream/old_stream.cpp insert_experiment.cpp -o insert_experiment $(MYSQL)

gerth_fanout_experiment: $(FILES) gerth_fanout_experiment.cpp
	$(CC) ../stream/old_stream.cpp gerth_fanout_experiment.cpp -o gerth_fanout_experiment $(MYSQL)

gerth_buffer_size_experiment: $(FILES) gerth_buffer_size_experiment.cpp
	$(CC) ../stream/old_stream.cpp gerth_buffer_size_experiment.cpp -o gerth_buffer_size_experiment $(MYSQL)

arge_buffer_size_experiment: $(FILES) arge_buffer_size_experiment.cpp
	$(CC) ../stream/old_stream.cpp arge_buffer_size_experiment.cpp -o arge_buffer_size_experiment $(MYSQL)

stream_input_speed_experiment: $(FILES) stream_input_speed_experiment.cpp
	$(CC) ../stream/old_stream.cpp stream_input_speed_experiment.cpp -o stream_input_speed_experiment $(MYSQL)

stream_output_speed_experiment: $(FILES) stream_output_speed_experiment.cpp
	$(CC) ../stream/old_stream.cpp stream_output_speed_experiment.cpp -o stream_output_speed_experiment $(MYSQL)

clean:
	$(RM) *.o *~ $(PROGRAMS)
