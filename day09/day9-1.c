#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define triangle(A) ((A) * ((A) - 1) / 2)
#define max(A, B) ((A) > (B) ? (A) : (B))

struct point {
  unsigned x,y;
};

int main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "USAGE: ./day9 foo.txt\n");
    return 1;
  }

  FILE *f = fopen(argv[1], "r");
  int c, size = 0;
  while ((c = fgetc(f)) != EOF)
    if (c == '\n')
      ++size;
  fseek(f, 0, SEEK_SET); // return to start
  struct point points[size];
  for (int i = 0; i < size; ++i) {
    // points[i] = malloc(sizeof(struct point));
    // if (points[i] == NULL) {
    //   fprintf(stderr, "main(): malloc failed\n");
    //   return 2;
    // }
    fscanf(f, "%u,%u", &points[i].x, &points[i].y);
  }
  unsigned long long *areas =
    malloc(sizeof(unsigned long long) * triangle(size));
  if (areas == NULL) {
    fprintf(stderr, "main(): malloc failed\n");
    return 2;
  }
  for (int i = 0; i < size; ++i) {
    for (int j = i + 1; j < size; ++j) {
      unsigned xi = points[i].x, yi = points[i].y,
               xj = points[j].x, yj = points[j].y;
      unsigned dx = (xi > xj) ? (xi - xj) : (xj - xi);
      unsigned dy = (yi > yj) ? (yi - yj) : (yj - yi);
      ++dx; // include both ends
      ++dy;

      int index = i*size - triangle(i+1) + (j-i-1);

      // fprintf(stderr, "%d: (%d, %d)\n", index, i, j);

      areas[index] = (unsigned long long) dx * dy;
    }
  }
  unsigned long long area_max = 0;
  for (int i = 0; i < triangle(size); ++i)
    area_max = max(area_max, areas[i]);

  printf("%llu\n", area_max);
}
