#!/usr/bin/gnuplot
set terminal postscript eps enhanced color font 'Verdana,11'
set output 'gerth_buffer_size_experiment_results/2016-05-03.13_51_54/ibo.eps'
set style line 11 lc rgb '#808080' lt 1
set border 3 back ls 11
set tics nomirror
set style line 12 lc rgb '#808080' lt 0 lw 1
set grid back ls 12
set style line 1 lc rgb '#8b1a0e' pi 4 pt 1 ps 1 lt 1 lw 2 # --- red
set style line 2 lc rgb '#5e9c36' pi 4 pt 6 ps 1 lt 1 lw 2 # --- green
set style line 3 lc rgb '#88419d' pi 4 pt 2 ps 1 lt 1 lw 2 # --- purple
set style line 4 lc rgb '#225ea8' pi 4 pt 3 ps 1 lt 1 lw 2 # --- blue
set style line 5 lc rgb '#000000' pi 4 pt 4 ps 1 lt 1 lw 2 # --- black
set style line 6 lc rgb '#00ced1' pi 4 pt 5 ps 1 lt 1 lw 2 # --- darkturquoise
set style line 7 lc rgb '#ff00ff' pi 4 pt 7 ps 1 lt 1 lw 2 # --- magenta
set style line 8 lc rgb '#87ceeb' pi 4 pt 8 ps 1 lt 1 lw 2 # --- skyblue
set key top left
set xlabel 'N (input size in Mb)'
set ylabel 'insert buffer overflows'
#set xrange [0:0]
#set yrange [0:0]
plot 'gerth_buffer_size_experiment_results/2016-05-03.13_51_54/gerth_buffer1MB' u 1:($8-90) t 'buffer1MB' w lp ls 2, \
'gerth_buffer_size_experiment_results/2016-05-03.13_51_54/gerth_buffer2MB' u 1:($8-29) t 'buffer2MB' w lp ls 3, \
'gerth_buffer_size_experiment_results/2016-05-03.13_51_54/gerth_buffer4MB' u 1:($8-137558) t 'buffer4MB' w lp ls 4, \
'gerth_buffer_size_experiment_results/2016-05-03.13_51_54/gerth_buffer8MB' u 1:($8-57180) t 'buffer8MB' w lp ls 5, \
'gerth_buffer_size_experiment_results/2016-05-03.13_51_54/gerth_buffer16MB' u 1:($8-160156) t 'buffer16MB' w lp ls 6
