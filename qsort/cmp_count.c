#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#define ROUNDS_PER_SIZE 10
#define MIN_SIZE 4
#define MAX_SIZE (1<<20)

int cmp_count;

int cmp_int(const void *a, const void *b) {
  cmp_count++;
  return *(int *)a - *(int *)b;
}

int main() {
  int *data;
  int size, round, cmp_sum, cmp_min, cmp_max, i;

  data = malloc(MAX_SIZE * sizeof(int));

  for (size=MIN_SIZE; size <= MAX_SIZE; size <<= 1) {

    cmp_min = INT_MAX;
    cmp_max = INT_MIN;
    cmp_sum = 0;

    for (round=0; round < ROUNDS_PER_SIZE; round++) {

      for (i=0; i < size; i++)
	data[i] = rand();

      cmp_count = 0;

      qsort(data, size, sizeof(int), cmp_int);

      cmp_sum += cmp_count;
      cmp_min = cmp_count < cmp_min ? cmp_count : cmp_min;
      cmp_max = cmp_count > cmp_max ? cmp_count : cmp_max;
    }

    printf("%06d: avg %d, min %d, max %d\n", size, cmp_sum/ROUNDS_PER_SIZE,
	   cmp_min, cmp_max);
  }

  return 0;
}
