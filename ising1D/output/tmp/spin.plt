set term lua tikz
set term tikz standalone size 5in,3in
set pm3d
set pm3d map
set ticslevel 0
set cbrange[-1:1]
set palette defined (-1 'white', 1 'black')
set xrange[0:1000]
set yrange[0:100]
set zrange[-1:1]
set xl 'time'
set yl 'position'
set pm3d corners2color c3
set nokey
set output 'ising.tex'
splot 'spin.dat' with pm3d