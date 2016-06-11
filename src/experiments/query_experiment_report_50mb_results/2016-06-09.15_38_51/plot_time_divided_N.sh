#!/usr/bin/gnuplot
set terminal postscript eps enhanced color size 5,2font 'Verdana,12'
set output 'query_experiment_report_50mb_results/2016-06-09.15_38_51/time_divided_N.eps'
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
set key top right
set xlabel 'K (reported data size in Mb)'
set ylabel 'Time (s) / K (reported data size in Mb)'
set xrange [100:1200]
set xtics 100
#set yrange [0:0]
f(x) = c/x + 1
fit f(x) 'query_experiment_report_50mb_results/2016-06-09.15_38_51/gerth_Gerth' u 1:($2/$1) via c
plot 'query_experiment_report_50mb_results/2016-06-09.15_38_51/gerth_Gerth' u 1:($2/($1)) t 'Brodal with fanout 4' w lp ls 2,\
     f(x) w lp ls 9 pi 5 title sprintf("f(K) = %.3f / K + 1",c)
