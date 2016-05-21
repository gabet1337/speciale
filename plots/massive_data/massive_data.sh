#!/usr/bin/env gnuplot
set terminal postscript eps enhanced color font 'Verdana,11'
set output 'massive_data.eps'
set xtics nomirror rotate by -45
set xrange [ -40000 : -200 ]
unset ytics

  set yrange [ 0 : 26000 ]
  #set xlabel "Time"

  set style line 5 lc rgb '#000000' pt 1 ps 1 lt 1 lw 2 # --- black
  set style line 3 lc rgb '#a9a9a9' pt 1 ps 1 lt 1 lw 1 # --- black

  set multiplot

  set border 2+1+4 back 
  set size 0.15,1
  set origin 0.0,0.0
  set lmargin 10
  set rmargin 0
  set ylabel "Data (Gigabytes)"
  #set ytics nomirror
  plot 'massive_data' using 2:3:xticlabels(1) with lines ls 5 notitle

  #set arrow from 0, graph -200 to -200, graph 1 nohead

  set arrow from -200,graph(0,0) to -200,graph(1,1) nohead ls 3
  
  set border 1+4 front
  set origin 0.15,0.0
  set size 0.275,1
  set format y ""
  set noytics
  set lmargin 0
  set rmargin 0
  set xrange [-200 : 1900]
  set ylabel ""
  plot 'massive_data' using 2:3:xticlabels(1) with lines ls 5 notitle

  set arrow from 1940,graph(0,0) to 1940,graph(1,1) nohead ls 3
    
  set border 1+4+8 front
  set origin 0.425,0.0
  set size 0.55,1
  set format y ""
  set noytics
  set lmargin 0
  set rmargin 2
  set xrange [1940 : 2003]
  set ylabel ""
  plot 'massive_data' using 2:3:xticlabels(1) with lines ls 5 notitle

  set nomultiplot
