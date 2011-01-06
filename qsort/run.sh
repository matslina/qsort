#!/bin/bash

set -e

rm *.o || true

for qsort_c in qsort_*.c; do
    gcc -O2 -c $qsort_c
done

for qsort_o in qsort_*.o; do
    gcc -O2 cmp_count.c $qsort_o -o cmp
    echo $qsort_o
    ./cmp
    echo
done

rm cmp
