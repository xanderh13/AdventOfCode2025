#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "USAGE: ./day12 foo.txt\n");
    return 1;
  }

  FILE *f = fopen(argv[1], "r");
  // hard-coded areas, areas[3] = 8 because no shape can fit into its center
  unsigned areas[6] = {6, 5, 7, 8, 7, 7};
  long regions_start_pos;
  int c, regions = 0;
  while ((c = fgetc(f)) != 'x') // x is unique to regions
    if (c == '\n')
      regions_start_pos = ftell(f);
  while ((c = fgetc(f)) != EOF) // count regions
    if (c == '\n')
      ++regions;
  fseek(f, regions_start_pos, SEEK_SET); // return to start of regions
  unsigned definite_fit = 0, possible_fit = 0;
  for (int i = 0; i < regions; ++i) {
    unsigned width, length, area;
    fscanf(f, "%ux%u", &width, &length);
    area = width * length;
    fgetc(f); // skip the :
    unsigned quantities[6];
    unsigned max_area = 0, min_area = 0;
    for (int j = 0; j < 6; ++j) {
      fgetc(f); // skip the SPACE
      fscanf(f, "%u", quantities + j);
      max_area += quantities[j] * 9;
      min_area += quantities[j] * areas[j];
    }
    if (max_area <= area) {
      ++definite_fit;
      continue;
    } else if (min_area <= area) // add the packing check here
      ++possible_fit;
  }
  printf("%u + %u = %u\n", definite_fit, possible_fit,
      definite_fit + possible_fit);
}
