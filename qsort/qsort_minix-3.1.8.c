/* This file is a copy of lib/libc/ansi/qsort.c from Minix 3.18.

License:

Copyright (c) 1987,1997, 2006, Vrije Universiteit, Amsterdam, The
Netherlands All rights reserved. Redistribution and use of the MINIX 3
operating system in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of the Vrije Universiteit nor the names of the
      software authors or contributors may be used to endorse or
      promote products derived from this software without specific
      prior written permission.
    * Any deviations from these conditions require written permission
      from the copyright holder in advance

Disclaimer

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS, AUTHORS, AND
CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR ANY AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	<stdlib.h>

static	void qsort1(char *, char *, size_t);
static	int (*qcompar)(const char *, const char *);
static	void qexchange(char *, char *, size_t);
static	void q3exchange(char *, char *, char *, size_t);

void
qsort(void *base, size_t nel, size_t width,
      int (*compar)(const void *, const void *))
{
	/* when nel is 0, the expression '(nel - 1) * width' is wrong */
	if (!nel) return;
	qcompar = (int (*)(const char *, const char *)) compar;
	qsort1(base, (char *)base + (nel - 1) * width, width);
}

static void
qsort1(char *a1, char *a2, register size_t width)
{
	register char *left, *right;
	register char *lefteq, *righteq;
	int cmp;

	for (;;) {
		if (a2 <= a1) return;
		left = a1;
		right = a2;
		lefteq = righteq = a1 + width * (((a2-a1)+width)/(2*width));
		/*
		   Pick an element in the middle of the array.
		   We will collect the equals around it.
		   "lefteq" and "righteq" indicate the left and right
		   bounds of the equals respectively.
		   Smaller elements end up left of it, larger elements end
		   up right of it.
		*/
again:
		while (left < lefteq && (cmp = (*qcompar)(left, lefteq)) <= 0) {
			if (cmp < 0) {
				/* leave it where it is */
				left += width;
			}
			else {
				/* equal, so exchange with the element to
				   the left of the "equal"-interval.
				*/
				lefteq -= width;
				qexchange(left, lefteq, width);
			}
		}
		while (right > righteq) {
			if ((cmp = (*qcompar)(right, righteq)) < 0) {
				/* smaller, should go to left part
				*/
				if (left < lefteq) {
					/* yes, we had a larger one at the
					   left, so we can just exchange
					*/
					qexchange(left, right, width);
					left += width;
					right -= width;
					goto again;
				}
				/* no more room at the left part, so we
				   move the "equal-interval" one place to the
				   right, and the smaller element to the
				   left of it.
				   This is best expressed as a three-way
				   exchange.
				*/
				righteq += width;
				q3exchange(left, righteq, right, width);
				lefteq += width;
				left = lefteq;
			}
			else if (cmp == 0) {
				/* equal, so exchange with the element to
				   the right of the "equal-interval"
				*/
				righteq += width;
				qexchange(right, righteq, width);
			}
			else	/* just leave it */ right -= width;
		}
		if (left < lefteq) {
			/* larger element to the left, but no more room,
			   so move the "equal-interval" one place to the
			   left, and the larger element to the right
			   of it.
			*/
			lefteq -= width;
			q3exchange(right, lefteq, left, width);
			righteq -= width;
			right = righteq;
			goto again;
		}
		/* now sort the "smaller" part */
		qsort1(a1, lefteq - width, width);
		/* and now the larger, saving a subroutine call
		   because of the for(;;)
		*/
		a1 = righteq + width;
	}
	/*NOTREACHED*/
}

static void
qexchange(register char *p, register char *q,
	  register size_t n)
{
	register int c;

	while (n-- > 0) {
		c = *p;
		*p++ = *q;
		*q++ = c;
	}
}

static void
q3exchange(register char *p, register char *q, register char *r,
	   register size_t n)
{
	register int c;

	while (n-- > 0) {
		c = *p;
		*p++ = *r;
		*r++ = *q;
		*q++ = c;
	}
}
