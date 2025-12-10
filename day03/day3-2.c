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
  int c = fgetc(f);
  unsigned long long sum = 0;
  while (c != EOF) {
    int size = 0;
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

    int start_index = 0;
    unsigned long long shift = 100000000000; // starts with 11 zeroes
    for (int i = 0; i < 12; ++i) { // 12 searches
      int bat = search_greatest(bats+start_index, size-(11-i));
      sum += bats[start_index+bat] * shift;
      fprintf(stderr, "%d", bats[start_index+bat]);
      shift /= 10;
      start_index += bat + 1;
      size -= bat + 1;
    }
    fprintf(stderr, "\n");



    fgetc(f); // skip \n
    c = fgetc(f); // skip to next line (so we can check if it exists)
    // fprintf(stderr, "%d\n", c);
  }
  printf("%llu\n", sum);
}
