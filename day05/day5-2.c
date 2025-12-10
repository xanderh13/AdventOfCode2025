#include <stdio.h>
#include <stdlib.h>

#define max(A, B) ((A) > (B) ? (A) : (B))

// array of pointers instead of linked list this time to use qsort
struct fresh_range {
  unsigned long long start;
  unsigned long long end;
};

struct fresh_range *fr_cons(unsigned long long start, unsigned long long end)
{
  struct fresh_range *res = malloc(sizeof(struct fresh_range));
  if (res == NULL) {
    fprintf(stderr, "fr_cons(): malloc failed\n");
    exit(1);
  }
  res->start = start;
  res->end = end;
  return res;
}

void fr_arr_free(struct fresh_range **fr_arr, int size)
{
  for (int i = 0; i < size; ++i)
    free(fr_arr[i]);
}

int cmp(const void *a, const void *b)
{
  struct fresh_range *x = *(struct fresh_range **)a;
  struct fresh_range *y = *(struct fresh_range **)b;

  return (x->start > y->start) - (x->start < y->start); // sort by start
}

int main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "USAGE: ./day5 foo.txt\n");
    return 1;
  }

  FILE *f = fopen(argv[1], "r");
  unsigned long long start, end, count = 0;
  struct fresh_range **ranges;
  int size = 0;

  // repeat until hitting the single numbers
  while (fscanf(f, "%llu-%llu", &start, &end) == 2) {
    // fprintf(stderr, "%llu-%llu\n", start, end);
    ++size; // size of the array
  }
  // redo the scan while building array
  fseek(f, 0, SEEK_SET); // return to start of file buffer
  ranges = malloc(sizeof(struct fresh_range *) * size); // init array
  if (ranges == NULL) {
    fprintf(stderr, "main(): malloc failed\n");
    exit(1);
  }
  for (int i = 0; i < size; ++i) {
    fscanf(f, "%llu-%llu", &start, &end);
    ranges[i] = fr_cons(start, end);
  }
  qsort(ranges, size, sizeof(ranges[0]), cmp); // sort the array by starts

  start = ranges[0]->start;
  for (int i = 0; i < size; ++i) { // iterate again and count
    start = max(start, ranges[i]->start);
    end = ranges[i]->end;
    fprintf(stderr, "%llu-%llu (%llu)\n", ranges[i]->start, end, start);
    if (start <= end) {
      count += (end - start + 1); // +1 to include both ends of range
      start = end + 1;            // go past end to avoid double counting
    }
  }

  fr_arr_free(ranges, size);
  printf("%llu\n", count);
  return 0;
}
