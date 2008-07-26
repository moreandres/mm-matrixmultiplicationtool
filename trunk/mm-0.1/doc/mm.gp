#! /usr/bin/gnuplot

set datafile separator ","
set terminal pdf fsize 12
set style fill solid 1.0 border -1
set ylabel "time in seconds"
set xlabel "matrix size in doubles"
set xtics rotate by -45
set boxwidth 0.7
set grid
unset key

set output "simple.pdf"
plot 'simple.csv' using 3:xticlabel(2) notitle with boxes, 'simple.csv' using 3:xticlabel(2) lc rgb "blue" lw 5 with linespoints

set output "block.pdf"
plot 'block.csv' using 3:xticlabel(2) notitle with boxes, 'block.csv' using 3:xticlabel(2) lc rgb "blue" lw 5 with linespoints

set output "transp.pdf"
plot 'transp.csv' using 3:xticlabel(2) notitle with boxes, 'transp.csv' using 3:xticlabel(2) notitle lc rgb "blue" lw 5 with linespoints

set output "blas.pdf"
plot 'blas.csv' using 3:xticlabel(2) notitle with boxes, 'blas.csv' using 3:xticlabel(2) notitle lc rgb "blue" lw 5 with linespoints

set output "blas2.pdf"
set style data histogram
set style histogram gap 0.5
set key left
plot 'blass.csv' using 3:xtic(2) title "single", 'blasm.csv' using 3:xtic(2) title "multi"

set output "timings.pdf"
set style data histogram
set style histogram gap 0.5
set key left
plot 'timings.csv' using 2:xticlabel(1) title "block", '' using 3:xticlabel(1) title "transp", '' using 4:xticlabel(1) title "blas"

set output "blocksize.pdf"
set xlabel "blocksize in doubles"
unset key
plot 'blocksize.csv' using 3:xticlabel(5) notitle with boxes, 'blocksize.csv' using 3:xticlabel(5) lc rgb "blue" lw 5 with linespoints

set output "mpi.pdf"
set xlabel "blocksize in doubles"
set style data histogram
set style histogram gap 0.5
set key left
plot 'mpi.csv' using 3:xtic(2) t 1, '' u 4:xtic(2) t 2, '' u 5:xtic(2) t 3, '' u 6:xtic(2) t 4

set output "flops.pdf"
set xlabel "matrix size in double"
set ylabel "mflops"
set key left
plot 'simple.csv' using 4:xtic(2) lw 5 with linespoints title "simple",  'block.csv' using 4:xtic(2) lw 5 with linespoints title "block", 'transp.csv' using 4:xtic(2) lw 5 with linespoints title "transp", 'blass.csv' using 4:xtic(2) lw 5 with linespoints title "blas"
