#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "USAGE: ./day6 foo.txt\n");
    return 1;
  }

  FILE *f = fopen(argv[1], "r");
  int c, size = 1, lines = 0; // 1 to account for first operator
  while ((c = fgetc(f)) != '*' && c != '+') // find first operator
    if (c == '\n')
      ++lines; // count lines
  long op1_pos = ftell(f) - 1; // -1 to jump back
  while ((c = fgetc(f)) != EOF) // count operators
    if (c == '*' || c == '+')
      ++size;

  fseek(f, op1_pos, SEEK_SET); // return to start of operators
  char ops[size];
  char digits[size];
  memset(digits, 0, size);
  int i = 0;
  // 1 for multiply, 0 for add
  while ((c = fgetc(f)) != EOF) {
    if (c == '*') {
      ops[i++] = 1;
    } else if (c == '+') {
      ops[i++] = 0;
    } else
      digits[i-1] += 1;
  }

  fseek(f, 0, SEEK_SET); // return to start;
  unsigned *nums[size];
  for (i = 0; i < size; ++i) {
    nums[i] = calloc(digits[i], sizeof(unsigned));
    if (nums[i] == NULL) {
      fprintf(stderr, "main(): calloc failed\n");
      return 2;
    }
  }
  // populate our numbers
  for (int k = 0; k < lines; ++k) {
    for (i = 0; i < size; ++i) {
      for (int j = 0; j < digits[i]; ++j) {
        c = fgetc(f);
        if (c != ' ') {
          c -= '0';
          if (nums[i][j] == 0)
            nums[i][j] = c;
          else
            nums[i][j] = (nums[i][j] * 10) + c;
        }
      }
      fgetc(f); // skip the space or \n
    }
  }
  // do the math
  unsigned long long sum = 0;
  for (i = 0; i < size; ++i) {
    unsigned long long prob = nums[i][0];
    for (int j = 1; j < digits[i]; ++j) {
      if (ops[i])
        prob *= nums[i][j];
      else
        prob += nums[i][j];
    }
    sum += prob;
  }
  printf("%llu\n", sum);
}
