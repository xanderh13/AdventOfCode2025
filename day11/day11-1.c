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
  unsigned paths_to_out; // for memoization
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

unsigned dfs(struct device *d, char *visited, struct device **devices)
{
  if (!strcmp(d->label, "out"))
    return 1;

  unsigned res = 0;
  struct adj_list *curr = d->adj;
  while (curr != NULL) {
    int id = curr->id;
    if (visited[id])
      res += devices[id]->paths_to_out;
    else {
      visited[id] = 1;
      res += dfs(devices[id], visited, devices);
    }
    curr = curr->next;
  }
  d->paths_to_out = res; // memoize
  return res;
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
  devices[size - 1]->paths_to_out = 1;

  qsort(devices, size, sizeof(devices[0]), cmp); // sort so we can bsearch
  char *label_arr[size]; // label-to-id lookup table
  for (int i = 0; i < size; ++i) {
    label_arr[i] = devices[i]->label;
  }

  fseek(f, 0, SEEK_SET); // return to start
  int you_id; // may as well set up on this search
  // set up adjacencies
  for (int i = 0; i < size - 1; ++i) {
    // char parent[LABEL_SIZE];
    char *parent = malloc(LABEL_SIZE);
    read_label(f, parent);
    fgetc(f); // skip :
    // hashmap better than bsearch but too lazy to implement
    int index = (char **) bsearch(&parent, label_arr, size, sizeof(label_arr[0]),
        str_cmp) - label_arr; // offset from label_arr
    if (!strcmp(parent, "you"))
      you_id = index;
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
  visited[you_id] = 1;
  unsigned count = dfs(devices[you_id], visited, devices);

  printf("%u\n", count);
}
