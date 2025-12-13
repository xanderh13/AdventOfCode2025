#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

int jolts_is_zero(int n_jolts, unsigned *jolts)
{
  for (int i = 0; i < n_jolts; ++i)
    if (jolts[i] != 0)
      return 0;
  return 1;
}

int dls(int n_jolts, int n_buts, int limit, unsigned *jolts, unsigned *buts)
{
  // for (int i = 0; i < n_jolts; ++i)
  //   fprintf(stderr, "%u,", jolts[i]);
  // fprintf(stderr, "\n");

  if (jolts_is_zero(n_jolts, jolts))
    return 1;

  if (limit <= 0)
    return 0;

  unsigned *adj = malloc(sizeof(unsigned) * n_jolts);
  if (adj == NULL) {
    fprintf(stderr, "dls(): malloc failed\n");
    exit(2);
  }
  for (int i = 0; i < n_buts; ++i) {
    int valid_child = 1;
    memcpy(adj, jolts, sizeof(unsigned) * n_jolts); // adj = jolts
    for (int j = 0; j < n_jolts; ++j) {
      adj[j] -= (buts[i] & (1 << (n_jolts - j - 1))) >> (n_jolts - j - 1);
      // fprintf(stderr, "%u,", adj[j]);
      if (adj[j] > jolts[j]) { // prune overflow
        valid_child = 0;
        break;
      }
    }
    // fprintf(stderr, "\n");
    // fprintf(stderr, "remaining depth %d, child %d, valid %d\n", limit, i, valid_child);
    if (valid_child && dls(n_jolts, n_buts, limit - 1, adj, buts)) {
      free(adj);
      return 1;
    }
  }
  free(adj);
  return 0;
}

// return the depth to reach {0, ..., 0}
int iddfs(int n_jolts, int n_buts, unsigned *jolts, unsigned *buts)
{
  int i = 0;
  do {
    fprintf(stderr, "Searching depth %d\n", i);
    if (dls(n_jolts, n_buts, i, jolts, buts))
      return i;
  } while (++i); // assuming solution guaranteed so run until int max
  fprintf(stderr, "iddfs(): no target found!\n");
  return -1; // somehow never reached target
}

int main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "USAGE: ./day10 foo.txt\n");
    return 1;
  }

  FILE *f = fopen(argv[1], "r");
  int c, n_machines = 0;
  while ((c = fgetc(f)) != EOF)
    if (c == '\n')
      ++n_machines;

  fseek(f, 0, SEEK_SET); // return to start
  unsigned sum = 0;
  for (int i = 0; i < n_machines; ++i) {
    // get the size from the lights
    unsigned n_jolts = 0;
    fgetc(f); // skip the [
    while ((c = fgetc(f)) != ']')
      ++n_jolts;

    // read the buttons as binary masks
    fgetc(f); // skip SPACE
    long but_pos = ftell(f);
    unsigned n_buts = 0;
    while ((c = fgetc(f)) != '{')
      if (c == ')')
        ++n_buts;
    fseek(f, but_pos, SEEK_SET);
    unsigned buts[n_buts];
    memset(buts, 0, sizeof(buts));
    for (int i = 0; i < n_buts; ++i) {
      fgetc(f); // skip (
      while ((c = fgetc(f)) != ' ') {
        buts[i] |= 1 << (n_jolts - (c - '0'));
        fgetc(f); // skip , or )
      }
      buts[i] >>= 1;
    }

    // read the joltages
    fgetc(f); // skip {
    unsigned *jolts = malloc(sizeof(unsigned) * n_jolts);
    if (jolts == NULL) {
      fprintf(stderr, "main(): malloc failed\n");
      return 2;
    }
    for (int j = 0; j < n_jolts; ++j) {
      fscanf(f, "%u", jolts + j);
      fgetc(f); // skip , or }
    }

    sum += iddfs(n_jolts, n_buts, jolts, buts);
    free(jolts);
    while (fgetc(f) != '\n'); // go to next line
  }

  printf("%u\n", sum);
}
