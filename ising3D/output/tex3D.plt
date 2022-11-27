# グラフの初期設定
set term lua tikz
set term tikz standalone size 5in,3in

# gnuplot-tikzのスタイルファイルが欲しいときはこのコマンドを有効化する
# set terminal tikz createstyle

set xrange [0:20]
set xl '$k_{B}T[J]$'

# グラフのプロット
set yl '$E[J]$'
set yrange [-400000:0]
set output 'energy3D.tex'
plot 'energy3D.dat' title '$E$' w l
set yl '$M/\mu\{B}[]$'
set yrange [-10000:150000]
set output 'magnetic3D.tex'
plot 'magnetic3D.dat' title '$M$' w l
set yl '$C_{V}[J/K]$'
set yrange [0:0.5]
set output 'heat3D.tex'
plot 'heat3D.dat' title '$C_{v}$' w l
set yl '$\chi$[/K]'
set yrange [0:5]
set output 'mag_suscep3D.tex'
plot 'mag_suscep3D.dat' title '$\chi$' w l