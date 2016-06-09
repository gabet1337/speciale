#!/usr/bin/gnuplot
set terminal postscript eps size 5,1.75 enhanced color font 'Verdana,12'
set output 'insert_experiment_results/combined_results/pfs.eps'
set style line 11 lc rgb '#000000' lt 1
set border 3 back ls 11
set tics nomirror
set style line 12 lc rgb '#000000' lt 0 lw 1
set grid back ls 12
set style line 1 lc rgb '#8b1a0e' pi 10 pt 1 ps 1 lt 1 lw 2 # --- red
set style line 2 lc rgb '#5e9c36' pi 10 pt 6 ps 1 lt 1 lw 2 # --- green
set style line 3 lc rgb '#88419d' pi 10 pt 2 ps 1 lt 1 lw 2 # --- purple
set style line 4 lc rgb '#225ea8' pi 10 pt 3 ps 1 lt 1 lw 2 # --- blue
set style line 5 lc rgb '#000000' pi 5 pt 4 ps 1 lt 1 lw 2 # --- black
set style line 6 lc rgb '#00ced1' pi 10 pt 5 ps 1 lt 1 lw 2 # --- darkturquoise
set style line 7 lc rgb '#ff00ff' pi 10 pt 7 ps 1 lt 1 lw 2 # --- magenta
set style line 8 lc rgb '#87ceeb' pi 10 pt 8 ps 1 lt 1 lw 2 # --- skyblue
set style line 9 lc rgb '#660066' pi 10 pt 9 ps 1 lt 1 lw 2 # --- dunno
set key at 260,2**25
set xlabel 'N (input size in Mb)'
set ylabel 'page faults'
set logscale y 2
set format y "2^{%L}"
#set xrange [0:0]
#set yrange [0:0]
plot 'insert_experiment_results/combined_results/internal_Internal' u 1:4 t 'Internal' w lp ls 5, \
     'insert_experiment_results/combined_results/rtree_Boost R-tree' u 1:4 t 'Boost R-tree' w lp ls 3, \
     'insert_experiment_results/combined_results/mysql_MySQL_with_index' u 1:4 t 'MySQL (with index)' w lp ls 4
