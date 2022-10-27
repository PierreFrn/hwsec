set terminal pngcairo size 1280,720 enhanced font 'Verdana,10'
set output 'average.png'
plot \
'average.dat' index 0 notitle with lines linecolor 3
