# グラフの初期設定
set term lua tikz
set term tikz standalone size 5in,3in
set terminal tikz createstyle
set title "Initial arrangement is chaos state"
set xrange [0:2]
set xl '$k_{B}T$'

# グラフのプロット
set yl '$M$'
set yrange [-1.5:1.5]
set output 'magnetic1D.tex'
plot 'magnetic1D-10000-1.dat' title '$10000-1$' w l, 'magnetic1D-10000-2.dat' title '$10000-2$' w l, 'magnetic1D-10000-3.dat' title '$10000-3$' w l