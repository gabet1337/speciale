#!/usr/bin/gnuplot
set terminal postscript eps enhanced color font 'Verdana,24'
set output 'insert_experiment_results/combined_results/time_with_and_without_mysql.eps'
set style line 11 lc rgb '#000000' lt 1
set border 3+8 back ls 11
set tics nomirror
set style line 12 lc rgb '#000000' lt 0 lw 1
#set grid back ls 12
set style line 1 lc rgb '#8b1a0e' pt 1 ps 1 lt 1 lw 2 # --- red
set style line 2 lc rgb '#5e9c36' pt 6 ps 1 lt 1 lw 2 # --- green
set style line 3 lc rgb '#88419d' pi 10 pt 2 ps 2 lt 1 lw 5 # --- purple
set style line 4 lc rgb '#225ea8' pt 3 ps 1 lt 1 lw 2 # --- blue
set style line 5 lc rgb '#000000' pi 70 pt 4 ps 2 lt 1 lw 5 # --- black
set style line 6 lc rgb '#00ced1' pt 5 ps 1 lt 1 lw 2 # --- darkturquoise
set style line 7 lc rgb '#ff00ff' pt 7 ps 1 lt 1 lw 2 # --- magenta
set style line 8 lc rgb '#87ceeb' pt 8 ps 1 lt 1 lw 2 # --- skyblue
set style line 9 lc rgb '#660066' pt 9 ps 1 lt 1 lw 2 # --- dunno
set key top center
set xlabel 'N (input size in Mb)'
set x2label ''
set ylabel 'Time (s) per insert / 1'
set y2label 'Time (s) per insert / log_B N'
#set yrange [0:1]
#set y2range [0:1]
#set ytics rotate by 45
unset ytics
set xrange [50:10000]
set x2range [0:300]
set xtics rotate by -45 offset -0.8, 0.2
set x2tics rotate by -45 offset -2, 2
set format y "%.0s{\267}10^{%S}"
set format y2 "%.0s{\267}10^{%S}"
#set autoscale y
#set autoscale y2
plot 'insert_experiment_results/combined_results/mysql_MySQL_with_index' u 1:(($2/($1*1024*1024))/(log($1*1024*1024)/log(4*1024*1024))) t 'time with index' w lp ls 3 axes x2y2 ,\
     'insert_experiment_results/combined_results/mysql_MySQL_no_index' u 1:($2/($1*1024*1024)) t 'time without index' w lp ls 5 axes x1y1
