#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "btree.h"

int cmp_int(const void *a, const void *b) {
  return *(int *)a - *(int *)b;
}

int main(int argc, char *argv[]) {
  int i, n, count;
  int *A;
  bt_node_t *T, *x;
  clock_t time_start, time_stop;

  n = -1;
  if (argc == 2) {
    sscanf(argv[1], "%d", &n);
  }
  if (n <= 0) {
    fprintf(stderr, "Usage: %s <n>\n", argv[0]);
    return 1;
  }

  A = malloc(n*sizeof(int));
  for (i=0; i<n; i++)
    A[i] = (int)(random() % n);

  /* insert */
  time_start = clock();
  T = bt_new(A[0], 0);
  for (i=1; i<n; i++)
    bt_set(T, A[i], i);
  time_stop = clock();
  printf("insert %d: %.2f\n", n,
	 ((double)(time_stop - time_start))/CLOCKS_PER_SEC);

  /* check */
  bt_stat(T, &stat);
  if (stat.broken_node) {
    printf("tree broken at key %d (depth %d): %s\n", stat.broken_node->key, stat.broken_depth, stat.broken_msg);
    return 1;
  }

  /* search */
  time_start = clock();
  for (i=0; i<n; i++) {
    x = bt_get(T, A[i]);
    if (x->key != A[i] || A[x->value] != A[i]) {
      printf("FAIL bt_get(%d) = (%d, %d)\n", A[i], x->key, x->value);
      return 1;
    }
  }
  time_stop = clock();
  printf("search %d: %.2f\n", n,
	 ((double)(time_stop - time_start))/CLOCKS_PER_SEC);

  qsort(A, n, sizeof(int), cmp_int);

  /* traverse */
  time_start = clock();
  for (i=0, count=0, x=bt_min(T); x; x=bt_next(x), count++) {
    if (x->key != A[i]) {
      printf("FAIL bt_next %d: %d != %d\n", i, x->key, A[i]);
      return 1;
    }
    for(i=i+1; i<n && A[i]==A[i-1]; i++);
  }
  time_stop = clock();
  printf("traverse %d: %.2f\n", count,
	 ((double)(time_stop - time_start))/CLOCKS_PER_SEC);

  return 0;
}
