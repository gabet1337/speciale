#!/usr/bin/gnuplot
set terminal postscript eps enhanced color font 'Verdana,11'
set output 'gerth_epsilon.eps'
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
set xlabel 'N'
set ylabel 'Time / N'
set xrange [1:10]
set yrange [0:*]
B = 4
#epsilon = 0.5
epsilon(fanout) = log(fanout)/log(B)
f(x,eps) = (1/(eps*(B**(1-eps)))) * (log(x)/log(B))

plot f(x,epsilon(2.0)) with lines title sprintf("fanout 2") ls 2,\
     f(x,epsilon(3.0)) with lines title sprintf("fanout 3") ls 3,\
     f(x,epsilon(4.0)) with lines title sprintf("fanout 4") ls 4,\
     f(x,epsilon(8.0)) with lines title sprintf("fanout 8") ls 5,\
     f(x,epsilon(16.0)) with lines title sprintf("fanout 16") ls 6,\
     f(x,epsilon(32.0)) with lines title sprintf("fanout 32") ls 7,\
     f(x,epsilon(128.0)) with lines title sprintf("fanout 128") ls 8

