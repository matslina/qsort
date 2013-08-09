#!/bin/bash

set -e

# list libc implementations
libcs=$(ls -1 impls/qsort_*.c |
        perl -pe "s/impls\/qsort_(.*)\.c/\1/" |
        sort)

# compile and run them
for impl in $libcs; do
    gcc -g impls/qsort_$impl.c cmp_count.c -o cmp_$impl
    for dist in inc dec rand; do
	if [ -e data/${dist}_$impl.dat ]; then
	    continue
	fi
	echo $dist $impl
	./cmp_${impl} $dist > data/${dist}_$impl.dat || \
            (echo "fail $impl $dist" && rm data/${dist}_$impl.dat)
    done
    rm cmp_$impl
done

# graph the resulting data
mkdir -p output
python cmp_count_graphs.py

# list all implementations
impls=$(ls -1 impls/*sort_*.c |
        perl -pe "s/impls\/.?sort_(.*)\.c/\1/" |
        sort)

# antiqsort for all implementations
for impl in $impls; do

    # plot 64 entry killer adversary for all
    echo impls/*sort_$impl.c
    gcc antiqsort.c impls/*sort_$impl.c -o anti_$impl -DDUMPDIST
    ./anti_$impl 64 | tail -n+2 > data/anti_dist_$impl.dat
    cat > anti_$impl.p <<EOF
set terminal png
set output "output/anti_$impl.png"
set title "$impl sort killer input"
set style data histogram
set style fill solid
set xrange [-0.5:63.5]
set yrange [0:65]
set ytics out 5
set xtics out
set grid xtics
set grid ytics
set mxtics 5
set mytics 5
set label ''
plot 'data/anti_dist_$impl.dat' using 1 with boxes notitle linecolor rgb "black"
EOF
    gnuplot anti_$impl.p
    rm anti_$impl.p
    rm anti_$impl

    # count comparisons for several input sizes
    gcc antiqsort.c impls/*sort_$impl.c -o anti_$impl
    > data/anti_cmp_$impl.dat
    for nmemb in $(seq 64 64 1024); do
	./anti_$impl $nmemb >> data/anti_cmp_$impl.dat
    done
    rm anti_$impl

    # plot as lines
    cat > anti_cmp_$impl.p <<EOF
set terminal png
set output "output/anti_cmp_lines_$impl.png"
set title "$impl - random vs killer input"
set xlabel "elements"
set ylabel "comparisons"
set xrange [64:1024]
set mxtics 2
set xtics 256
set key top left
set key box
set key spacing 0.9
plot 'data/anti_cmp_$impl.dat' using 1:2 title "killer input" with linespoints linecolor rgb "red", \
     'data/anti_cmp_$impl.dat' using 1:3 title "random input" with linespoints linecolor rgb "blue"

EOF
    gnuplot anti_cmp_$impl.p
done
