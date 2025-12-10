#include <stdio.h>
#include <string.h>

int main (int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "USAGE: ./day7 foo.txt\n");
    return 1;
  }

  FILE *f = fopen(argv[1], "r");
  int c, size = 0, lines = 2; // 2 for first and last
  while (fgetc(f) != '\n')
    ++size;
  while ((c = fgetc(f)) != EOF)
    if (c == '\n')
      ++lines;

  fseek(f, 0, SEEK_SET); // return to start
  unsigned count = 0;
  char prev[size], curr[size];
  // beams and S = 1, splitters = 2
  for (int i = 0; i < size; ++i)
    prev[i] = (fgetc(f) == 'S');
  for (int i = 1; i < lines; ++i) {
    fgetc(f); // skip the \n
    for (int j = 0; j < size; ++j) // find the splitters
      curr[j] = (fgetc(f) == '^') ? 2 : 0;
    for (int j = 0; j < size; ++j) {
      if (prev[j] == 1) {
        if (curr[j] == 2) { // if splitter
          ++count;
          // add split beams to curr
          if (j > 0)
            curr[j-1] = 1;
          if (j < size - 1)
            curr[j+1] = 1;
        } else // otherwise just extend beam
          curr[j] = 1;
      }
    }
    for (int j = 0; j < size; ++j) {
      switch (curr[j]) {
        case 0:
          fprintf(stderr, ".");
          break;
        case 1:
          fprintf(stderr, "|");
          break;
        case 2:
          fprintf(stderr, "^");
          break;
        default:
          fprintf(stderr, "\'%u\'", curr[j]);
      }
    }
    fprintf(stderr, "\n");
    memcpy(prev, curr, size); // set prev = curr
  }
  printf("%u\n", count);
}
