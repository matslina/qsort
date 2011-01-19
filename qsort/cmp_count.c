#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#define ROUNDS_PER_SIZE 2
#define MIN_SIZE (1<<16)
#define MAX_SIZE (1<<22)
#define STEPS 20

long long int cmp_count;
int cmp_int(const void *a, const void *b) {
  cmp_count++;
  return *(int *)a - *(int *)b;
}

void gen_data(int *buf, int size, char *dist) {
  int i;

  if (!strcmp(dist, "rand"))
    for (i=0; i < size; i++)
      buf[i] = rand();
  else if (!strcmp(dist, "inc"))
    for (i=0; i < size; i++)
      buf[i] = i;
  else if (!strcmp(dist, "dec"))
    for (i=0; i < size; i++)
      buf[i] = size - i;
}

int main(int argc, char **argv) {
  int *data;
  int size, round, i;
  char *dist;
  long long int cmp_sum;

  if (argc != 2) {
    fprintf(stderr,
	    "Usage: %s <distribution>\n\n"
	    "Distribution should be one of the following:\n"
	    "  rand - uniformly random\n"
	    "  inc  - strictly increasing\n"
	    "  dec  - strictly decreasing\n", argv[0]);
    exit(1);
  }
  dist = argv[1];

  data = malloc((MAX_SIZE + 1) * sizeof(int));

  for (size=MIN_SIZE; size <= MAX_SIZE; size += (MAX_SIZE - MIN_SIZE)/STEPS) {

    cmp_sum = 0;

    for (round=0; round < ROUNDS_PER_SIZE; round++) {
      gen_data(data, size, dist);

      cmp_count = 0;
      qsort(data, size, sizeof(int), cmp_int);

#ifdef CHECK_OUTPUT
      for (i=0; i < size - 1; i++) {
	if (data[i] > data[i+1]) {
	  fprintf(stderr, "broken implementation\n");
	  return 1;
	}
      }
#endif

      cmp_sum += cmp_count;
    }

    printf("%d %lld\n", size, cmp_sum/ROUNDS_PER_SIZE);
  }

  return 0;
}
