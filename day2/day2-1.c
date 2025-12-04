#include <stdio.h>

int main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "USAGE: ./day2 foo.txt\n");
    return -1;
  }

  FILE *f = fopen(argv[1], "r");
  unsigned long long start, end, sum = 0;
  while (fscanf(f, "%llu-%llu", &start, &end) != EOF) {
    fgetc(f); // skip the comma
    while (start <= end) {
      // count digits:
      unsigned long long tmp = start;
      unsigned n_dig = 0;
      while (tmp != 0) {
        ++n_dig;
        tmp = start;
        for (unsigned i = 0; i < n_dig; ++i)
          tmp /= 10;
      }

      // create a shift to easily access first/second half of start
      unsigned long half_shift = 1;
      for (unsigned i = 0; i < n_dig; i += 2)
        half_shift *= 10;

      unsigned long half;
      if (n_dig % 2 == 1) {
        // if odd digits we can't have repeat pattern
        start = 1;
        for (unsigned i = 0; i < n_dig; ++i)
          start *= 10;
        start += start / half_shift; // set start to the next repeat
      } else if (start / half_shift == start % half_shift) {
        // repeat found
        fprintf(stderr, "%llu,", start);
        sum += start;
        start += half_shift + 1; // add 1 to first and second half
      } else if (start / half_shift <  start % half_shift) {
        // second half too high so increase first half (and second)
        // start / half_shift + 1 * half_shift + a copy of that
        half = (start / half_shift + 1);
        start = half * half_shift + half;
      } else {
        // second half too low so set it equal to first half
        half = start / half_shift;
        start = half * half_shift + half;
      }
    }
    fprintf(stderr, "\n");
  }
  printf("%llu\n", sum);
  return 0;
}
