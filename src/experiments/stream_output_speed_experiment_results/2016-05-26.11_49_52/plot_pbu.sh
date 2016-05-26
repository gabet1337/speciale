#!/usr/bin/gnuplot
set terminal postscript eps enhanced color font 'Verdana,12'
set output 'stream_output_speed_experiment_results/2016-05-26.11_49_52/pbu.eps'
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
set ylabel 'point buffer underflows'
#set xrange [0:0]
#set yrange [0:0]
plot 'stream_output_speed_experiment_results/2016-05-26.11_49_52/mmap_mmap' u 1:9 t 'mmap' w lp ls 8, \
'stream_output_speed_experiment_results/2016-05-26.11_49_52/buffered_stream_buffered' u 1:9 t 'buffered' w lp ls 9, \
'stream_output_speed_experiment_results/2016-05-26.11_49_52/file_stream_file' u 1:9 t 'file' w lp ls 10