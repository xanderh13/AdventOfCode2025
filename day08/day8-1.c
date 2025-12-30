#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// triangle number:
#define triangle(A) ((A) * ((A) - 1)) / 2

struct box {
  int i; // index
  unsigned x, y, z;
};

struct dist {
  struct box *a, *b;
  unsigned long long dist;
};

int cmp(const void *a, const void *b)
{
  struct dist *x = *(struct dist **) a;
  struct dist *y = *(struct dist **) b;

  return (x->dist > y->dist) - (x->dist < y->dist); // sort asc by dist
}

// find func for union-find
int find(int n, int *reps)
{
  if (reps[n] == n)
    return n;
  return find(reps[n], reps);
}
// union func for union-find
// also updates lengths of parent
void unite(int a, int b, int *reps, unsigned *lens)
{
  int a_rep = find(a, reps), b_rep = find(b, reps);
  if (a_rep != b_rep) {
    reps[b_rep] = a_rep;
    lens[a_rep] += lens[b_rep];
    lens[b_rep] = 0; // only store a length for representatives
  }
}

int main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "USAGE: ./day8 foo.txt\n");
    return 1;
  }

  FILE *f = fopen(argv[1], "r");
  int c, size = 0;
  while ((c = fgetc(f)) != EOF)
    if (c == '\n')
      ++size;

  fseek(f, 0, SEEK_SET); // return to start
  int reps[size]; // representatives list for union-find
  unsigned lens[size]; // lengths for each representative
  struct box *boxes[size];
  for (int i = 0; i < size; ++i) {
    reps[i] = i; // initially all disjoint sets
    lens[i] = 1; // initially all length 1
    boxes[i] = malloc(sizeof(struct box));
    if (boxes[i] == NULL) {
      fprintf(stderr, "main(): malloc failed\n");
      return 2;
    }
    boxes[i]->i = i;
    fscanf(f, "%u,%u,%u\n", &(boxes[i]->x), &(boxes[i]->y), &(boxes[i]->z));
  }
  // for (int i = 0; i < size; ++i)
  //   fprintf(stderr, "%u,%u,%u\n", boxes[i]->x, boxes[i]->y, boxes[i]->z);

  // actually the squared dists so use unsigned long long
  // unsigned long long *dists[size];
  // for (int i = 0; i < size; ++i) {
  //   dists[i] = malloc(size * size * sizeof(unsigned long long));
  //   if (dists[i] == NULL) {
  //     fprintf(stderr, "main(): malloc failed\n");
  //     return 2;
  //   }
  // }
  // if (dists == NULL) {
  //   fprintf(stderr, "main(): malloc failed\n");
  //   return 2;
  // }
  // for (int i = 0; i < size; ++i)
  //   dists[i][i] = 0;

  // build sorted distances list
  struct dist **dists = malloc(sizeof(struct dist *) * triangle(size));
  if (dists == NULL) {
    fprintf(stderr, "main(): malloc failed\n");
    return 2;
  }
  for (int i = 0; i < size; ++i) {
    for (int j = i + 1; j < size; ++j) {
      unsigned xi = boxes[i]->x, yi = boxes[i]->y, zi = boxes[i]->z,
               xj = boxes[j]->x, yj = boxes[j]->y, zj = boxes[j]->z;
      // abs:
      unsigned long long x = (xi > xj) ? (xi - xj) : (xj - xi);
      unsigned long long y = (yi > yj) ? (yi - yj) : (yj - yi);
      unsigned long long z = (zi > zj) ? (zi - zj) : (zj - zi);
      // square:
      x *= x;
      y *= y;
      z *= z;
      // dists[i][j] = dists[j][i] = x + y + z;
      int index = i*size - triangle(i+1) + (j-i-1);
      dists[index] = malloc(sizeof(struct dist));
      if (dists[index] == NULL) {
        fprintf(stderr, "main(): malloc failed\n");
        return 2;
      }
      dists[index]->a = boxes[i];
      dists[index]->b = boxes[j];
      dists[index]->dist = x + y + z;

      // fprintf(stderr, "adj %d: (%d, %d)\n", index, i, j);
    }
  }

  qsort(dists, triangle(size), sizeof(dists[0]), cmp);

  for (int i = 0; i < 1000; ++i) // change the num of pairs accordingly
    unite(dists[i]->a->i, dists[i]->b->i, reps, lens);

  unsigned largest[3] = {1, 1, 1};
  for (int i = 0; i < size; ++i) {
    // fprintf(stderr, "%d %u\n", i, lens[i]);
    unsigned len = lens[i];
    if (len > 0) { // most will be part of a circuit => 0
      if (len > largest[0]) {
        largest[2] = largest[1];
        largest[1] = largest[0];
        largest[0] = len;
      } else if (len > largest[1]) {
        largest[2] = largest[1];
        largest[1] = len;
      } else if (len > largest[2]) {
        largest[2] = len;
      }
    }
  }
  printf("%llu\n", (unsigned long long) largest[0] * largest[1] * largest[2]);
}
