/* Killer adversary for 4.4BSD Lite qsort().
 *
 * Note that NetBSD qsort() is not vulnerable to this particular
 * permutation, as of a fairly recent version.
 *
 * Compile this program and link against the BSD libc qsort.
 */

#include <stdio.h>
#include <stdlib.h>

int count;

int cmp_int(const void *a, const void *b) {
  count++;
  return *(int *)a - *(int *)b;
}

int main(int argc, char *argv[]) {
  int *array;
  int n, i;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <nmemb>\n", argv[0]);
    return 1;
  }

  if (sscanf(argv[1], "%d", &n) != 1) {
    fprintf(stderr, "Bad nmemb, expected decimal integer, found '%s'\n", argv[1]);
    return 1;
  }

  array = malloc(n * sizeof(int));

  /* BSD samples 9 elements when selecting pivot for quicksort
   * partitioning:
   *
   * pivot = median(median(v[0], v[n/8], v[n/4]),
   *                median(v[n/2 - n/8], v[n/2], v[n/2 + n/8]),
   *                median(v[n - 1 - n/4], v[n - 1 - n/8], v[n - 1]))
   *
   * If the data is already partitioned around the selected pivot,
   * i.e. if no element swaps are performed during the partitioning
   * round, then qsort() will resort to insertion sort.
   */

  for (i = 0; i < n / 2; i++)
    array[i] = n / 2 - i;
  array[n / 2] = n / 2 + 1;
  for (i = n / 2 + 1; i < n; i++)
    array[i] = n  + n / 2 + 1 - i;

#ifdef DUMPDIST
  for (i = 0; i < n; i++)
    printf("%d\n", array[i]);
  return 0;
#endif

  /* qsort() the killer adversary and count comparisons */
  printf("%d ", n);
  qsort(array, n, sizeof(int), cmp_int);
  printf("%d ", count);

  /* qsort() a random distribution for comparison */
  for (i = 0; i < n; i++)
    array[i] = rand();
  count = 0;
  qsort(array, n, sizeof(int), cmp_int);
  printf("%d\n", count);

  return 0;
}
