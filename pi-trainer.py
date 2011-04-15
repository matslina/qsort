#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright (c) 2010 Spotify AB

import sys
import os.path

DECIMALS = ("14159265358979323846264338327950288419716939937510582097494"
            "45923078164062862089986280348253421170679821480865132823066"
            "47093844609550582231725359408128481117450284102701938521105"
            "55964462294895493038196442881097566593344612847564823378678"
            "31652712019091456485669234603486104543266482133936072602491"
            "41273724587006606315588174881520920962829254091715364367892"
            "59036001133053054882046652138414695194151160943305727036575"
            "95919530921861173819326117931051185480744623799627495673518"
            "85752724891227938183011949129833673362440656643086021394946"
            "39522473719070217986094370277053921717629317675238467481846"
            "76694051320005681271452635608277857713427577896091736371787"
            "21468440901224953430146549585371050792279689258923542019956"
            "1121290219608640344181598136297747713099605187072113")


def request_decimals():
    sys.stdout.write("π=3.")
    decimals = sys.stdin.readline().strip()
    match = os.path.commonprefix([decimals, DECIMALS])
    return (len(match), len(decimals))

def main():

    required = 0
    if len(sys.argv) > 1:
        if len(sys.argv) > 2:
            sys.stderr.write("Usage: %s [required number of correct decimals]\n")
            return 1
        required = int(sys.argv[1])
        if requried < 0:
            sys.stderr.write("Can't require less than 0 correct decimals\n")
            return 1

    match, entered = request_decimals()

    sys.stdout.write("π=3.%s (%d correct)\n" %
                     (DECIMALS[:max(entered+4, required)], match))
    if match < entered:
        sys.stdout.write("   %s%s\n" % (" " * match, "^ fail"))

    if required and match < required:
        return 1
    return 0

if __name__ == "__main__":
    sys.exit(main())
