#!/usr/bin/gnuplot
set terminal postscript eps enhanced color size 2.5,2.0 font 'Verdana,12'
set output 'query_experiment_report_50mb_results/Final_100mb/time.eps'
set style line 11 lc rgb '#000000' lt 1
set border 3 back ls 11
set tics nomirror
set style line 12 lc rgb '#000000' lt 0 lw 1
set grid back ls 12
set style line 1 lc rgb '#8b1a0e' pi 5 pt 1 ps 1 lt 1 lw 2 # --- red
set style line 2 lc rgb '#5e9c36' pi 5 pt 6 ps 1 lt 1 lw 2 # --- green
set style line 3 lc rgb '#88419d' pi 5 pt 2 ps 1 lt 1 lw 2 # --- purple
set style line 4 lc rgb '#225ea8' pi 5 pt 3 ps 1 lt 1 lw 2 # --- blue
set style line 5 lc rgb '#000000' pt 4 ps 1 lt 1 lw 2 # --- black
set style line 6 lc rgb '#00ced1' pi 5 pt 5 ps 1 lt 1 lw 2 # --- darkturquoise
set style line 7 lc rgb '#ff00ff' pi 5 pt 7 ps 1 lt 1 lw 2 # --- magenta
set style line 8 lc rgb '#87ceeb' pi 5 pt 8 ps 1 lt 1 lw 2 # --- skyblue
set key at 45,1
set xlabel 'N (reported data in Mb)'
set ylabel 'Time (s)'
set logscale y
#set xrange [0:0]
#set yrange [0.1:*]
set format y "10^{%T}"
plot 'query_experiment_report_50mb_results/Final_100mb/gerth_Gerth' u 1:2 t 'Brodal with fanout 2' w lp ls 2, \
'query_experiment_report_50mb_results/Final_100mb/rtree_Boost R-tree' u 1:($4/1000.0) t 'Boost R-tree' w lp ls 3, \
'query_experiment_report_50mb_results/Final_100mb/mysql_MySQL' u 1:2 t 'MySQL (no index)' w lp ls 4, \
'query_experiment_report_50mb_results/Final_100mb/internal_Internal' u 1:2 t 'Internal PST' w lp ls 5, \
'query_experiment_report_50mb_results/Final_100mb/spatial_libspatial' u 1:2 t 'libspatial R*-Tree' w lp ls 6, \
'query_experiment_report_50mb_results/Final_100mb/arge_Arge' u 1:2 t 'Arge' w lp ls 1
