# グラフの初期設定
set term lua tikz
set term tikz standalone size 5in,3in
set terminal tikz createstyle
set xrange [0:10]
set xl '$k_{B}T$'

# グラフのプロット
set yl '$E$'
set yrange [-11000:0]
set output 'energy1D.tex'
plot 'energy1D.dat' title '$E$' w l
set yl '$M$'
set yrange [-1000:11000]
set output 'magnetic1D.tex'
plot 'magnetic1D.dat' title '$M$' w l
set yl '$C_{v}$'
set yrange [0:0.5]
set output 'heat1D.tex'
plot 'heat1D.dat' title '$C_{v}$' w l
set yrange [0:40]
set output 'mag_suscep1D.tex'
plot 'mag_suscep1D.dat' title '$\chi$' w l