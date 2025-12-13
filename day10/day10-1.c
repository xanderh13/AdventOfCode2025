#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node {
  unsigned data, depth;
  struct node *next;
};

struct queue {
  struct node *start, *end;
};

void enqueue(struct queue *q, unsigned data, unsigned depth)
{
  struct node *n = malloc(sizeof(struct node));
  n->data = data;
  n->depth = depth;
  n->next = NULL;
  if (n == NULL) {
    fprintf(stderr, "enqueue(): malloc failed\n");
    exit(2);
  }
  if (q->start == NULL) {
    q->start = q->end = n;
  } else {
    q->end->next = n;
    q->end = n;
  }
}

unsigned dequeue(struct queue *q, unsigned *depth)
{
  if (q->start == NULL) {
    fprintf(stderr, "dequeue(): empty struct queue* received\n");
    return 0; // probably not a good return value since it's the solved state
  }
  struct node *tmp = q->start;
  unsigned res = tmp->data;
  *depth = tmp->depth;
  q->start = tmp->next;
  if (q->start == NULL) // last element dequeued
    q->end = NULL;
  free(tmp);
  return res;
}

void node_free(struct node *n)
{
  if (n != NULL) {
    node_free(n->next);
    free(n);
  }
}

void queue_free(struct queue *q)
{
  node_free(q->start);
  free(q);
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
    // read the lights as a binary number
    unsigned n_lights = 0, lights = 0;
    fgetc(f); // skip the [
    while ((c = fgetc(f)) != ']') {
      ++n_lights;
      if (c == '#')
        lights = (lights | 1) << 1;
      if (c == '.')
        lights <<= 1;
    }
    lights >>= 1;

    // fprintf(stderr, "lights: %u\n", lights);

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
        buts[i] |= 1 << (n_lights - (c - '0'));
        fgetc(f); // skip , or )
      }
      buts[i] >>= 1;

      // fprintf(stderr, "button %d: %u\n", i, buts[i]);

    }

    // create a visited list for every binary num possible with given digits
    char visited[1 << n_lights];
    memset(visited, 0, sizeof(visited));
    struct queue *q = calloc(1, sizeof(struct queue));
    if (q == NULL) {
      fprintf(stderr, "main(): calloc failed\n");
      return 2;
    }
    unsigned depth = 0;
    enqueue(q, lights, depth);
    while (!visited[0]) {
      unsigned curr = dequeue(q, &depth);

      fprintf(stderr, "bfs depth: %u\n", depth);

      visited[curr] = 1;
      for (int j = 0; j < n_buts; ++j) {
        unsigned adj = curr ^ buts[j];
        if (!visited[adj]) {
          enqueue(q, adj, depth + 1);
        }
      }
    }
    queue_free(q);
    sum += depth;
    while (fgetc(f) != '\n'); // go to next line
  }

  printf("%u\n", sum);
}
