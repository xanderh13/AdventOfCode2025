#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define triangle(A) ((A) * ((A) - 1) / 2)
#define min(A, B) ((A) < (B) ? (A) : (B))
#define max(A, B) ((A) > (B) ? (A) : (B))

struct point {
  unsigned x,y;
};

struct segment {
  struct point p, q;
};

// check if two segments intersect
// to avoid miscounting:
// intersection must pass through (not just touch)
// this means overlapped segments must count iff neither contains the other
int intersects(struct segment *A, struct segment *B)
{
  unsigned Aminx = min(A->p.x, A->q.x), Amaxx = max(A->p.x, A->q.x),
           Aminy = min(A->p.y, A->q.y), Amaxy = max(A->p.y, A->q.y),
           Bminx = min(B->p.x, B->q.x), Bmaxx = max(B->p.x, B->q.x),
           Bminy = min(B->p.y, B->q.y), Bmaxy = max(B->p.y, B->q.y);

  int A_vert = (Aminx == Amaxx), B_vert = (Bminx == Bmaxx);
  // perpendicular cases (skipping L or T intersections)
  if (A_vert && !B_vert)
    return Bminx < Aminx && Aminx < Bmaxx &&
           Aminy < Bminy && Bminy < Amaxy;
  if (!A_vert && B_vert)
    return Bminy < Aminy && Aminy < Bmaxy &&
           Aminx < Bminx && Bminx < Amaxx;

  // parallel cases (overlapping)
  if (A_vert && B_vert)
    return Aminx == Bminx &&
         ((Aminy < Bminy && Bminy < Amaxy && Amaxy < Bmaxy) ||
          (Bminy < Aminy && Aminy < Bmaxy && Bmaxy < Amaxy));
  if (!A_vert && !B_vert)
    return Aminy == Bminy &&
         ((Aminx < Bminx && Bminx < Amaxx && Amaxx < Bmaxx) ||
          (Bminx < Aminx && Aminx < Bmaxx && Bmaxx < Amaxx));

  return 0; // should never be reached
}

// check if p is on s
int on_segment(struct point p, struct segment *s)
{
  unsigned minx = min(s->p.x, s->q.x), maxx = max(s->p.x, s->q.x),
           miny = min(s->p.y, s->q.y), maxy = max(s->p.y, s->q.y);
  return (p.x == minx && p.x == maxx && miny <= p.y && p.y <= maxy) ||
         (p.y == miny && p.y == maxy && minx <= p.x && p.x <= maxx);
}

// check if a point is inside the polygon by counting segments crossed
int inside(struct point p, struct segment **segs, int size)
{
  int intersections = 0;
  struct segment ray = {{0, p.y}, p}; // extend left since 0 is a bound
  for (int i = 0; i < size; ++i) {
    if (on_segment(p, segs[i])) { // automatically inside if on an edge
      // fprintf(stderr, "(%u, %u) on segment (%u, %u)-(%u, %u)\n", p.x, p.y, segs[i]->p.x, segs[i]->p.y, segs[i]->q.x, segs[i]->q.y);
      return 1;
    }

    // cannot just use intersects because of behavior at vertices
    /* if p is not on a segment, ray will contain any horizontal segment,
       so they will effectively be ignored */
    // this effectively collapses segs to only vertical segments
    /* we want to count intersections in the middle of a segment and only
       the bottom vertex to handle parity:
       -- two upward segments is equivalent to never crossing so 2 & 1 == 0
       -- two downward segments are the same so 0 & 1 == 0
       -- one up/one down is equivalent to crossing so 1 & 1 == 1 */
    if (segs[i]->p.y != segs[i]->q.y) { // ignore vertical
      if (intersects(&ray, segs[i])) {
        // fprintf(stderr, "true intersection\n");
        ++intersections;
      } else {
        // fprintf(stderr, "p: (%u, %u), s: (%u, %u)-(%u, %u)\n", p.x, p.y, segs[i]->p.x, segs[i]->p.y, segs[i]->q.x, segs[i]->q.y);
        struct point bottom, top;
        if (segs[i]->p.y < segs[i]->q.y)
          bottom = segs[i]->p, top = segs[i]->q;
        else
          bottom = segs[i]->q, top = segs[i]->p;
        intersections += on_segment(bottom, &ray) && top.y > p.y;
      }
    }
  }
  // fprintf(stderr, "intersections: %d\n", intersections);
  return intersections & 1; // even => outside, odd => inside
}

// check if a segment intersects any segment in the segs array
int intersects_segs(struct segment *s, struct segment **segs, int size)
{
  for (int i = 0; i < size; ++i)
    if (intersects(s, segs[i]))
      return 1;
  return 0;
}

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
  for (int i = 0; i < size; ++i)
    fscanf(f, "%u,%u", &points[i].x, &points[i].y);

  struct segment *segs[size];
  for (int i = 0; i < size; ++i) {
    segs[i] = malloc(sizeof(struct segment));
    if (segs[i] == NULL) {
      fprintf(stderr, "main(): malloc failed\n");
      return 2;
    }
    segs[i]->p = points[i];
    segs[i]->q = points[(i+1) % size]; // % to wrap end to start
  }

  // checking to make sure the polygon doesn't intersect itself
  // unsigned intersections = 0;
  // for (int i = 0; i < size; ++i) {
  //   for (int j = i + 1; j < size; ++j) {
  //     intersections += intersects(segs[i], segs[j]);
  //   }
  // }
  // fprintf(stderr, "%u\n", intersections);

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
      int index = i*size - triangle(i+1) + (j-i-1);
      // fprintf(stderr, "%d: (%d, %d)\n", index, i, j);
      areas[index] = 0; // default if we rectangle invalid

      // check if the other corners of the rectangle are in the polygon
      struct point k = {xi, yj}, l = {xj, yi};
      // fprintf(stderr, "k: (%u, %u), l: (%u, %u)\n", xi, yj, xj, yi);
      if (inside(k, segs, size) &&
          inside(l, segs, size)) {
        // check if the edges intersect the bigger polygon
        struct segment s0 = {points[i], k}, s1 = {k, points[j]},
                       s2 = {points[j], l}, s3 = {l, points[i]};
        if (!intersects_segs(&s0, segs, size) &&
            !intersects_segs(&s1, segs, size) &&
            !intersects_segs(&s2, segs, size) &&
            !intersects_segs(&s3, segs, size)) {
          // check if center is inside to avoid miscounting edge cases
          unsigned dx = (xi > xj) ? (xi - xj) : (xj - xi);
          unsigned dy = (yi > yj) ? (yi - yj) : (yj - yi);
          ++dx, ++dy; // include both ends
          struct point c = {min(xi, xj) + dx/2, min(yi, yj) + dy/2};
          if (inside(c, segs, size))
            areas[index] = (unsigned long long) dx * dy;
        }
      }
      fprintf(stderr, "(%u, %u) * (%u, %u) = %llu\n", xi, yi, xj, yj, areas[index]);
    }
  }
  unsigned long long area_max = 0;
  for (int i = 0; i < triangle(size); ++i)
    area_max = max(area_max, areas[i]);

  printf("%llu\n", area_max);
}
