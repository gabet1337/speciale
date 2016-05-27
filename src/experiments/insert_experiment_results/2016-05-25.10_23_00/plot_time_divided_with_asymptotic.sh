#!/usr/bin/gnuplot
set terminal postscript eps enhanced color font 'Verdana,11'

set style line 11 lc rgb '#808080' lt 1
set border 3 back ls 11
set tics nomirror
set style line 12 lc rgb '#808080' lt 0 lw 1
#set grid back ls 12
set style line 1 lc rgb '#8b1a0e' pt 1 ps 1 lt 1 lw 2 # --- red
set style line 2 lc rgb '#5e9c36' pi 8 pt 6 ps 1 lt 1 lw 2 # --- green
set style line 3 lc rgb '#88419d' pi 6 pt 2 ps 1 lt 1 lw 2 # --- purple
set style line 4 lc rgb '#225ea8' pi 8 pt 3 ps 1 lt 1 lw 2 # --- blue
set style line 5 lc rgb '#000000' pt 4 ps 1 lt 1 lw 2 # --- black
set style line 6 lc rgb '#00ced1' pt 5 ps 1 lt 1 lw 2 # --- darkturquoise
set style line 7 lc rgb '#ff00ff' pt 7 ps 1 lt 1 lw 2 # --- magenta
set style line 8 lc rgb '#87ceeb' pt 8 ps 1 lt 1 lw 2 # --- skyblue
set key top right
set xlabel 'N (input size in Mb)'
#set ylabel 'Time (s) / N (input size in Mb)'
#set xrange [0:3500]
#set yrange [0:4000]
#unset ytics
B = 1024*1024
epsilon(fanout) = log(fanout)/log(B)
f(x,eps) = (1/(eps*B**(1-eps))) * (log(x)/log(B))

set output 'ios_divided_with_asymptotic_arge.eps'
plot 'arge_Arge' u 1:($3/(($1*1024*1024)*(log($1*1024*1024)/log(4*1024)))) notitle w lp ls 5
set output 'time_divided_with_asymptotic_arge.eps'
plot 'arge_Arge' u 1:($2/(($1*1024*1024)*(log($1*1024*1024)/log(4*1024)))) notitle w lp ls 5

