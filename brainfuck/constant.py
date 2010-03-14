#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Code generator that given input X prints a brainfuck
# program that outputs X using only two memory cells.
#
# Mats Linander
# 2009-11-01

def gcd(a, b):
    while a and b:
        a, b = max(a, b), min(a, b)
        a -= b
    return max(a,b)

def suitable_neighbours(a, b, d=6):
    """Finds suitable start and end values for common divisor loop.

    With a in the current cell, b will be stored in the adjacent cell
    by running:

    '[%s>%s<]' % ('-' * (a/gcd(a,b)),
                  '+' * (b/gcd(a,b)))

    This will suck badly when gcd(a,b) is small, so it makes sense to
    scan the neighbourhood of a and b for more suitable A,B. Assuming
    A<a and B>b, the loop will be:

    '%s[%s>%s<]>%s' % ('-' * abs(A-a),
                       '-' * (A/gcd(A,B)),
                       '+' * (B/gcd(A,B)),
                       '+' * abs(B-b))

    The optimal choice will be the A,B that minimizes the sum of
    |A-a|, |B-b|, A/gcd(A,B) and B/gcd(A,B).

    @return: (A, B)
    """

    min_clen, opt_A, opt_B = 2*(a+b+d), -1, -1
    for A in range(max(a - d, 1), min(a + d, 0xff) + 1):
        for B in range(max(b - d, 1), min(b + d, 0xff) + 1):
            clen = abs(a-A) + abs(b-B) + (A + B) / gcd(A, B)
            if clen < min_clen:
                min_clen = clen
                opt_A, opt_B = A, B
    assert opt_A > 0
    assert opt_A <= 0xff
    assert opt_B > 0
    assert opt_B <= 0xff
    return (opt_A, opt_B)

def go(x, y):
    return (x < y and '+' or '-') * abs(x - y)

def constant(s):

    reverse = {'>' : '<', '<' : '>'}

    s = list(s)
    code = []
    dir = '>'

    curr = 0
    while s:
        goal = ord(s.pop(0))

        n_curr, n_goal = suitable_neighbours(curr, goal)
        divisor = gcd(n_curr, n_goal)

        len_loop = (abs(n_curr - curr) + n_curr / divisor +
                    abs(n_goal - goal) + n_goal / divisor +
                    len('[><]>'))
        len_direct = abs(curr - goal)

        if len_direct <= len_loop:
            code.append(go(curr, goal))
        else:
            code.append(go(curr, n_curr))
            code.append('[')
            code.append('-' * (n_curr / divisor))
            code.append(dir)
            code.append('+' * (n_goal / divisor))
            code.append(reverse[dir])
            code.append(']')
            code.append(dir)
            code.append(go(n_goal, goal))
            dir = reverse[dir]

        curr = goal
        code.append('.')

    return ''.join(code)

if __name__ == "__main__":
    import sys
    sys.stdout.write(constant(sys.stdin.read()))
    sys.stdout.write('\n')
    sys.exit(0)
