# グラフの初期設定
set term lua tikz
set term tikz standalone size 5in,3in
set xrange [0:10]
set xl '$k_{B}T$'

# グラフのプロット
set yl '$E$'
set yrange [-1:0]
set output 'energy2D.tex'
plot 'energy2D.dat' title '$E$' w l
set yl '$M$'
set yrange [-1:4]
set output 'magnetic2D.tex'
plot 'magnetic2D.dat' title '$M$' w l
set yl '$C_{v}$'
set yrange [0:20]
set output 'heat2D.tex'
plot 'heat2D.dat' title '$C_{v}$' w l