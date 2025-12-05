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

      unsigned curr_len = 0;
      unsigned long shift = 1;
      while (curr_len++ < n_dig / 2) {
        shift *= 10;
        if (n_dig % curr_len == 0) { // only check if divisible
          unsigned long pattern = start % shift;
          tmp = start / shift; // may as well reuse tmp
          while (tmp != 0) {
            if (pattern != tmp % shift) // pattern not matching
              break;
            tmp /= shift;
          }
          if (tmp == 0) { // all repeats found
            fprintf(stderr, "%llu,", start);
            sum += start;
            break; // prevent repeat counting/unnecessary loops
          }
        }
      }
      ++start;
    }
    fprintf(stderr, "\n");
  }
  printf("%llu\n", sum);
  return 0;
}
