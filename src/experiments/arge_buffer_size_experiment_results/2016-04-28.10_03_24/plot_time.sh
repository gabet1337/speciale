#!/usr/bin/gnuplot
set terminal postscript eps enhanced color font 'Verdana,24'
set output 'arge_buffer_size_experiment_results/2016-04-28.10_03_24/time.eps'
set style line 11 lc rgb '#000000' lt 1
set border 3 back ls 11
set tics nomirror
set style line 12 lc rgb '#000000' lt 0 lw 1
set grid back ls 12
#set style line 1 lc rgb '#8b1a0e' pt 1 ps 2 lt 1 lw 2 # --- red
#set style line 2 lc rgb '#5e9c36' pt 6 ps 2 lt 1 lw 2 # --- green
#set style line 3 lc rgb '#88419d' pt 2 ps 2 lt 1 lw 2 # --- purple
#set style line 4 lc rgb '#225ea8' pt 3 ps 2 lt 1 lw 2 # --- blue
#set style line 5 lc rgb '#000000' pt 4 ps 2 lt 1 lw 2 # --- black
#set style line 6 lc rgb '#00ced1' pt 5 ps 2 lt 1 lw 2 # --- darkturquoise
#set style line 7 lc rgb '#ff00ff' pt 7 ps 2 lt 1 lw 2 # --- magenta
#set style line 8 lc rgb '#87ceeb' pt 8 ps 2 lt 1 lw 2 # --- skyblue

set style line 1 lc rgb '#8b1a0e' pi 1 pt 1 ps 2 lt 1 lw 2 # --- red
set style line 2 lc rgb '#5e9c36' pi 1 pt 5 ps 2 lt 1 lw 2 # --- green
set style line 3 lc rgb '#88419d' pi 1 pt 6 ps 2 lt 1 lw 2 # --- purple
set style line 4 lc rgb '#225ea8' pi 1 pt 3 ps 2 lt 1 lw 2 # --- blue
set style line 5 lc rgb '#000000' pi 1 pt 4 ps 2 lt 1 lw 2 # --- black
set style line 6 lc rgb '#00ced1' pi 1 pt 11 ps 2 lt 1 lw 2 # --- darkturquoise
set style line 7 lc rgb '#ff00ff' pi 1 pt 7 ps 2 lt 1 lw 2 # --- magenta
set style line 8 lc rgb '#87ceeb' pi 1 pt 8 ps 2 lt 1 lw 2 # --- skyblue
set style line 9 lc rgb '#808080' pi 1 pt 2 ps 2 lt 1 lw 2 # --- darkgray

set key top left
set xlabel 'N (input size in Mb)'
set ylabel 'Time (s) / N (input size in Mb)'
#set xrange [0:0]
#set yrange [0:0]
plot 'arge_buffer_size_experiment_results/2016-04-28.10_03_24/arge_buffer4096' u 1:($2/$1) t '4 Kb' w lp ls 1, \
    'arge_buffer_size_experiment_results/2016-04-28.10_03_24/arge_buffer8192' u 1:($2/$1) t '8 Kb' w lp ls 2, \
    'arge_buffer_size_experiment_results/2016-04-28.10_03_24/arge_buffer16384' u 1:($2/$1) t '16 Kb' w lp ls 3, \
    'arge_buffer_size_experiment_results/2016-04-28.10_03_24/arge_buffer32768' u 1:($2/$1) t '32 Kb' w lp ls 4
