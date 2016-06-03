#!/usr/bin/gnuplot
set terminal postscript eps enhanced color font 'Verdana,12'
set output 'gerth_buffer_size_experiment_results/2016-05-03.13_51_54/pfs.eps'
set style line 11 lc rgb '#000000' lt 1
set border 3 back ls 11
set tics nomirror
set style line 12 lc rgb '#000000' lt 0 lw 1
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
set ylabel 'page faults'
#set xrange [0:0]
#set yrange [0:0]
f(x)=0
plot f(x) t 'buffer1Mb' w lp ls 2, \
     f(x) t 'buffer2Mb' w lp ls 3, \
     f(x) t 'buffer4Mb' w lp ls 4, \
     'gerth_buffer_size_experiment_results/2016-05-03.13_51_54/gerth_8Mb' u 1:4 t 'buffer8Mb' w lp ls 5, \
     'gerth_buffer_size_experiment_results/2016-05-03.13_51_54/gerth_16Mb' u 1:4 t 'buffer16Mb' w lp ls 6,\
     'gerth_buffer_size_experiment_results/2016-05-03.13_51_54/gerth_32Mb' u 1:4 t 'buffer32Mb' w lp ls 7
