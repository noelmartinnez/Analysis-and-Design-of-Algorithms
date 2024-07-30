# NOEL MARTÍNEZ POMARES 48771960T

# quickSort.png

set terminal png
set output "quickSort.png"

set xlabel "size of arrays"
set ylabel "Millions of program steps"

set yrange[0:25]
set xrange[2**15:2**20]

set xtics ("2¹⁶" 2**16, "2¹⁷" 2**17, "2¹⁸" 2**18, "2¹⁹" 2**19, "2²⁰" 2**20) 

set title  "Quicksort"

plot "qs-vs-hs.Msteps" using 1:2 with linespoints title "random arrays", "qs-vs-hs.Msteps" using 1:4 with linespoints title "sorted arrays", "qs-vs-hs.Msteps" using 1:6 with linespoints title "reverse-sorted arrays" 

# heapSort.png

set terminal png
set output "heapSort.png"

set xlabel "size of arrays"
set ylabel "Millions of program steps"

set title  "Heapsort"

plot "qs-vs-hs.Msteps" using 1:3 with linespoints title "random arrays", "qs-vs-hs.Msteps" using 1:5 with linespoints title "sorted arrays" , "qs-vs-hs.Msteps" using 1:7 with linespoints title "reverse-sorted arrays"

# qs-vs-hs-RA.png

set terminal png
set output "qs-vs-hs-RA.png"

set xlabel "size of arrays"
set ylabel "Millions of program steps"

set title  "Quicksort versus Heapsort (random arrays)"

##Fit to a (n log n + n) function:
y(x)=a*x*log(x) + b*x + c
fit y(x) "qs-vs-hs.Msteps" using 1:2 via a,b,c 

z(x)=d*x*log(x) + e * x + f 
fit z(x) "qs-vs-hs.Msteps" using 1:3 via d,e,f

plot "qs-vs-hs.Msteps" using 1:2 with points title "QuickSort", "qs-vs-hs.Msteps" using 1:3 with points title "HeapSort", y(x) title "fitting Quicksort time values to ...", z(x) title "fitting Heapsort time values to ..."

# qs-vs-hs-SA.png

set terminal png
set output "qs-vs-hs-SA.png" 

set xlabel "size of arrays"
set ylabel "Millions of program steps"

set title  "Quicksort versus Heapsort (sorted arrays)"

##Fit to a (n log n + n) function: 
y(x)=a*x*log(x) + b * x + c
fit y(x) "qs-vs-hs.Msteps" using 1:4 via a,b,c

z(x)=d*x*log(x) + e * x + f 
fit z(x) "qs-vs-hs.Msteps" using 1:5 via d,e,f

plot "qs-vs-hs.Msteps" using 1:4 with points title "QuickSort", "qs-vs-hs.Msteps" using 1:5 with points title "HeapSort", y(x) title "fitting Quicksort time values to ...", z(x) title "fitting Heapsort time values to ..."

# qs-vs-hs-RSA.png
set terminal png
set output "qs-vs-hs-RSA.png" 

set xlabel "size of arrays"
set ylabel "Millions of program steps"

set yrange[0:20]
set ytics 2

set title  "Quicksort versus Heapsort (reverse-sorted arrays)"

##Fit to a (n log n + n) function: 
y(x)=a*x*log(x) + b * x + c
fit y(x) "qs-vs-hs.Msteps" using 1:6 via a,b,c

z(x)=d*x*log(x) + e * x + f 
fit z(x) "qs-vs-hs.Msteps" using 1:7 via d,e,f

plot "qs-vs-hs.Msteps" using 1:6 with points title "QuickSort", "qs-vs-hs.Msteps" using 1:7 with points title "HeapSort", y(x) title "fitting Quicksort time values to ...", z(x) title "fitting Heapsort time values to ..."