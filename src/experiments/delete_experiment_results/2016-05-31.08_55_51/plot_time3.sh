#!/usr/bin/gnuplot
set terminal postscript eps enhanced color size 5,3.0 font 'Verdana,12'
set output 'delete_experiment_results/2016-05-31.08_55_51/time3.eps'
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
set key top left
set xlabel 'N (non-deleted data in Mb)'
set ylabel 'Time (s) per deleted Mb'
set y2label '{ }'
set xtics ('400' 1,'350' 50,'300' 100, '250' 150, '200' 200, '150' 250, '100' 300, '50' 350, '0' 400)
set y2tics
#set format x "50-%g"
#set xrange [0:52]
set yrange [0:500]

delta_1(x) = (r = next_1 - x, next_1 = x, r)
next_1 = NaN
delta_2(x) = (r = next_2 - x, next_2 = x, r)
next_2 = NaN
delta_3(x) = (r = next_3 - x, next_3 = x, r)
next_3 = NaN
delta_4(x) = (r = next_4 - x, next_4 = x, r)
next_4 = NaN
delta_5(x) = (r = next_5 - x, next_5 = x, r)
next_5 = NaN
delta_6(x) = (r = next_6 - x, next_6 = x, r)
next_6 = NaN

plot 'delete_experiment_results/2016-05-31.08_55_51/gerth_Gerth' u (400-$1):(delta_1($2)) t 'Brodal' w lp ls 2
