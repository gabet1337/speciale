#!/usr/bin/gnuplot
set terminal postscript eps enhanced color font 'Verdana,12'
set output 'insert_experiment_results/2016-06-03.10_06_00/ios.eps'
set style line 11 lc rgb '#000000' lt 1
set border 3 back ls 11
set tics nomirror
set style line 12 lc rgb '#000000' lt 0 lw 1
set grid back ls 12
set style line 1 lc rgb '#8b1a0e' pt 1 ps 1 lt 1 lw 2 # --- red
set style line 2 lc rgb '#5e9c36' pt 6 ps 1 lt 1 lw 2 # --- green
set style line 3 lc rgb '#88419d' pt 2 ps 1 lt 1 lw 2 # --- purple
set style line 4 lc rgb '#225ea8' pt 3 ps 1 lt 1 lw 2 # --- blue
set style line 5 lc rgb '#000000' pt 4 ps 1 lt 1 lw 2 # --- black
set style line 6 lc rgb '#00ced1' pt 5 ps 1 lt 1 lw 2 # --- darkturquoise
set style line 7 lc rgb '#ff00ff' pt 7 ps 1 lt 1 lw 2 # --- magenta
set style line 8 lc rgb '#87ceeb' pt 8 ps 1 lt 1 lw 2 # --- skyblue
set style line 9 lc rgb '#660066' pt 9 ps 1 lt 1 lw 2 # --- dunno
set key top left
set xlabel 'N (input size in Mb)'
set ylabel 'I/Os'
#set xrange [0:0]
#set yrange [0:0]
plot 'insert_experiment_results/2016-06-03.10_06_00/internal_Internal' u 1:3 t 'Internal' w lp ls 5, \
'insert_experiment_results/2016-06-03.10_06_00/rtree_Boost R-tree' u 1:3 t 'Boost R-tree' w lp ls 3, \
'insert_experiment_results/2016-06-03.10_06_00/spatial_libspatial' u 1:3 t 'libspatial' w lp ls 6, \
'insert_experiment_results/2016-06-03.10_06_00/mysql_MySQL' u 1:3 t 'MySQL' w lp ls 4