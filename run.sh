#!/bin/bash

set -e

# list libc implementations
implementations=$(ls -1 impls/qsort_*.c |
                  perl -pe "s/impls\/qsort_(.*)\.c/\1/" |
		  sort)

# compile and run them
for impl in $implementations; do
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
