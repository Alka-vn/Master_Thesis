set term pdf
set output "./hexagonal-topology.gnuplot.pdf"
set style arrow 1 lc "black" lt 1 head filled
set xrange [-801:801]
set yrange [-801:801]
set arrow 1 from 0,0 rto 14.4338,8.33333 arrowstyle 1 
set object 1 polygon from \
-1, -66.6667 to \
-58.735, -33.3333 to \
-58.735, 33.3333 to \
-1, 66.6667 to \
56.735, 33.3333 to \
56.735, -33.3333 to \
-1, -66.6667 front fs empty 
set label 1 "1" at -1 , 0 center
set label at -28.0601 , 45.6577 point pointtype 7 pointsize 0.2 center
set label at 44.1378 , 7.48829 point pointtype 7 pointsize 0.2 center
set label at 34.0761 , -21.893 point pointtype 7 pointsize 0.2 center
set label at -37.0927 , -3.70587 point pointtype 7 pointsize 0.2 center
set label at 32.1998 , -13.0766 point pointtype 7 pointsize 0.2 center
set label at -46.8737 , 26.0433 point pointtype 7 pointsize 0.2 center
set label at 11.5482 , 13.8051 point pointtype 7 pointsize 0.2 center
set label at 31.2183 , 31.2956 point pointtype 7 pointsize 0.2 center
set label at 7.23587 , -56.3176 point pointtype 7 pointsize 0.2 center
set label at -34.8558 , -31.839 point pointtype 7 pointsize 0.2 center
unset key
plot 1/0
