#!/usr/bin/gnuplot
set terminal postscript eps enhanced color font 'Verdana,9'
set output 'scaling_discrepancy_hdd_cpu'
set style line 11 lc rgb '#808080' lt 1
set border 3 back ls 11
set tics nomirror
set style line 12 lc rgb '#808080' lt 0 lw 1
set grid back ls 12
set style line 1 lc rgb '#8b1a0e' pt 1 ps 1 lt 1 lw 2 # --- red
set style line 2 lc rgb '#5e9c36' pt 6 ps 1 lt 1 lw 2 # --- green
set style line 3 lc rgb '#88419d' pt 2 ps 1 lt 1 lw 2 # --- purple
set style line 4 lc rgb '#225ea8' pt 3 ps 1 lt 1 lw 2 # --- blue
set style line 5 lc rgb '#000000' pt 4 ps 1 lt 1 lw 2 # --- black
set style line 6 lc rgb '#00ced1' pt 5 ps 1 lt 1 lw 2 # --- darkturquoise
set style line 7 lc rgb '#ff00ff' pt 7 ps 1 lt 1 lw 2 # --- magenta
set style line 8 lc rgb '#87ceeb' pt 8 ps 1 lt 1 lw 2 # --- skyblue
set key top left
set xlabel 'Time'
set ylabel 'CPU MIPS / HDD read 10^6 Integers'
#set xrange [0:0]
#set yrange [0:0]
#f(x) = a* exp(-x*b) + c
#f(x) = a*x + b
f(x) = x**a
fit f(x) 'discrepancy_data' using 1:2 via a
plot 'discrepancy_data' u 1:2 t '' w lp ls 2, f(x)

