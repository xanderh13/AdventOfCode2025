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
    if (dir == 'L')
      curr = mod(curr - num, DIAL_RAN);
    else
      curr = (curr + num) % DIAL_RAN;

    fprintf(stderr, "%d\n", curr);
    if (curr == 0)
      ++res;
  }
  printf("%u\n", res);
}
