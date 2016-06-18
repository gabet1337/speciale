#!/usr/bin/gnuplot
set terminal postscript eps enhanced color font 'Verdana,24'
#set ytics font "Verdana,12"
set style line 11 lc rgb '#000000' lt 1
set border 3+8 back ls 11
set tics nomirror
set style line 12 lc rgb '#000000' lt 0 lw 1
set grid back ls 12
set style line 1 lc rgb '#8b1a0e' pt 20 ps 1 lt 1 lw 5 # --- red
set style line 2 lc rgb '#5e9c36' pi 20 pt 6 ps 1 lt 1 lw 5 # --- green
set style line 3 lc rgb '#88419d' pi 20 pt 2 ps 1 lt 1 lw 5 # --- purple
set style line 4 lc rgb '#225ea8' pi 20 pt 3 ps 1 lt 1 lw 5 # --- blue
set style line 5 lc rgb '#000000' pi 20 pt 4 ps 2 lt 1 lw 5 # --- black
set style line 6 lc rgb '#00ced1' pi 20 pt 5 ps 1 lt 1 lw 5 # --- darkturquoise
set style line 7 lc rgb '#ff00ff' pi 20 pt 7 ps 2 lt 1 lw 5 # --- magenta
set style line 8 lc rgb '#87ceeb' pi 20 pt 8 ps 1 lt 1 lw 5 # --- skyblue
set key top left
set xtics rotate by -35 offset -0.2 -1.5
set xlabel 'N (input size in Mb)'
set ylabel 'Time (s) per insert / (1/({/Symbol \145} B^{1-{/Symbol \145}}) log_{B} N)' font "Verdana,18"
set y2label 'I/Os per insert / (1/({/Symbol \145} B^{1-{/Symbol \145}}) log_{B} N)' font "Verdana,18"
set y2tics
#set xrange [0:3500]
set yrange [0:1]
set y2range [0:2000]
#unset ytics
set autoscale y2
B = 1024*1024
epsilon(fanout) = log(fanout)/log(B)
f(x,eps) = (1/(eps*B**(1-eps))) * (log(x)/log(B))
set output 'insert_experiment_results/2016-05-22.16_20_22/time_and_ios.eps'
plot 'insert_experiment_results/2016-05-22.16_20_22/gerth_Gerth' u 1:($2/(($1*1024*1024)*f(($1*1024*1024),epsilon(2)))) title 'time' w lp ls 5 axes x1y1,\
     'insert_experiment_results/2016-05-22.16_20_22/gerth_Gerth' u 1:(($3)/(($1*1024*1024)*f(($1*1024*1024),epsilon(2)))) title 'I/Os' w lp ls 7 axes x1y2
