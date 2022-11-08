# グラフの初期設定
set term lua tikz
set term tikz standalone size 5in,3in
set xrange [0:5]
set xl '$k_{B}T$'

# グラフのプロット
set yl '$E$'
set yrange [-110:0]
set output 'energy.tex'
plot 'energy.dat' title '$E$' w l, -100*tanh(1/x) title '$E$:Analysis' w l
set yl '$M$'
set yrange [-30:110]
set output 'magnetic.tex'
plot 'magnetic.dat' title '$M$' w l, 0*x title '$M$:Analysis' w l
set yl '$C_{v}$'
set yrange [-10:80]
set output 'heat.tex'
plot 'heat.dat' title '$C_{v}$' w l, 100/(x*x + cosh(1/x)*cosh(1/x)) title '$C_{v}$:Analysis' w l