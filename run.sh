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

# compile antiqsort for all implementations, bsdkiller for freebsd
for impl in $impls; do
    gcc antiqsort.c impls/*sort_$impl.c -o anti_$impl
    gcc antiqsort.c impls/*sort_$impl.c -o dist_anti_$impl -DDUMPDIST
done
echo impls/qsort_freebsd-8.1.0.c
gcc bsdkiller.c impls/qsort_freebsd-8.1.0.c -o anti_freebsd-8.1.0
gcc bsdkiller.c impls/qsort_freebsd-8.1.0.c -o dist_anti_freebsd-8.1.0 -DDUMPDIST

# run and dump data
for impl in $impls; do

    # 64 entry killer adversary
    if [ ! -e data/dist_anti_$impl.dat ]; then
	./dist_anti_$impl 64 > data/dist_anti_$impl.dat
    fi
    rm dist_anti_$impl

    # count comparisons for several input sizes
    if [ ! -e data/anti_$impl.dat ]; then
	> data/anti_$impl.dat
	for nmemb in $(seq 64 64 1024); do
	    ./anti_$impl $nmemb >> data/anti_$impl.dat
	done
    fi
done

# process data
antidata=$(ls -1 data/anti_*.dat | perl -pe 's/^data\/anti_(.*)\.dat$/\1/')
echo antidata $antidata
for impl in $antidata; do

    # plot 64 dist
    cat > dist_anti_$impl.p <<EOF
set terminal png
set output "output/anti_$impl.png"
set title "$impl qsort() killer input"
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
plot 'data/dist_anti_$impl.dat' using 1 with boxes notitle linecolor rgb "black"
EOF
    gnuplot dist_anti_$impl.p
    rm dist_anti_$impl.p

    # plot as lines
    cat > anti_$impl.p <<EOF
set terminal png
set output "output/anti_lines_$impl.png"
set title "$impl - random vs killer input"
set xlabel "elements"
set ylabel "comparisons"
set xrange [64:1024]
set mxtics 2
set xtics 256
set key top left
set key box
set key spacing 0.9
plot 'data/anti_$impl.dat' using 1:2 title "killer input" with linespoints linecolor rgb "red", \
     'data/anti_$impl.dat' using 1:3 title "random input" with linespoints linecolor rgb "blue"

EOF
    gnuplot anti_$impl.p
    rm anti_$impl.p

done

# compose a matrix of a few of the quicksorts
quicksorts="illumos plan9 glibc.*quick"
imgs=""
for quicksort in $quicksorts; do
    imgs="$imgs $(find output -name anti_\*.png | egrep $quicksort.*\\.png\$ | grep -v lines)"
    imgs="$imgs $(find output -name anti_lines\*.png | egrep $quicksort.*\\.png\$)"
    echo $imgs
done

montage $imgs -geometry 320x240 -tile 2 output/anti_montage.png
