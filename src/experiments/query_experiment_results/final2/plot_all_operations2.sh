#!/usr/bin/gnuplot
set terminal postscript eps enhanced color size 5.0,2.0 font 'Verdana,12'
set output 'query_experiment_results/final2/all_operations2.eps'
set style line 11 lc rgb '#000000' lt 1
set border 3+8 back ls 11
set tics nomirror
set style line 12 lc rgb '#000000' lt 0 lw 1
set grid back ls 12
set style line 1 lc rgb '#8b1a0e'  pt 1 ps 1 lt 1 lw 2 # --- red
set style line 2 lc rgb '#5e9c36' pt 6 ps 1 lt 1 lw 2 # --- green
set style line 3 lc rgb '#88419d' pt 2 ps 1 lt 1 lw 2 # --- purple
set style line 4 lc rgb '#225ea8' pt 3 ps 1 lt 1 lw 2 # --- blue
set style line 5 lc rgb '#000000' pt 4 ps 1 lt 1 lw 2 # --- black
set style line 6 lc rgb '#00ced1' pt 5 ps 1 lt 1 lw 2 # --- darkturquoise
set style line 7 lc rgb '#ff00ff' pt 7 ps 1 lt 1 lw 2 # --- magenta
set style line 8 lc rgb '#87ceeb' pt 8 ps 1 lt 1 lw 2 # --- skyblue
set key top left
set xlabel 'N (input size in Mb)'
set ylabel 'Running time (s)'
set y2label "Number of NDO's"
set xrange [0:1000]
set y2range [0:3.5]
set yrange[0:350]
set y2tics 1
plot 'query_experiment_results/final2/gerth_fanout4' u 1:2 t 'running time brodal fanout 4' w lp ls 8 axis x1y1,\
     'query_experiment_results/final2/gerth_fanout4' u 1:10 t 'node degree overflow' w lp ls 10 axis x1y2,\
     