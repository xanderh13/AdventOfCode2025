#include <stdio.h>

// return the INDEX of the greatest single-digit in arr
// return -1 if nothing found
int search_greatest(char *arr, int size)
{
  char max = 0;
  int index = -1;
  for (int i = 0; i < size; ++i) {
    char c = arr[i];
    if (c == 9) // max single-digit
      return i;
    if (c > max) {
      max = c;
      index = i;
    }
  }
  return index;
}

int main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "USAGE: ./day3 foo.txt\n");
    return 1;
  }

  FILE *f = fopen(argv[1], "r");
  int c = fgetc(f), size = 0;
  unsigned sum = 0;
  while (c != EOF) {
    ungetc(c, f); // if this line exists (i.e. not EOF) go back to its start
    while ((c = fgetc(f)) != '\n' && c != EOF) // parse to end of bank
      ++size;
    // fprintf(stderr, "%d\n", size);

    // reparse line to populate array
    char bats[size];
    fseek(f, -size - 1, SEEK_CUR); // extra -1 for the \n or EOF
    for (int i = 0; i < size; ++i)
      bats[i] = ((char) (fgetc(f))) - '0'; // convert to actual number
    // fprintf(stderr, "%c...%c\n", bats[0], bats[size-1]);
    int bat1 = search_greatest(bats, size-1); // -1 since first digit not at end
    sum += bats[bat1] * 10;
    int bat2 = search_greatest(bats+bat1+1, size-bat1-1);
    sum += bats[bat2+bat1+1]; // must shift the same as the search window
    fprintf(stderr, "%d,%d => %d%d\n", bat1, bat2+bat1+1,
        bats[bat1], bats[bat2+bat1+1]);



    size = 0;
    fgetc(f); // skip \n
    c = fgetc(f); // skip to next line (so we can check if it exists)
    // fprintf(stderr, "%d\n", c);
  }
  printf("%u\n", sum);
}
