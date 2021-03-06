#!/usr/bin/gnuplot
set terminal postscript eps enhanced color size 3,1.5 font 'Verdana,12'
set output 'gerth_buffer_size_experiment_results/2016-05-03.13_51_54/time_slide.eps'
set style line 11 lc rgb '#000000' lt 1
set border 3 back ls 11
set tics nomirror
set style line 12 lc rgb '#000000' lt 0 lw 1
set grid back ls 12
set style line 1 lc rgb '#8b1a0e' pi 15 pt 1 ps 1 lt 1 lw 2 # --- red
set style line 2 lc rgb '#5e9c36' pi 15 pt 6 ps 1 lt 1 lw 2 # --- green
set style line 3 lc rgb '#88419d' pi 15 pt 2 ps 1 lt 1 lw 2 # --- purple
set style line 4 lc rgb '#225ea8' pi 15 pt 3 ps 1 lt 1 lw 2 # --- blue
set style line 5 lc rgb '#000000' pi 15 pt 4 ps 1 lt 1 lw 2 # --- black
set style line 6 lc rgb '#00ced1' pi 15 pt 5 ps 1 lt 1 lw 2 # --- darkturquoise
set style line 7 lc rgb '#ff00ff' pi 15 pt 7 ps 1 lt 1 lw 2 # --- magenta
set style line 8 lc rgb '#87ceeb' pi 15 pt 8 ps 1 lt 1 lw 2 # --- skyblue
#set key top left
set nokey
set xlabel 'N (input size in Mb)'
set ylabel 'Time (s) / N'
set xrange [0:5025]
#set yrange [0:1.7e-05]
plot 'gerth_buffer_size_experiment_results/2016-05-03.13_51_54/gerth_buffer1MB' u 1:($2/($1*1024*1024)) t '1Mb' w lp ls 2, \
'gerth_buffer_size_experiment_results/2016-05-03.13_51_54/gerth_buffer2MB' u 1:($2/($1*1024*1024)) t '2Mb' w lp ls 3, \
'gerth_buffer_size_experiment_results/2016-05-03.13_51_54/gerth_buffer4MB' u 1:($2/($1*1024*1024)) t '4Mb' w lp ls 4, \
'gerth_buffer_size_experiment_results/2016-05-03.13_51_54/gerth_8Mb' u 1:($2/($1*1024*1024)) t '8Mb' w lp ls 5, \
'gerth_buffer_size_experiment_results/2016-05-03.13_51_54/gerth_16Mb' u 1:($2/($1*1024*1024)) t '16Mb' w lp ls 6,\
'gerth_buffer_size_experiment_results/2016-05-03.13_51_54/gerth_32Mb' u 1:($2/($1*1024*1024)) t '32Mb' w lp ls 7
