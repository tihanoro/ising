# グラフの初期設定
set term lua tikz
set term tikz standalone size 5in,3in
set terminal tikz createstyle
set title "Initial arrangement is ordered state"
set xrange [0:5]
set xl '$k_{B}T$'

# グラフのプロット
set yl '$M$'
set yrange [-1.5:1.5]
set output 'magnetic1D.tex'
plot 'magnetic1D-1000.dat' title '$1000$' w l, 'magnetic1D-10000.dat' title '$10000$' w l, 'magnetic1D-100000.dat' using 1:($2/100000) title '100000' w l