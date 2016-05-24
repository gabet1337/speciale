#!/usr/bin/gnuplot
set terminal postscript eps enhanced color font 'Verdana,30'
set output 'stream_output_speed_experiment_results/2016-04-23.16_07_40/time.eps'
set style line 11 lc rgb '#808080' lt 1
set border 3 back ls 11
set tics nomirror
set style line 12 lc rgb '#808080' lt 0 lw 1
set grid back ls 12
set style line 1 lc rgb '#8b1a0e' pi 16 pt 1 ps 2 lt 2 lw 10 # --- red
set style line 2 lc rgb '#5e9c36' pi 16 pt 6 ps 2 lt 2 lw 5 # --- green
set style line 3 lc rgb '#88419d' pi 16 pt 2 ps 2 lt 2 lw 5 # --- purple
set style line 4 lc rgb '#225ea8' pi 16 pt 3 ps 2 lt 2 lw 5 # --- blue
set style line 5 lc rgb '#000000' pi 16 pt 4 ps 2 lt 2 lw 10 # --- black
set style line 6 lc rgb '#00ced1' pi 16 pt 5 ps 2 lt 2 lw 5 # --- darkturquoise
set style line 7 lc rgb '#ff00ff' pi 16 pt 7 ps 2 lt 2 lw 5 # --- magenta
set style line 8 lc rgb '#87ceeb' pi 16 pt 8 ps 2 lt 2 lw 5 # --- skyblue
set key top left
set xlabel 'N (input size in Mb)'
set ylabel 'Time (s)'
#set xrange [0:0]
#set yrange [0:0]
plot 'stream_output_speed_experiment_results/2016-04-23.16_07_40/buffered_stream_buffered' u 1:2 t 'buffered' w lp ls 5, \
'stream_output_speed_experiment_results/2016-04-23.16_07_40/mmap_mmap' u 1:2 t 'mmap' w lp ls 1, \
'stream_output_speed_experiment_results/2016-04-23.16_07_40/file_stream_file' u 1:2 t 'file' w lp ls 4
