#include <stdlib.h>
#include <stdio.h>
#include <limits.h>



/***********************
 * McIlroy's antiqsort *
 ***********************/
#define IS_GAS(i) (data[i] == gas)
#define IS_SOLID(i) (!IS_GAS(i))
#define FREEZE(i) data[i] = frozen++
int frozen = 1;
int gas;
int *data;

int antiqsort_cmp(const void *ap, const void *bp) {
  int a, b;
  static int candidate;

  a = *(int *)ap;
  b = *(int *)bp;

  if (IS_SOLID(a) && IS_GAS(b)) {
    candidate = b;
    return -1;
  }

  if (IS_GAS(a) && IS_SOLID(b)) {
    candidate = a;
    return 1;
  }

  if (IS_GAS(a) && IS_GAS(b)) {
    if (a == candidate) {
      FREEZE(a);
      return -1;
    } else {
      FREEZE(b);
      return 1;
    }
  }

  return data[a] - data[b];
}

int *antiqsort(int nmemb) {
  int *refs;
  int i;

  data = malloc(nmemb * sizeof(int));
  refs = malloc(nmemb * sizeof(int));
  gas = nmemb;
  for (i=0; i<nmemb; i++) {
    refs[i] = i;
    data[i] = gas;
  }

  qsort(refs, nmemb, sizeof(int), antiqsort_cmp);

  free(refs);
  return data;
}

/**********************************
 * Comparison counting comparator *
 **********************************/

int count;
int cmp_int(const void *a, const void *b) {
  count++;
  return *(int *)a - *(int *)b;
}

/****************
 * int main() ? *
 ****************/

int main(int argc, char *argv[]) {
  int nmemb, *worst, i;

  /* arguments */
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <nmemb>\n", argv[0]);
    return 1;
  }
  if (sscanf(argv[1], "%d", &nmemb) != 1) {
    fprintf(stderr, "Fail: '%s' is not a number\n", argv[1]);
    fprintf(stderr, "Usage: %s <nmemb>\n", argv[0]);
    return 1;
  }

  /* generate and print a McIlroy killer adversary */
  worst = antiqsort(nmemb);
  for (i=0; i<nmemb; i++)
    printf("%d%c", worst[i], i==nmemb-1 ? '\n' : ' ');

  /* qsort() the killer adversary and count comparisons */
  qsort(worst, nmemb, sizeof(int), cmp_int);
  printf("%d\n", count);

  return 0;
}
