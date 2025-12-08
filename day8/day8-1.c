#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// triangle number:
#define triangle(A) ((A) * ((A) - 1)) / 2

struct box {
  int i; // index
  unsigned x, y, z;
  unsigned len;
  struct box *next;
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

// void box_free(struct box *box)
// {
//   if (box != NULL) {
//     box_free(box->next);
//     free(box);
//   }
// }
//
// void boxes_free(struct box **boxes, int size)
// {
//   for (int i = 0; i < size; ++i)
//     box_free(boxes[i]);
//   free(boxes);
// }
//
// void dists_free(struct dist **dists, int size)
// {
//   for (int i = 0; i < size; ++i)
//     free(dists[i]);
//   free(dists);
// }

// void dl_free(struct dist_list *dl)
// {
//   if (dl != NULL) {
//     dl_free(dl->next);
//     free(dl);
//   }
// }

int main (int argc, char *argv[])
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
  struct box *boxes[size];
  for (int i = 0; i < size; ++i) {
    boxes[i] = malloc(sizeof(struct box));
    if (boxes[i] == NULL) {
      fprintf(stderr, "main(): malloc failed\n");
      return 2;
    }
    boxes[i]->i = i;
    fscanf(f, "%u,%u,%u\n", &(boxes[i]->x), &(boxes[i]->y), &(boxes[i]->z));
    boxes[i]->len = 1;
    boxes[i]->next = boxes[i]; // make cycles
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

  for (int i = 0; i < 1000; ++i) { // change the num of pairs accordingly
    // append b to a

    struct box *a = dists[i]->a;
    struct box *b = dists[i]->b;
    unsigned alen = a->len, blen = b->len, len = alen + blen;

    // fprintf(stderr, "%u: (%u,%u,%u)-(%u,%u,%u)\n", len, a->x, a->y, a->z,
    //                                                     b->x, b->y, b->z);

    // check if they're already in the same circuit
    for (int j = 0; j < alen; ++j) {
      if (a == b) {
        // fprintf(stderr, "skipped\n\n");
        goto same_circuit; // instead of continue inside nested for loop
      }
      a = a->next;
    }

    struct box *bcpy = b;
    // reach element just before b in the cycle:
    for (int j = 0; j < blen - 1; ++j) {
      boxes[b->i] = NULL;
      b->len = len;
      b = b->next;
    }
    boxes[b->i] = NULL;
    b->len = len;
    // update remaining lengths:
    for (int j = 0; j < alen; ++j) {
      boxes[a->i] = NULL;
      a->len = len;
      a = a->next;
    }
    boxes[a->i] = a;
    b->next = a->next;
    a->next = bcpy;

    // for (int j = 0; j < len; ++j) {
    //   fprintf(stderr, "(%u,%u,%u)-", a->x, a->y, a->z);
    //   a = a->next;
    // }
    // fprintf(stderr, "(%u,%u,%u)\n\n", a->x, a->y, a->z);

same_circuit: ;
  }
  unsigned largest[3] = {1, 1, 1};
  for (int i = 0; i < size; ++i) {
    if (boxes[i] == NULL)
      continue;
    unsigned len = boxes[i]->len;
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
  printf("%llu\n", (unsigned long long) largest[0] * largest[1] * largest[2]);

  // boxes_free(boxes, size);
  // dists_free(dists, triangle(size));
}
