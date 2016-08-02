# Gnuplot script file for plotting p1 data
#
set title "FFT-estimated moment vs. simulated data - Trip 1 velocity"
set xlabel "V1 - simulated (dBM)"
set ylabel "V1 - FFT (dBM)"
set xrange[-30:30] 
set yrange[-30:30] 
set nolabel
set nokey
plot "verify_fft/moments.table" using 4:3

