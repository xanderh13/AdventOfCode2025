#include <stdio.h>
#include <string.h>

int main (int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "USAGE: ./day4 foo.txt\n");
    return 1;
  }

  FILE *f = fopen(argv[1], "r");
  // height = 1 since we skip the first line parsing the width
  int width = 0, height = 1;
  while (fgetc(f) != '\n')
    ++width;
  for (int c = fgetc(f); c != EOF; c = fgetc(f))
    if (c == '\n')
      ++height;
  fseek(f, 0, SEEK_SET); // return to beginning of file

  char grid[height+2][width+2]; // add a border of .'s for easier algo
  memset(grid, 0, (height+2) * (width+2)); // initialize to 0
  for (int i = 1; i <= height; ++i) {
    for (int j = 1; j <= width; ++j)
      grid[i][j] = fgetc(f) == '@'; // 1 for @, 0 for .
    fgetc(f); // skip the \n
  }

  // for (int i = 0; i < height + 2; ++i) {
  //   for (int j = 0; j < width + 2; ++j)
  //     fprintf(stderr, "%d", grid[i][j]);
  //   fprintf(stderr, "\n");
  // }

  unsigned count = 0;
  for (int i = 1; i <= height; ++i) {
    for (int j = 1; j <= width; ++j) {
      if (grid[i][j] == 0) // skip if no roll
        continue;
      if (grid[i-1][j-1] + grid[i-1][j] + grid[i-1][j+1] +
          grid[i  ][j-1] +                grid[i  ][j+1] +
          grid[i+1][j-1] + grid[i+1][j] + grid[i+1][j+1] < 4)
        ++count;
    }
  }

  printf("%u\n", count);
}
