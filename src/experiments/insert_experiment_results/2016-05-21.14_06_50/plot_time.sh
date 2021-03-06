#!/usr/bin/gnuplot
#set terminal postscript eps enhanced color font 'Verdana,11'
set terminal postscript eps size 7,2.62 enhanced color font 'Verdana,22'
set output 'insert_experiment_results/2016-05-21.14_06_50/time.eps'
set style line 11 lc rgb '#808080' lt 1
set border 3 back ls 11
set tics nomirror
set style line 12 lc rgb '#808080' lt 0 lw 1
set grid back ls 12
set style line 1 lc rgb '#8b1a0e' pt 1 ps 2 lt 1 lw 5 # --- red
set style line 2 lc rgb '#5e9c36' pt 6 ps 2 lt 1 lw 5 # --- green
set style line 3 lc rgb '#88419d' pt 2 ps 2 lt 1 lw 5 # --- purple
set style line 4 lc rgb '#225ea8' pt 3 ps 2 lt 1 lw 5 # --- blue
set style line 5 lc rgb '#000000' pt 4 ps 2 lt 1 lw 5 # --- black
set style line 6 lc rgb '#00ced1' pt 5 ps 2 lt 1 lw 5 # --- darkturquoise
set style line 7 lc rgb '#ff00ff' pt 7 ps 2 lt 1 lw 5 # --- magenta
set style line 8 lc rgb '#87ceeb' pt 8 ps 2 lt 1 lw 5 # --- skyblue
set key top left
set xlabel 'N (input size in Mb)'
set ylabel 'Time (s)'
#set xrange [0:0]
#set yrange [0:0]
plot 'insert_experiment_results/2016-05-21.14_06_50/arge_Arge' u 1:2 t 'Arge' w lp ls 1,\
     'insert_experiment_results/2016-05-21.14_06_50/gerth_Gerth' u 1:2 t 'Brodal' w lp ls 2
