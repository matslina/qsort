#include <stdlib.h>
#include <stdio.h>
#include <limits.h>


/***********************
 * McIlroy's antiqsort *
 ***********************/
int frozen = 1;
int gas;
int *data;

int antiqsort_cmp(const void *ap, const void *bp) {
  int a, b;
  static int candidate = 0;

  a = *(int *)ap;
  b = *(int *)bp;

  if (data[a] == gas && data[b] == gas)
    if (a == candidate)
      data[a] = frozen++;
    else
      data[b] = frozen++;

  if (data[a] == gas) {
    candidate = a;
    return 1;
  }

  if (data[b] == gas) {
    candidate = b;
    return -1;
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

  /* generate a McIlroy killer adversary */
  worst = antiqsort(nmemb);
#ifdef DUMPDIST
  for (i=0; i<nmemb; i++)
    printf("%d\n", worst[i]);
  return 0;
#endif


  /* qsort() the killer adversary and count comparisons */
  printf("%d ", nmemb);
  qsort(worst, nmemb, sizeof(int), cmp_int);
  printf("%d ", count);

  /* qsort() a random distribution for comparison */
  for (i=0; i<nmemb; i++)
    worst[i] = rand();
  count = 0;
  qsort(worst, nmemb, sizeof(int), cmp_int);
  printf("%d\n", count);

  return 0;
}
