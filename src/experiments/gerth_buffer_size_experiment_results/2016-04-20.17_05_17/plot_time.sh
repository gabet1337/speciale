#!/usr/bin/gnuplot
set terminal postscript eps enhanced color font 'Verdana,9'
set output 'gerth_buffer_size_experiment_results/2016-04-20.17:05:17/time'
set style line 11 lc rgb '#808080' lt 1
set border 3 back ls 11
set tics nomirror
set style line 12 lc rgb '#808080' lt 0 lw 1
set grid back ls 12
set style line 1 lc rgb '#8b1a0e' pt 1 ps 1 lt 1 lw 2 # --- red
set style line 2 lc rgb '#5e9c36' pt 6 ps 1 lt 1 lw 2 # --- green
set style line 3 lc rgb '#88419d' pt 2 ps 1 lt 1 lw 2 # --- purple
set style line 4 lc rgb '#225ea8' pt 3 ps 1 lt 1 lw 2 # --- blue
set style line 5 lc rgb '#000000' pt 4 ps 1 lt 1 lw 2 # --- black
set key bottom right
set xlabel 'N (input size)'
set ylabel 'Time (s)'
plot '/home/overnight/speciale/src/experiments/gerth_buffer_size_experiment_results/2016-04-20.17:05:17/gerth_buffer1MB' u 1:2 t 'buffer1MB' w lp ls 2, \
'/home/overnight/speciale/src/experiments/gerth_buffer_size_experiment_results/2016-04-20.17:05:17/gerth_buffer2MB' u 1:2 t 'buffer2MB' w lp ls 2, \
'/home/overnight/speciale/src/experiments/gerth_buffer_size_experiment_results/2016-04-20.17:05:17/gerth_buffer4MB' u 1:2 t 'buffer4MB' w lp ls 2, \
'/home/overnight/speciale/src/experiments/gerth_buffer_size_experiment_results/2016-04-20.17:05:17/gerth_buffer8MB' u 1:2 t 'buffer8MB' w lp ls 2, \
'/home/overnight/speciale/src/experiments/gerth_buffer_size_experiment_results/2016-04-20.17:05:17/gerth_buffer16MB' u 1:2 t 'buffer16MB' w lp ls 2