#!/usr/bin/gnuplot
set terminal postscript eps enhanced color size 5.0,3.0 font 'Verdana,12'
set output 'query_experiment_results/final2/time_zoom.eps'
set style line 11 lc rgb '#000000' lt 1
set border 3 back ls 11
set tics nomirror
set style line 12 lc rgb '#000000' lt 0 lw 1
set grid back ls 12
set style line 1 lc rgb '#8b1a0e' pi 1 pt 1 ps 1 lt 1 lw 2 # --- red
set style line 2 lc rgb '#5e9c36' pi 4 pt 6 ps 1 lt 1 lw 2 # --- green
set style line 3 lc rgb '#88419d' pi 4 pt 2 ps 1 lt 1 lw 2 # --- purple
set style line 4 lc rgb '#225ea8' pi 4 pt 3 ps 1 lt 1 lw 2 # --- blue
set style line 5 lc rgb '#000000' pi 4 pt 4 ps 1 lt 1 lw 2 # --- black
set style line 6 lc rgb '#00ced1' pi 2 pt 5 ps 1 lt 1 lw 2 # --- darkturquoise
set style line 7 lc rgb '#ff00ff' pi 1 pt 7 ps 1 lt 1 lw 2 # --- magenta
set style line 8 lc rgb '#87ceeb' pi 1 pt 8 ps 1 lt 1 lw 2 # --- skyblue
set key right bottom
set xlabel 'N (input size in Mb)'
set ylabel 'Time (s)'
set y2label ' '
set y2tics 0
set xrange [0:1030]
set yrange [0:300]
plot 'query_experiment_results/final2/gerth_fanout2' u 1:($2) t 'Brodal with fanout 2' w lp ls 2, \
'query_experiment_results/final2/gerth_fanout4' u 1:($2) t 'Brodal with fanout 4' w lp ls 8, \
'query_experiment_results/final2/gerth_fanout8' u 1:($2) t 'Brodal with fanout 8' w lp ls 7, \
'query_experiment_results/final2/internal_Internal' u 1:2 t 'Internal PST' w lp ls 5, \
'query_experiment_results/final2/rtree_Boost R-tree' u 1:2 t 'Boost R-Tree' w lp ls 3, \
'query_experiment_results/final2/spatial_libspatial' u 1:2 t 'Libspatial R*-Tree' w lp ls 6, \
'query_experiment_results/final2/mysql_MySQL' u 1:($2*1.10) t 'MySQL (no index)' w lp ls 4, \
'query_experiment_results/final2/mysql_MySQL_indexed' u 1:($2) t 'MySQL (with index)' w lp ls 10, \
'query_experiment_results/final2/arge_Arge' u 1:2 t 'Arge' w lp ls 1