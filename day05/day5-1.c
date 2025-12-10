#include <stdio.h>
#include <stdlib.h>

struct fresh_range {
  unsigned long long start;
  unsigned long long end;
  struct fresh_range *next;
};

struct fresh_range *fr_cons(unsigned long long start, unsigned long long end,
                            struct fresh_range *prev)
{
  struct fresh_range *res = malloc(sizeof(struct fresh_range));
  if (res == NULL) {
    fprintf(stderr, "fr_cons(): malloc failed\n");
    exit(1);
  }
  res->start = start;
  res->end = end;
  res->next = prev;
  return res;
}

void fr_free(struct fresh_range *fr)
{
  if (fr != NULL) {
    fr_free(fr->next);
    free(fr);
  }
}

int main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "USAGE: ./day5 foo.txt\n");
    return 1;
  }

  FILE *f = fopen(argv[1], "r");
  unsigned long long start, end;
  struct fresh_range *ranges = NULL, *curr;
  long pos;

  // repeat until hitting the single numbers
  while (fscanf(f, "%llu-%llu", &start, &end) == 2) {
    // fprintf(stderr, "%llu-%llu\n", start, end);
    ranges = fr_cons(start, end, ranges);
    pos = ftell(f);
  }

  // jump back to get all the single numbers
  fseek(f, pos, SEEK_SET);

  unsigned long long n;
  unsigned count = 0;
  while (fscanf(f, "%llu", &n) != EOF) {
    fprintf(stderr, "%llu\n", n);
    curr = ranges;
    while (curr != NULL) {
      // fprintf(stderr, "%llu %llu %llu\n", curr->start, n, curr->end);
      if (curr->start <= n && n <= curr->end) { // fresh
        ++count;
        break;
      }
      curr = curr->next;
    }
  }


  fr_free(ranges);
  printf("%u\n", count);
  return 0;
}
