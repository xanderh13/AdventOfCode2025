#include <stdio.h>

int main (int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "USAGE: ./day6 foo.txt\n");
    return 1;
  }

  FILE *f = fopen(argv[1], "r");
  int c, size = 1; // 1 to account for first operator
  while ((c = fgetc(f)) != '*' && c != '+'); // find first operator
  long op1_pos = ftell(f) - 1; // -1 to jump back
  while ((c = fgetc(f)) != EOF) // count operators
    if (c == '*' || c == '+')
      ++size;

  fseek(f, op1_pos, SEEK_SET); // return to start of operators
  char ops[size];
  int i = 0;
  // 1 for multiply, 0 for add
  while ((c = fgetc(f)) != EOF) {
    if (c == '*')
      ops[i++] = 1;
    if (c == '+')
      ops[i++] = 0;
  }

  fseek(f, 0, SEEK_SET); // return to start;
  unsigned curr;
  unsigned long long probs[size];
  for (i = 0; i < size; ++i)
    fscanf(f, "%llu", probs + i); // fill array with first row

  while (fscanf(f, "%u", &curr) > 0) { // until we run out of ints
    fprintf(stderr, "%u\n", curr);
    if (i >= size) // reset i
      i = 0;
    if (ops[i])
      probs[i] *= curr;
    else
      probs[i] += curr;
    ++i;
  }

  unsigned long long sum = 0;
  for (i = 0; i < size; ++i)
    sum += probs[i];

  printf("%llu\n", sum);
}
