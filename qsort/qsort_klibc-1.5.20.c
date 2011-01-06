/* This is a copy of klibc-1.5.20/usr/klibc/qsort.c with the memswap
 * routine from klibc-1.5.20/usr/klibc/memswap.c inserted.
 *
 * License appears to be:
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * Any copyright notice(s) and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


/*
 * qsort.c
 *
 * This is actually combsort.  It's an O(n log n) algorithm with
 * simplicity/small code size being its main virtue.
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/*
 * memswap()
 *
 * Swaps the contents of two nonoverlapping memory areas.
 * This really could be done faster...
 */
void memswap(void *m1, void *m2, size_t n)
{
	char *p = m1;
	char *q = m2;
	char tmp;

	while (n--) {
		tmp = *p;
		*p = *q;
		*q = tmp;

		p++;
		q++;
	}
}

static inline size_t newgap(size_t gap)
{
	gap = (gap * 10) / 13;
	if (gap == 9 || gap == 10)
		gap = 11;

	if (gap < 1)
		gap = 1;
	return gap;
}

void qsort(void *base, size_t nmemb, size_t size,
	   int (*compar) (const void *, const void *))
{
	size_t gap = nmemb;
	size_t i, j;
	char *p1, *p2;
	int swapped;

	if (!nmemb)
		return;

	do {
		gap = newgap(gap);
		swapped = 0;

		for (i = 0, p1 = base; i < nmemb - gap; i++, p1 += size) {
			j = i + gap;
			if (compar(p1, p2 = (char *)base + j * size) > 0) {
				memswap(p1, p2, size);
				swapped = 1;
			}
		}
	} while (gap > 1 || swapped);
}
