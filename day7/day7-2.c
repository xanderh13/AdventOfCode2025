#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long dfs(int beam_pos, long **grid, int lines, int size) {
  // fprintf(stderr, "%d\n", lines);
  if (lines == 0) // base case
    return 1;
  if (grid[0][beam_pos] > 0) // memoization
    return grid[0][beam_pos];

  long res = 0;
  if (grid[0][beam_pos] == -2) { // splitter
    if (beam_pos > 0)
      res += dfs(beam_pos - 1, grid + 1, lines - 1, size); // left
    if (beam_pos < size - 1)
      res += dfs(beam_pos + 1, grid + 1, lines - 1, size); // right
  } else // beam
    res += dfs(beam_pos, grid + 1, lines - 1, size);

  // fprintf(stderr, "%ld\n", res);
  return (grid[0][beam_pos] = res); // memoize
}

int main (int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "USAGE: ./day7 foo.txt\n");
    return 1;
  }

  FILE *f = fopen(argv[1], "r");
  int c, size = 0, lines = 1; // 1 for last
  while (fgetc(f) != '\n')
    ++size;
  while ((c = fgetc(f)) != EOF)
    if (c == '\n')
      ++lines;

  fseek(f, 0, SEEK_SET); // return to start
  int beam_start;
  // setup lines*size grid in a way that can be passed to dfs func
  long *grid[lines - 1];
  for (int i = 0; i < lines - 1; ++i) {
    grid[i] = malloc(size * sizeof(long));
    if (grid[i] == NULL) {
      fprintf(stderr, "main(): malloc failed\n");
      return 2;
    }
  }
  for (int i = 0; i < size; ++i)
    if (fgetc(f) == 'S')
      beam_start = i;
  // populate grid: beams and S = -1, splitters = -2
  for (int i = 0; i < lines - 1; ++i) {
    fgetc(f); // skip \n
    for (int j = 0; j < size; ++j)
      grid[i][j] = (fgetc(f) == '^') ? -2 : 0;
  }

  printf("%ld\n", dfs(beam_start, grid, lines - 1, size));

}
