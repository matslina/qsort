#!/bin/bash

set -e

dist=$1
test -z "$dist" && dist="rand"

implementations=$(ls -1 qsort_*.c | perl -pe "s/qsort_(.*)\.c/\1/")

for impl in $implementations; do
    gcc -O2 qsort_$impl.c cmp_count.c -o cmp_$impl
    last_impl=$impl
done

for impl in $implementations; do
    echo $dist $impl
    ./cmp_${impl} $dist | tee ${dist}_$impl.dat
    rm cmp_${impl}
done

cat > count_$dist.p <<EOF
set title "sorting $dist data"
set xlabel "elements"
set ylabel "comparisons"
#set logscale x 2
set key top left
set key box
EOF

echo -n "plot " >> count_$dist.p
for impl in $implementations; do
    echo -n "'${dist}_${impl}.dat' title \"$impl\" with linespoints" \
	>> count_$dist.p
    test $impl == $last_impl && echo >> count_$dist.p || echo ', \' \
	>> count_$dist.p
done

echo "pause mouse" >> count_$dist.p
