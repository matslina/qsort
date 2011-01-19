#!/bin/bash

set -e

dist=$1
test -z "$dist" && dist="rand"

# list implementations
implementations=$(ls -1 qsort_*.c | perl -pe "s/qsort_(.*)\.c/\1/")

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

# sort datfiles by max cmp count in falling order
datfiles=$((for datf in ${dist}_*.dat; do \
                echo $(tail -n 1 $datf | awk '{print $2}') $datf; \
           done) | sort -nr | awk '{print $2}')
last_datf=$(echo $datfiles | tr ' ' '\n' | tail -n 1)

# build gnuplot script
cat > count_$dist.p <<EOF
set title "sorting $dist data"
set xlabel "elements"
set ylabel "comparisons"
#set logscale x 2
set key top left
set key box
EOF

echo -n "plot " >> count_$dist.p
for datf in $datfiles; do

    impl=$(echo $datf | perl -pe "s/${dist}_(.*)\.dat/\1/")

    echo -n "'$datf' title \"$impl\" with linespoints" \
	>> count_$dist.p
    test $datf == $last_datf && echo >> count_$dist.p || echo ', \' \
	>> count_$dist.p
done
echo "pause mouse" >> count_$dist.p

# build histogram plot of the min and max values
rm -f max_$dist.dat min_$dist.dat
for datf in $datfiles; do
    impl=$(echo $datf | perl -pe "s/${dist}_(.*)\.dat/\1/")
    echo $impl $(tail -n 1 $datf | awk '{print $2}') >> max_$dist.dat
    echo $impl $(head -n 1 $datf | awk '{print $2}') >> min_$dist.dat
done
cat > max_$dist.p <<EOF
set title "sorting $(tail -n 1 $datf | awk '{print $1}') elements"
set auto x
set style data histogram
set style histogram cluster gap 1
set style fill solid 1.0 border -1
set xtic rotate by -45 scale 0
set boxwidth 0.9
set ylabel "comparisons"
plot 'max_$dist.dat' using 2:xticlabel(1) notitle
pause mouse
EOF
cat max_$dist.p | \
    sed -e "s/rting.*el/rting $(head -n 1 $datf | awk '{print $1}') el/" | \
    sed -e "s/max_$dist/min_$dist/" > min_$dist.p
