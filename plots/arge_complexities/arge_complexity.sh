#!/usr/bin/gnuplot
set terminal postscript eps enhanced color font 'Verdana,24'
set output 'arge_complexity.eps'
set style line 11 lc rgb '#000000' lt 1
set border 3 back ls 11
set tics nomirror
set style line 12 lc rgb '#000000' lt 0 lw 1
set grid back ls 12
set style line 1 lc rgb '#8b1a0e' pi 4.10 pt 1 ps 2 lt 1 lw 2 # --- red
set style line 2 lc rgb '#5e9c36' pi 5 pt 5 ps 2 lt 1 lw 2 # --- green
set style line 3 lc rgb '#88419d' pi 5 pt 6 ps 2 lt 1 lw 2 # --- purple
set style line 4 lc rgb '#225ea8' pi 5 pt 3 ps 2 lt 1 lw 2 # --- blue
set style line 5 lc rgb '#000000' pi 5 pt 4 ps 2 lt 1 lw 2 # --- black
set style line 6 lc rgb '#00ced1' pi 5 pt 11 ps 2 lt 1 lw 2 # --- darkturquoise
set style line 7 lc rgb '#ff00ff' pi 5 pt 7 ps 2 lt 1 lw 2 # --- magenta
set style line 8 lc rgb '#87ceeb' pi 5 pt 8 ps 2 lt 1 lw 2 # --- skyblue
set style line 9 lc rgb '#808080' pi 5 pt 2 ps 2 lt 1 lw 2 # --- darkgray
set key top left
set xlabel 'N'
set ylabel 'log_B N'
set xrange [1:*]
#set yrange [0:4.8e-05]
unset ytics
unset xtics
#B = 1024*1024
#epsilon = 0.5
#epsilon(fanout) = log(fanout)/log(B)
#f(x,eps) = (1/(eps*(B**(1-eps)))) * (log(x)/log(B))

f(x,B) = (log(x)/log(1024*B))

plot f(x,4) with linespoint title sprintf("4 Kb") ls 1,\
     f(x,8) with linespoint title sprintf("8 Kb") ls 2,\
     f(x,16) with linespoint title sprintf("16 Kb") ls 3,\
     f(x,32) with linespoint title sprintf("32 Kb") ls 4