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

for impl in $impls; do
    # compile antiqsort
    echo impls/*sort_$impl.c
    gcc antiqsort.c impls/*sort_$impl.c -o anti_$impl

    # plot killer adversary for 64 elements
    ./anti_$impl 64 | tail -n+2 > anti_$impl.out
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
plot 'data/anti_$impl.dat' using 1 with boxes notitle linecolor rgb "black"
EOF
    gnuplot anti_$impl.p
    rm anti_$impl.out
    rm anti_$impl.p
    rm anti_$impl
done
