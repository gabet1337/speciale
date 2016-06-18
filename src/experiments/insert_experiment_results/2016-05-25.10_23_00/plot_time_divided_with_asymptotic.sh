#!/usr/bin/gnuplot
set terminal postscript eps enhanced color font 'Verdana,24'

set style line 11 lc rgb '#000000' lt 1
set border 3+8 back ls 11
set tics nomirror
set style line 12 lc rgb '#000000' lt 0 lw 1
set grid back ls 12
set style line 1 lc rgb '#8b1a0e' pt 1 ps 1 lt 1 lw 2 # --- red
set style line 2 lc rgb '#5e9c36' pi 8 pt 6 ps 1 lt 1 lw 2 # --- green
set style line 3 lc rgb '#88419d' pi 5 pt 2 ps 1 lt 1 lw 5 # --- purple
set style line 4 lc rgb '#225ea8' pi 10 pt 3 ps 1 lt 1 lw 2 # --- blue
set style line 5 lc rgb '#000000' pi 5 pt 4 ps 2 lt 1 lw 5 # --- black
set style line 6 lc rgb '#00ced1' pt 5 ps 1 lt 1 lw 2 # --- darkturquoise
set style line 7 lc rgb '#ff00ff' pi 5 pt 7 ps 2 lt 1 lw 2 # --- magenta
set style line 8 lc rgb '#87ceeb' pt 8 ps 1 lt 1 lw 2 # --- skyblue
set key top left
set xtics rotate by -35 offset -0.3,0
set xlabel 'N (input size in Mb)'
set ylabel 'Time (s) per insert / log_B N'
set y2label 'I/Os per insert / log_B N'
set y2tics
#set xrange [0:3500]
set y2range [0:2]
set yrange [0:2000]
#unset ytics
set autoscale y
B = 1024*1024
epsilon(fanout) = log(fanout)/log(B)
f(x,eps) = (1/(eps*B**(1-eps))) * (log(x)/log(B))

set output 'insert_experiment_results/2016-05-25.10_23_00/time_and_ios.eps'
plot 'insert_experiment_results/2016-05-25.10_23_00/arge_Arge' u 1:($2/(($1*1024*1024)*(log($1*1024*1024)/log(4*1024)))) title "time" w lp ls 5 axes x1y1,\
     'insert_experiment_results/2016-05-25.10_23_00/arge_Arge' u 1:($3/(($1*1024*1024)*(log($1*1024*1024)/log(4*1024)))) title "I/Os" w lp ls 7 axes x1y2

