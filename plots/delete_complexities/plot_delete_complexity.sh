#!/usr/bin/gnuplot
set terminal postscript eps enhanced color font 'Verdana,11'
set output 'delete_complexity.eps'
set style line 11 lc rgb '#808080' lt 1
set border 3 back ls 11
set tics nomirror
set style line 12 lc rgb '#808080' lt 0 lw 1
set grid back ls 12
set style line 1 lc rgb '#8b1a0e' pi 5 pt 1 ps 1 lt 1 lw 2 # --- red
set style line 2 lc rgb '#5e9c36' pi 5 pt 6 ps 1 lt 1 lw 2 # --- green
set style line 3 lc rgb '#88419d' pi 5 pt 2 ps 1 lt 1 lw 2 # --- purple
set style line 4 lc rgb '#225ea8' pi 5 pt 3 ps 1 lt 1 lw 2 # --- blue
set style line 5 lc rgb '#000000' pi 5 pt 4 ps 1 lt 1 lw 2 # --- black
set style line 6 lc rgb '#00ced1' pi 6 pt 5 ps 1 lt 1 lw 2 # --- darkturquoise
set style line 7 lc rgb '#ff00ff' pi 5 pt 7 ps 1 lt 1 lw 2 # --- magenta
set style line 8 lc rgb '#87ceeb' pi 5 pt 8 ps 1 lt 1 lw 2 # --- skyblue
set key top left
set xlabel 'N'
set ylabel 'Time / N'
set xrange [1:10]
set yrange [0:*]
buffer_size = 8
epsilon(fanout, B) = log(fanout)/log(B)
brodal(x,B) = (1/(epsilon(2,B)*(B**(1-epsilon(2,B))))) * (log(x)/log(B))
internal(x) = log(x)/log(2)
arge(x,B) = log(x)/log(B)
rtree(x) = 0.8*x
rstar(x) = x
mysql(x) = x
unset ytics
unset xtics

plot brodal(x,buffer_size) with linespoint title sprintf("Brodal") ls 2,\
     internal(x) with linespoint title sprintf("McCreight Internal PST") ls 3,\
     arge(x,buffer_size) with linespoint title sprintf("Arge et al") ls 4,\
     rtree(x) with linespoint title sprintf("RTree") ls 5,\
     rstar(x) with linespoint title sprintf("R*Tree") ls 6,\
     mysql(x) with linespoint title sprintf("MySQL") ls 7


