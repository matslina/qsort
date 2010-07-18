/* The selection problem.
 *
 * Given list[0...n-1], find the k smallest elements.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>


typedef struct algorithm {
  void (*f)(int*, int, int, int*);
  char *name;
} alg_t;


/* Naive algorithm.
 * O(n*k)
 */
void naive(int *list, int n, int k, int *out) {
  int oi, i;

  int last, lasti;
  int min, mini;

  last = -1;

  for (oi=0; oi<k; oi++) {

    for (i=1, mini=0, min=list[0]; i<n; i++) {
      if(list[i] < min && (list[i] > last ||
			   (list[i] == last && lasti < i))) {
	min = list[i];
	mini = i;
      }
    }
    out[oi] = mini;
    lasti = mini;
    last = min;
  }

  for (i=0; i<k; i++)
    out[i] = list[out[i]];
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
void quickselect(int *list, int n, int k, int *out) {
  int i, left, right, pivot, target;

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

  for (i=0; i<k; i++)
    out[i] = list[i];
}


int cmp_int(const void *a, const void *b) {
  return *(int *)a - *(int *)b;
}


alg_t algs[] = {{.name="naive", .f=naive},
		{.name="quickselect", .f=quickselect}};
int nalgs = sizeof(algs) / sizeof(alg_t);


int main(int argc, char **argv) {
  int n, k, i, alg_i;
  int *list, *orig, **min;
  clock_t start;
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
  min = malloc(k * sizeof(int *));

  for (i=0; i<n; i++)
    orig[i] = (int)(random() % n);

  for (alg_i=0; alg_i<nalgs; alg_i++) {
    alg = algs[alg_i];
    min[alg_i] = malloc(k * sizeof(int));
    memcpy(list, orig, n * sizeof(int));

    start = clock();
    alg.f(list, n, k, min[alg_i]);
    printf("%s: %.2f\n", alg.name, ((double)(clock() - start))/CLOCKS_PER_SEC);

    qsort(min[alg_i], k, sizeof(int), &cmp_int);
  }

  for (alg_i=1; alg_i<nalgs; alg_i++)
    if (memcmp(min[alg_i], min[alg_i-1], k)) {
      fprintf(stderr,"mismatch between %s and %s\n",
	      algs[alg_i].name, algs[alg_i-1].name);
      return 1;
    }

  return 0;
}
