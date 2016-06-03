#!/usr/bin/gnuplot
set terminal postscript eps enhanced color font 'Verdana,12'
set output 'gerth_buffer_size_experiment_results/2016-05-03.13_51_54/io_per_insert.eps'
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
set style line 9 lc rgb '#660066' pi 4 pt 9 ps 1 lt 1 lw 2 # --- 
set key top left
set xlabel 'N (input size in Mb)'
set ylabel 'I/Os / N (input size in Mb)'
#set xrange [800:6000]
#set yrange [0:1000]
#B = 1024*1024
epsilon(fanout,B) = log(fanout)/log(B)
f(x,eps,B) = (1/(eps*B**(1-eps))) * (log(x)/log(B))


plot 'gerth_buffer_size_experiment_results/2016-05-03.13_51_54/gerth_buffer1MB' u 1:($3/($1*1024*1024)) t '1Mb' w lp ls 2, \
     'gerth_buffer_size_experiment_results/2016-05-03.13_51_54/gerth_buffer2MB' u 1:($3/($1*1024*1024)) t '2Mb' w lp ls 3 , \
     'gerth_buffer_size_experiment_results/2016-05-03.13_51_54/gerth_buffer4MB' u 1:(($3-5731724)/($1*1024*1024)) t '4Mb' w lp ls 4 , \
     'gerth_buffer_size_experiment_results/2016-05-03.13_51_54/gerth_8Mb' u 1:(($3)/($1*1024*1024*26)) t '8Mb' w lp ls 5, \
     'gerth_buffer_size_experiment_results/2016-05-03.13_51_54/gerth_16Mb' u 1:(($3)/($1*1024*1024*28)) t '16Mb' w lp ls 6,\
     'gerth_buffer_size_experiment_results/2016-05-03.13_51_54/gerth_32Mb' u 1:($3/($1*1024*1024*30)) t '32Mb' w lp ls 7
#     'gerth_buffer_size_experiment_results/2016-05-03.13_51_54/gerth_buffer8MB' u 1:(($3-2733929)/($1*1024*1024)) t '8Mb' w lp ls 5, \
#     'gerth_buffer_size_experiment_results/2016-05-03.13_51_54/gerth_buffer16MB' u 1:(($3-7156100)/($1*1024*1024)) t '16Mb' w lp ls 6,\

