#!/usr/bin/gnuplot
set terminal postscript eps enhanced color size 5,3.5 font 'Verdana,12'
set output 'time.eps'
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
set xlabel 'N (input size in Mb)'
set ylabel 'Time (s)'
set key left top
set ytics 500
plot "ms.dat" using ($1*4/(1024*1024)):($2/1000) ls 5 with linespoints title 'External Merge-Sort' , \
     "qs.dat" using ($1*4/(1024*1024)):($2/1000) ls 3 with linespoints title 'Internal Quick Sort' , \
     "hs.dat" using ($1*4/(1024*1024)):($2/1000) ls 4 with linespoints title 'Internal Heap Sort' , \
     "ehs-naive.dat" using ($1*4/(1024*1024)):($2/1000) ls 2 with linespoints title 'External Heap Sort (naive)' , \
     "ehs-lin-sel.dat" using ($1*4/(1024*1024)):($2/1000) ls 6 with linespoints title 'External Heap Sort (Linear selection)'

