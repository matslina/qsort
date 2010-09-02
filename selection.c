/* The selection problem.
 *
 * Given list[0...n-1], find the k smallest element.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>


typedef struct algorithm {
  int (*f)(int*, int, int);
  char *name;
} alg_t;


/* Naive algorithm.
 * O(n*k)
 */
int naive(int *list, int n, int k) {
  int i, last, lasti, min, mini;

  last = -1;
  while (k--) {
    for (mini=0; mini<n; mini++)
      if (list[mini] > last || (list[mini] == last && lasti < mini))
	break;
    min = list[mini];
    for (i=mini; i<n; i++)
      if (list[i] < min && (list[i] > last ||
			    (list[i] == last && lasti < i))) {
	mini = i;
	min = list[i];
      }
    lasti = mini;
    last = min;
  }
  return last;
}


/* Partitions A[left...right] around A[pivot].
 * Returns r such that:
 *        A[i] <= A[r] for i=left, ..., r-1
 *        A[i] >  A[r] for i=r+1, ..., right
 */
int partition(int *A, int left, int right, int pivot) {
  int swap, r, l, p;

  r = right;
  l = left + 1;
  p = A[pivot];
  A[pivot] = A[left];

  for (;;) {

    while(r >= l && A[r] > p) r--;
    while(l < r  && A[l] <= p) l++;
    if(l >= r) break;

    swap = A[l]; A[l] = A[r]; A[r] = swap;
  }

  A[left] = A[r];
  A[r] = p;

  return r;
}


/* Quickselect.
 * O(n^2) (avg O(n))
 */
int quickselect(int *list, int n, int k) {
  int left, right, pivot, target;

  target = k - 1;
  left = 0;
  right = n - 1;

  pivot = -1;
  while (pivot != target) {

    pivot = partition(list, left, right, left + random() % (right - left + 1));

    if (pivot <  target)
      left = pivot + 1;
    if (pivot > target)
      right = pivot - 1;
  }

  return list[k-1];
}


int cmp_int(const void *a, const void *b) {
  return *(int *)a - *(int *)b;
}


/* Selection by sorting.
 * O(n log n)
 */
int sortselect(int *list, int n, int k) {
  qsort(list, n, sizeof(int), cmp_int);
  return list[k-1];
}


/* The median-of-medians algorithm.
 * O(n)
 */
int medianofmedians(int *list, int n, int k) {
  int i, mm, mmi, pivot, target;
  int *medians;

  medians = malloc((n/5)*sizeof(int));

  target = k - 1;

  while (n >= 5) {

    for (i=0; i<n/5; i++) {
      qsort(list + i*5, 5, sizeof(int), cmp_int);
      medians[i] = list[i*5+2];
    }

    mm = medianofmedians(medians, n/5, (n/5)/2+1);
    for (mmi=2; list[mmi]!=mm; mmi+=5);

    pivot = partition(list, 0, n-1, mmi);

    if (pivot == target) {
      free(medians);
      return list[pivot];
    }
    else if (pivot < target) {
      list += pivot + 1;
      n -= pivot + 1;
      target -= pivot + 1;
    }
    else if (pivot > target)
      n = pivot;
  }

  free(medians);
  qsort(list, n, sizeof(int), cmp_int);
  return list[target];
}


alg_t algs[] = {{.name="naive", .f=naive},
		{.name="sortselect", .f=sortselect},
		{.name="quickselect", .f=quickselect},
		{.name="medianofmedians", .f=medianofmedians}};
int nalgs = sizeof(algs) / sizeof(alg_t);


int main(int argc, char **argv) {
  int n, k, i, alg_i;
  int *list, *orig, *result;
  clock_t time_start, time_stop;
  alg_t alg;

  n = k = -1;
  if (argc == 3) {
    sscanf(argv[1], "%d", &n);
    sscanf(argv[2], "%d", &k);
  }
  if (n <= 0 || k >= n) {
    fprintf(stderr, "Usage: %s <n> <k>\n", argv[0]);
    return 1;
  }

  orig = malloc(n * sizeof(int));
  list = malloc(n * sizeof(int));
  result = malloc(nalgs * sizeof(int));

  for (i=0; i<n; i++)
    orig[i] = (int)(random() % n);

  for (alg_i=0; alg_i<nalgs; alg_i++) {
    alg = algs[alg_i];
    memcpy(list, orig, n * sizeof(int));

    time_start = clock();
    result[alg_i] = alg.f(list, n, k);
    time_stop = clock();

    printf("%s: %.2f\n", alg.name,
	   ((double)(time_stop - time_start))/CLOCKS_PER_SEC);
  }

  for (alg_i=1; alg_i<nalgs; alg_i++)
    if (result[alg_i] != result[alg_i-1]) {
      fprintf(stderr,"mismatch between %s (%d) and %s (%d)\n",
	      algs[alg_i].name, result[alg_i], algs[alg_i-1].name, result[alg_i-1]);
      return 1;
    }

  return 0;
}
