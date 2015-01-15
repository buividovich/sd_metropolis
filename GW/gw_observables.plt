reset
set term postscript color enhanced landscape "Helvetica" 24
cd 'G:\\LAT\\sd_metropolis\\GW\\data\\'
set pointsize 1.5
set bar 1
set style line 1  lt 1 lc rgb '#000000' lw 8 pt 5
set style line 2  lt 1 lc rgb '#FF0000' lw 8 pt 7
set style line 3  lt 1 lc rgb '#00FF00' lw 8 pt 9
set style line 4  lt 1 lc rgb '#0000FF' lw 8 pt 11
set style line 5  lt 1 lc rgb '#FF00FF' lw 8 pt 13
set style line 6  lt 1 lc rgb '#00FFFF' lw 8 pt 15
set style line 7  lt 1 lc rgb '#888888' lw 8 pt 1
set style line 11 lt 2 lc rgb '#000000' lw 8 pt 5
set style line 12 lt 2 lc rgb '#FF0000' lw 8 pt 7
set style line 13 lt 2 lc rgb '#00FF00' lw 8 pt 9
set style line 14 lt 2 lc rgb '#0000FF' lw 8 pt 11
set style line 15 lt 2 lc rgb '#FF00FF' lw 8 pt 13
set style line 16 lt 2 lc rgb '#00FFFF' lw 8 pt 15
set style line 17 lt 2 lc rgb '#888888' lw 8 pt 1

set out    'G:\\LAT\\sd_metropolis\\GW\\Gn.eps'
set key left top
set xlabel "{/Symbol l}"
set ylabel "{/Symbol l} G_n({/Symbol l})"
plot\
'obs_mode0_nmc5000000.dat'  using ($1):($1*$2):($1*$3) title 'G_1, N_{mc}=5x10^6'  with yerrorbars ls 1,\
'obs_mode0_nmc5000000.dat'  using ($1):($1*$5):($1*$6) title 'G_2, N_{mc}=5x10^6'  with yerrorbars ls 2


set out    'G:\\LAT\\sd_metropolis\\GW\\sign_problem.eps'
unset logscale y
unset logscale x
set key left top
set xlabel "{/Symbol l}"
set ylabel "<(G_+ - G_-)/(G_+ + G_-)>"
plot\
'obs_mode0_nmc5000000.dat'  using ($1):($4) title 'G_1'  with points ls 2
