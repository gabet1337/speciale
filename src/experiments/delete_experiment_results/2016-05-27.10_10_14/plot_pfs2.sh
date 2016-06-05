#!/usr/bin/gnuplot
set terminal postscript eps enhanced color size 7,2.62 font 'Verdana,16'
set output 'delete_experiment_results/2016-05-27.10_10_14/pfs2.eps'
set style line 11 lc rgb '#000000' lt 1
set border 3 back ls 11
set tics nomirror
set style line 12 lc rgb '#000000' lt 0 lw 1
set grid back ls 12
set style line 1 lc rgb '#8b1a0e' pt 1 ps 1.5 lt 1 lw 2 # --- red
set style line 2 lc rgb '#5e9c36' pt 6 ps 1.5 lt 1 lw 2 # --- green
set style line 3 lc rgb '#88419d' pt 2 ps 1.5 lt 1 lw 2 # --- purple
set style line 4 lc rgb '#225ea8' pt 3 ps 1.5 lt 1 lw 2 # --- blue
set style line 5 lc rgb '#000000' pt 4 ps 1.5 lt 1 lw 2 # --- black
set style line 6 lc rgb '#00ced1' pt 5 ps 1.5 lt 1 lw 2 # --- darkturquoise
set style line 7 lc rgb '#ff00ff' pt 7 ps 1.5 lt 1 lw 2 # --- magenta
set style line 8 lc rgb '#87ceeb' pt 8 ps 1.5 lt 1 lw 2 # --- skyblue
set key top right
set xlabel 'N (non-deleted data in Mb)
set xtics ('50' 1,'45' 6,'40' 11,'35' 16,'30' 21,'25' 26,'20' 31,'15' 36, '10' 41, '5' 46, '0' 50) 
set ylabel 'page faults per deleted Mb'
#set xrange [0:0]
set yrange [0:450000]

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

plot 'delete_experiment_results/2016-05-27.10_10_14/gerth_Gerth' u (50-$1):(delta_1($4)*0) t 'Brodal' w lp ls 2 , \
     'delete_experiment_results/2016-05-27.10_10_14/internal_Internal' u (50-$1):(delta_2($4)) t 'Internal PST' w lp ls 5, \
     'delete_experiment_results/2016-05-27.10_10_14/rtree_Boost R-tree' u (50-$1):(delta_3($4)) t 'Boost R-tree' w lp ls 3, \
     'delete_experiment_results/2016-05-27.10_10_14/spatial_libspatial' u (60-$1):(delta_4($4)) t 'Libspatial R*-Tree' w lp ls 6, \
     'delete_experiment_results/2016-05-27.10_10_14/mysql_MySQL' u (50-$1):(delta_5($4)) t 'MySQL (no index)' w lp ls 4, \
    'delete_experiment_results/2016-05-27.10_10_14/mysql_MySQL_index' u (50-$1):($4) t 'MySQL (with index)' w lp ls 7


#plot 'delete_experiment_results/2016-05-27.10_10_14/gerth_Gerth' u 1:4 t 'Gerth' w lp ls 2, \
#'delete_experiment_results/2016-05-27.10_10_14/internal_Internal' u 1:4 t 'Internal' w lp ls 5, \
#'delete_experiment_results/2016-05-27.10_10_14/rtree_Boost R-tree' u 1:4 t 'Boost R-tree' w lp ls 3, \
#'delete_experiment_results/2016-05-27.10_10_14/spatial_libspatial' u 1:4 t 'libspatial' w lp ls 6, \
#'delete_experiment_results/2016-05-27.10_10_14/mysql_MySQL' u 1:4 t 'MySQL' w lp ls 4