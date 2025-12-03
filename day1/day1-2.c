#include <stdio.h>

#define DIAL_RAN 100
// arithmetic mod (always nonnegative):
#define mod(A, B) ((((A) % (B)) + (B)) % (B))

int main (int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "USAGE: ./day1 foo.txt\n");
    return -1;
  }

  FILE *f = fopen(argv[1], "r");
  char dir;
  int  num, curr = 50; // dial starts at 50
  unsigned res = 0;

  while (fscanf(f, "%c%d\n", &dir, &num) != EOF) {
    if (dir == 'L') {
      if (curr != 0)
        curr -= DIAL_RAN; // converts dial to negatives from 0
      curr -= num;
      res -= curr / DIAL_RAN;
      fprintf(stderr, "%d ", curr);
      curr = mod(curr, DIAL_RAN);
      // curr %= DIAL_RAN;
    } else {
      curr += num;
      res += curr / DIAL_RAN;
      fprintf(stderr, "%d ", curr);
      curr %= DIAL_RAN;
    }
    fprintf(stderr, "%d %u\n", curr, res);
  }
  printf("%u\n", res);
}
