#!/usr/bin/gnuplot 

set boxwidth 1 relative
set style data histograms
set style fill solid 1.0
set ylabel "execution time(10^-6 s)"
set y2label "#"
set ytics nomirror
set y2tics 0,20
set ytics nomirror
set datafile separator "\t"
set terminal png enhanced truecolor
set title "PDG construction in prog. 'PROGNAME'"
set output 'PROGNAME.png'
plot 'PROGNAME.tsv' using 2:xticlabels(1) title "time(micro-sec.)", \
 ''  using 4:xticlabels(1) axis x1y2 title "# instruction"
