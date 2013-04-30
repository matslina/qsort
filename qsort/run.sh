#!/bin/bash

set -e

dist=$1
test -z "$dist" && dist="rand"

# list implementations
implementations=$(ls -1 qsort_*.c | perl -pe "s/qsort_(.*)\.c/\1/" | sort)

# compile
for impl in $implementations; do
    gcc -g qsort_$impl.c cmp_count.c -o cmp_$impl
done

# run
for impl in $implementations; do
    if [ -e ${dist}_$impl.dat ]; then
	continue
    fi
    echo $dist $impl
    ./cmp_${impl} $dist > ${dist}_$impl.dat || (echo "fail $impl $dist" && rm ${dist}_$impl.dat)
    rm cmp_${impl}
done
