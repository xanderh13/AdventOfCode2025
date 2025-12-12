#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LABEL_SIZE 4 // all labels 3 chars + 1 '\0'

struct adj_list {
  int id;
  struct adj_list *next;
};

struct device {
  // char label[LABEL_SIZE];
  char *label;
  struct adj_list *adj;
  // 4 classifications of paths (fft,dac)
  // 00 - neither fft nor dac
  // 01 - only dac
  // 10 - only fft
  // 11 - both fft nor dac
  unsigned long long paths_to_out[4]; // for memoization
};

struct adj_list *adj_cons(int id, struct adj_list *curr)
{
  struct adj_list *res = malloc(sizeof(struct adj_list));
  if (res == NULL) {
    fprintf(stderr, "adj_cons(): malloc failed\n");
    exit(2);
  }
  res->id = id;
  res->next = curr;
  return res;
}

int cmp(const void *a, const void *b)
{
  const struct device *const x = *(const struct device *const *)a;
  const struct device *const y = *(const struct device *const *)b;
  return strcmp(x->label, y->label);
}

int str_cmp(const void *a, const void *b)
{
  const char *const x = *(const char *const *)a;
  const char *const y = *(const char *const *)b;
  return strcmp(x, y);
}

void read_label(FILE *f, char *label)
{
  for (int i = 0; i < LABEL_SIZE - 1; ++i)
    label[i] = fgetc(f);
  label[LABEL_SIZE - 1] = '\0';
}

// just populate the memoizations without returning
void dfs(struct device *d, char *visited, struct device **devices)
{
  char *label = d->label;
  if (!strcmp(label, "out")) {
    d->paths_to_out[0] = 1; // 00
    return;
  }
  int dac = !strcmp(label, "dac");
  int fft = !strcmp(label, "fft");

  struct adj_list *curr = d->adj;
  while (curr != NULL) {
    int id = curr->id;
    if (!visited[id]) {
      visited[id] = 1;
      dfs(devices[id], visited, devices);
    }

    unsigned long long *paths = devices[id]->paths_to_out;
    if (dac) {
      // convert 00 to 01
      d->paths_to_out[1] += paths[0] +
                           paths[1];
      // convert 10 to 11
      d->paths_to_out[3] += paths[3] +
                           paths[2];
    } else if (fft) {
      // convert 00 to 10
      d->paths_to_out[2] += paths[2] +
                           paths[0];
      // convert 01 to 11
      d->paths_to_out[3] += paths[3] +
                           paths[1];
    } else
      for (int i = 0; i < 4; ++i)
        d->paths_to_out[i] += paths[i];
    curr = curr->next;
  }
  fprintf(stderr, "%s: [%llu, %llu, %llu, %llu]\n", label, d->paths_to_out[0],
      d->paths_to_out[1], d->paths_to_out[2], d->paths_to_out[3]);
}

int main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "USAGE: ./day11 foo.txt\n");
    return 1;
  }

  FILE *f = fopen(argv[1], "r");
  int c, size = 1; // start at 1 to capture "out"
  while ((c = fgetc(f)) != EOF)
    if (c == '\n')
      ++size;
  fseek(f, 0, SEEK_SET); // return to start

  // create array of struct device * first
  struct device **devices = malloc(sizeof(struct device) * size);
  if (devices == NULL) {
    fprintf(stderr, "main(): malloc failed\n");
    return 2;
  }
  for (int i = 0; i < size - 1; ++i) {
    struct device *device = calloc(1, sizeof(struct device));
    char *label = malloc(LABEL_SIZE);
    if (device == NULL || label == NULL) {
      fprintf(stderr, "main(): calloc failed\n");
      return 2;
    }
    read_label(f, label);
    device->label = label;
    devices[i] = device;

    while (fgetc(f) != '\n'); // skip to next line
  }
  devices[size - 1] = calloc(1, sizeof(struct device));
  devices[size - 1]->label = "out";

  qsort(devices, size, sizeof(devices[0]), cmp); // sort so we can bsearch
  char *label_arr[size]; // label-to-id lookup table
  for (int i = 0; i < size; ++i) {
    label_arr[i] = devices[i]->label;
  }

  fseek(f, 0, SEEK_SET); // return to start
  int svr_id; // may as well set up on this search
  // set up adjacencies
  for (int i = 0; i < size - 1; ++i) {
    // char parent[LABEL_SIZE];
    char *parent = malloc(LABEL_SIZE);
    read_label(f, parent);
    fgetc(f); // skip :
    // hashmap better than bsearch but too lazy to implement
    int index = (char **) bsearch(&parent, label_arr, size, sizeof(label_arr[0]),
        str_cmp) - label_arr; // offset from label_arr
    if (!strcmp(parent, "svr"))
      svr_id = index;
    free(parent);
    while (fgetc(f) != '\n') { // skips the SPACE or gets a '\n'
      // char child[LABEL_SIZE];
      char *child = malloc(LABEL_SIZE);
      read_label(f, child);
      devices[index]->adj = adj_cons(
          (char **) bsearch(&child, label_arr, size, sizeof(label_arr[0]),
            str_cmp) - label_arr, // offset from label_arr
          devices[index]->adj);
      free(child);
    }
  }

  char visited[size];
  memset(visited, 0, size);
  visited[svr_id] = 1;
  dfs(devices[svr_id], visited, devices);

  printf("%llu\n", devices[svr_id]->paths_to_out[3]);
}
