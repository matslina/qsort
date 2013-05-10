#!/usr/bin/python
# -*- coding: utf-8 -*-

"""Builds some nice gnuplot graphs.

This is tightly coupled with the rest of the qsort stuff.

Gnuplot files will be (over)written in the current working
directory. Data files are expected to exist in the current working
directory.
"""

import sys
import os
import os.path
import re
import math
import subprocess

COUNT_TEMPLATE = """
set terminal png
set output "count_%(dist)s.png"
set title "sorting %(distname)s data"
set xlabel "elements"
set ylabel "comparisons"
set xrange [0:%(xrange_upper)s]
set xtics 1000000
set key top left
set key box
set key spacing 0.9
plot %(plots)s
"""

COUNT_PLOT_TEMPLATE = ("'%(datfile)s' title \"%(implementation)s\" "
                       "with linespoints lt %(lt)d pt %(pt)d")

MINMAX_TEMPLATE = """
set terminal png
set output "%(minmax)s_%(dist)s.png"
set title "sorting approximately 2^%(elslg)d %(distname)s elements"
set auto x
set yrange [0:*]
set style data histogram
set style histogram cluster gap 1
set style fill solid 1.0 noborder
set grid y
set xtic rotate by -45 scale 0
set boxwidth 0.9
set ylabel "%(unit)s comparisons"
plot '%(minmax)s_%(dist)s.dat' using 2:xticlabel(1) notitle
"""


def main():

    # load the data from all .dat files
    # map dist -> (map implementation name -> (map elements -> comparisons))
    data = {}
    for dist in ('inc', 'dec', 'rand'):
        data[dist] = {}
        datfile_re = re.compile(r'^%s_(.+)\.dat$' % dist)
        for f in os.listdir('.'):
            if not os.path.isfile(f):
                continue
            m = datfile_re.match(f)
            if m is None:
                continue

            data[dist][m.group(1)] = dict(map(int, row.strip().split())
                                          for row in open(f) if row.split())

    # warn if we lack data for some distributions
    inc_impl = set(data['inc'])
    dec_impl = set(data['dec'])
    rnd_impl = set(data['rand'])
    if inc_impl != dec_impl or dec_impl != rnd_impl:
        complete_impl = inc_impl & dec_impl & rnd_impl
        odd_impl = (impl for impl in (inc_impl | dec_impl | rnd_impl)
                    if impl not in complete_impl)
        sys.stderr.write("WARNING: .dat-files missing 1 or more dists "
                         "for: %s\n" % ', '.join(odd_impl))

    # order the implemenations by the minimum number of comparisons
    # performed for the random distribution. ignore implementations
    # lacking data for the random distribution.
    impls = sorted(data['rand'],
                   key=lambda k: min(data['rand'][k].values()),
                   reverse=True)

    # assign color and line type for all implementations
    ltpt = dict((impl, (i + 1, i + 1)) for i, impl in enumerate(impls))
    i = len(ltpt)
    for d in data.values():
        for impl in d:
            if impl not in impls:
                i += 1
                ltpt[impl] = (i + 1, i + 1)
                impls.append(impl)

    # create the count gnuplot graphs
    for dist in ('inc', 'dec', 'rand'):
        param = {'dist': dist,
                 'distname': {'inc': 'increasing',
                              'dec': 'decreasing',
                              'rand': 'random'}.get(dist, dist),
                 'xrange_upper': max(max(d) for d in data[dist].values()),
                 'plots': ', \\\n'.join(COUNT_PLOT_TEMPLATE %
                                        {'datfile': '%s_%s.dat' % (dist, impl),
                                         'implementation': impl,
                                         'lt': ltpt[impl][0],
                                         'pt': ltpt[impl][1]}
                                        for impl in impls
                                        if impl in data[dist])}

        open('count_%s.p' % dist, 'w').write(COUNT_TEMPLATE % param)
        print "wrote count_%s.p" % dist

    # histogram the comparisons for the largest and smallest number of
    # elements

    for dist in ('inc', 'dec', 'rand'):
        for f, f_name in [(min, 'min'), (max, 'max')]:

            # figure out which number of elements we're looking at
            numels = f(data[dist].values()[0])

            # implemenation name and comparison count for that number
            # of elements
            hists = [(impl, data[dist][impl][numels])
                     for impl in impls if impl in data[dist]]

            # try to count in a nicer unit than single comparisons
            maxcmps = max(h[1] for h in hists)
            unit = 10 ** (math.log(maxcmps, 10) - math.log(maxcmps, 10) % 3)
            if unit == 10 ** 3:
                unit_name = "thousands of "
            elif unit == 10 ** 6:
                unit_name = "millions of "
            elif unit == 10 ** 9:
                unit_name = "billions of "
            else:
                unit_name = ""
                unit = 1

            # write the .dat file
            datf = ''.join('%s %f\n' % (h[0], h[1] / float(unit))
                           for h in hists)
            open('%s_%s.dat' % (f_name, dist), "w").write(datf)

            # write the gnuplot script
            distname = {'inc': 'increasing',
                        'dec': 'decreasing',
                        'rand': 'random'}.get(dist, dist)
            gplot = MINMAX_TEMPLATE % {'minmax': f_name,
                                       'dist': dist,
                                       'elslg': round(math.log(numels, 2)),
                                       'distname': distname,
                                       'unit': unit_name}
            open('%s_%s.p' % (f_name, dist), "w").write(gplot)
            print "wrote %s_%s.p" % (f_name, dist)

            # and finally create the png
            subprocess.call(["gnuplot", "%s_%s.p" % (f_name, dist)])

    return 0

if __name__ == "__main__":
    sys.exit(main())
