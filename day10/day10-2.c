#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// arbitrary initial upper bound
#define MAX_UPPER_BOUND 300
#define min(A, B) ((A) <= (B) ? (A) : (B))
#define max(A, B) ((A) >= (B) ? (A) : (B))

// Elementary Row Operations:
// swap rows ra and rb
void rswap(int *ra, int *rb, unsigned n_buts)
{
  if (ra != rb) {
    for (int i = 0; i <= n_buts; ++i) {
      int tmp = ra[i];
      ra[i] = rb[i];
      rb[i] = tmp;
    }
  }
}
// scale row r up by c
void rscale(int *r, int c, unsigned n_buts)
{
  if (c == 0)
    fprintf(stderr, "WARNING: rscale() called with c = 0\n");
  if (c != 1) {
    for (int i = 0; i <= n_buts; ++i)
      r[i] *= c;
  }
}
// subtract row rb, scaled by c, from row ra
void rsub(int *ra, int *rb, int c, unsigned n_buts)
{
  if (c != 0) {
    for (int i = 0; i <= n_buts; ++i)
      ra[i] -= rb[i] * c;
  }
}

// find the gcd between two unsigned ints
// binary gcd algorithm, sign matches a
int gcd(int a_signed, int b_signed)
{
  // 0 divisible by every int
  if (a_signed == 0 || b_signed == 0)
    return a_signed + b_signed;

  int a = abs(a_signed), b = abs(b_signed);
  int sign = a_signed / a;

  // initial division of both by powers of 2:
  unsigned k, t;
  for (k = 0, t = a | b; (t & 1) == 0; ++k, t >>= 1); // >> logical for unsigned
  a = (unsigned) a >> k;
  b = (unsigned) b >> k;

  // repeat odd cases until 0
  // d < 0 => a < b, d > 0 => a > b
  int d;
  for (d = (a & 1) == 0 ? a / 2  // if a is the even
         : (b & 1) == 0 ? -b / 2 // if b is the even
         :              (a - b) / 2;
       d != 0; d = (a - b) / 2) {
    while ((d & 1) == 0) // halve until odd
      d /= 2; // >> implementation-defined for signed
    if (d > 0)
      a = d;
    else
      b = -d;
  }
  return (a << k) * sign;
}
// find the (non-negative) gcd of row r
int rgcd(int *r, unsigned n_buts)
{
  int res = gcd(r[0], r[1]);
  // gcd of gcd so far and the next int
  for (int i = 2; i <= n_buts; ++i)
    res = gcd(res, r[i]);
  return res;
}
// scale each row in the matrix down to minimum integers
void min_int_matrix(int **matrix, unsigned n_jolts, unsigned n_buts)
{
  for (int i = 0; i < n_jolts; ++i) {
    int gcd = rgcd(matrix[i], n_buts);
    for (int j = 0; j <= n_buts; ++j)
      matrix[i][j] /= gcd;
  }
}

// use elementary row ops to reduce to "almost Gauss-Jordan":
// instead of pivots of 1, ensure all entries are integers
void rref(int **matrix, unsigned n_jolts, unsigned n_buts)
{
  int pivots = 0, nonpivots = 0;
  for (int i = 0; i < n_buts; ++i) {
    int pivot_candidate = 0, pivot_id;
    for (int j = i - nonpivots; j < n_jolts; ++j) {
      if (abs(matrix[j][i]) == 1) { // 1's are preferable for elimination
        pivot_candidate = 1;
        pivot_id = j;
        break;
      } else if (abs(matrix[j][i]) > pivot_candidate) {
        pivot_candidate = abs(matrix[j][i]);
        pivot_id = j;
      }
    }
    if (!pivot_candidate)
      ++nonpivots;
    else {
      rswap(matrix[i - nonpivots], matrix[pivot_id], n_buts);
      /* scale the other rows up before subtracting instead of scaling down
         which can cause non-integers */
      for (int k = 0; k < n_jolts; ++k) {
        if (k != i - nonpivots && matrix[k][i] != 0) {
          rscale(matrix[k], matrix[i - nonpivots][i], n_buts);
          rsub(matrix[k], matrix[i - nonpivots],
              matrix[k][i] / matrix[i - nonpivots][i], n_buts);
        }
      }
      ++pivots;
    }
    if (pivots >= n_jolts)
      break;
  }
  min_int_matrix(matrix, n_jolts, n_buts);
}

// return bitmask of the buttons with a 1 if pivot, 0 if not; little-endian
unsigned get_pivots(int **matrix, unsigned n_jolts, unsigned n_buts)
{
  unsigned res = 0, nonpivots = 0;
  for (int i = 0; i < n_buts; ++i) {
    if (i >= n_jolts) // end of array => any other columns are nonpivots
      break;
    if (matrix[i][i + nonpivots] == 0) // no pivot
      while (matrix[i][i + ++nonpivots] == 0)
        if (i + nonpivots >= n_buts) // reached the augmented column
          goto zero_row; // break outer loop since no more pivots
    res |= 1 << (i + nonpivots);
  }
zero_row:
  return res;
}

// add all the entries of a column in the matrix
int csum(int **matrix, unsigned n_jolts, unsigned cid)
{
  int res = 0;
  for (int i = 0; i < n_jolts; ++i)
    res += matrix[i][cid];
  return res;
}

// compute the constraints on the free variable(s) assuming others = 0
// gets lower and upper bound(s)
// constrained by positive, integer button presses and mod by pivot = 0
unsigned get_free_constraints(int **matrix, unsigned n_jolts, unsigned n_buts,
             int **free_vars, int **lower_bounds, int **upper_bounds)
{
  unsigned pivot_mask = get_pivots(matrix, n_jolts, n_buts), n_free = 0;
  for (int i = 0; i < n_buts; ++i)
    if (!(pivot_mask & (1 << i)))
      ++n_free;
  if (n_free == 0)
    return 0;
  *free_vars = malloc(n_free * sizeof(int));
  *lower_bounds = calloc(n_free, sizeof(int));
  *upper_bounds = malloc(n_free * sizeof(int));
  if (*free_vars == NULL || *lower_bounds == NULL || *upper_bounds == NULL) {
    fprintf(stderr, "get_free_constraints(): calloc/malloc failed\n");
    exit(2);
  }
  for (int i = 0; i < n_free; ++i)
    (*upper_bounds)[i] = MAX_UPPER_BOUND;
  int index = 0;
  for (int i = 0; i < n_buts; ++i)
    if (!(pivot_mask & (1 << i)))
      (*free_vars)[index++] = i;

  // computer lower/upper bounds
  // multiple passes so useful bounds are used?
  // for (int passes = 0; passes < 2; ++passes) {
  for (int i = 0; i < n_jolts; ++i) {
    for (int j = 0; j < n_free; ++j) {
      int f = matrix[i][(*free_vars)[j]], c = matrix[i][n_buts];
      for (int k = 0; k < n_free; ++k) {
        if (k != j) {
          int f_other = matrix[i][(*free_vars)[k]];
          c -= (f_other > 0) ? ((*lower_bounds)[k] * f_other)
                             : ((*upper_bounds)[k] * f_other);
        }
      }
      if (f < 0 && c < 0) // => looking for lower bound
        (*lower_bounds)[j] = max((*lower_bounds)[j], c / f); // floor div
      if (f > 0 && c > 0) // => looking for upper bound
        (*upper_bounds)[j] = min((*upper_bounds)[j], (c+f-1) / f); // ceil div
      // if f and c have different signs we never reach 0 constraint
    }
  }
  // }
  return n_free;
}

// compute the num of button presses from given free_vars and coefficients
unsigned test_free_vars(int **matrix, unsigned n_jolts, unsigned n_buts,
                        unsigned n_free, int *free_vars, int *coefficients)
{
  unsigned res = 0, nonpivots = 0;
  for (int i = 0; i < n_jolts; ++i) {
    int sum = matrix[i][n_buts];
    for (int j = 0; j < n_free; ++j) {
      // if (i == free_vars[j])
      //   ++nonpivots;
      while (i + nonpivots == free_vars[j])
        ++nonpivots;
      sum -= coefficients[j] * matrix[i][free_vars[j]];
    }
    if (sum < 0 || sum % matrix[i][i + nonpivots]) // breaks constraints
      return -1; // max unsigned
    else
      res += sum / matrix[i][i + nonpivots];
  }
  for (int i = 0; i < n_free; ++i)
    res += coefficients[i]; // need to account for pressing the free buttons

  // for (int i = 0; i < n_free; ++i)
  //   fprintf(stderr, "%d ", coefficients[i]);
  // fprintf(stderr, "%u\n", res);
  return res;
}

// minimize sum via brute forcing free variable combos within bounds
// recursive to allow arbitrary num of free vars (exponential!)
unsigned min_free_vars(int **matrix, unsigned n_jolts, unsigned n_buts,
                       unsigned n_free, int depth, int *free_vars,
                       int *lower_bounds, int *upper_bounds, int *coefficients)
{
  if (depth >= n_free)
    return test_free_vars(matrix, n_jolts, n_buts, n_free,
                          free_vars, coefficients);
  unsigned res = -1; // max unsigned
  for (int i = lower_bounds[depth]; i <= upper_bounds[depth]; ++i) {
    coefficients[depth] = i;
    res = min(res, min_free_vars(matrix, n_jolts, n_buts, n_free, depth + 1,
                       free_vars, lower_bounds, upper_bounds, coefficients));
  }
  return res;
}

void display_matrix(int **matrix, unsigned n_jolts, unsigned n_buts)
{
  for (int i = 0; i < n_jolts; ++i) {
    for (int j = 0; j < n_buts; ++j)
      fprintf(stderr, "%2d ", matrix[i][j]);
    fprintf(stderr, "%4d\n", matrix[i][n_buts]);
  }
  // fprintf(stderr, "\n");
}

int main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "./day10 foo.txt\n");
    return 1;
  }

  FILE *f = fopen(argv[1], "r");
  int c, n_machines = 0;
  while ((c = fgetc(f)) != EOF)
    if (c == '\n')
      ++n_machines;
  fseek(f, 0, SEEK_SET); // return to start
  unsigned long long sum = 0;
  for (int i = 0; i < n_machines; ++i) {
    // still use the lights to determine number of jolts
    unsigned n_jolts = 0;
    fgetc(f); // skip the [
    while ((c = fgetc(f)) != ']')
      ++n_jolts;

    // read the buttons as binary masks
    // fgetc(f); // skip SPACE
    long but_pos = ftell(f);
    unsigned n_buts = 0;
    while ((c = fgetc(f)) != '{')
      if (c == ')')
        ++n_buts;
    fseek(f, but_pos, SEEK_SET);
    int **matrix = malloc(sizeof(int *) * n_jolts);
    if (matrix == NULL) {
      fprintf(stderr, "main(): malloc failed\n");
      return 2;
    }
    for (int j = 0; j < n_jolts; ++j) {
      matrix[j] = calloc(n_buts + 1, sizeof(int));
      if (matrix[j] == NULL) {
        fprintf(stderr, "main(): calloc failed\n");
        return 2;
      }
    }
    for (int j = 0; j < n_buts; ++j) {
      fgetc(f); // skip the SPACE
      while ((c = fgetc(f)) != ')') {
        int row;
        fscanf(f, "%d", &row);
        matrix[row][j] = 1; // since buttons toggle a position by 1
      }
    }

    // read the joltages into the matrix's augmented column
    fgetc(f); // skip the SPACE
    for (int j = 0; j < n_jolts; ++j) {
      fgetc(f); // skip the { or ,
      fscanf(f, "%d", &(matrix[j][n_buts]));
      // fprintf(stderr, "%d,", matrix[j][n_buts]);
    }
    // fprintf(stderr, "\n");
    fgetc(f); // skip the }
    fgetc(f); // skip the \n

    rref(matrix, n_jolts, n_buts);
    display_matrix(matrix, n_jolts, n_buts);
    int *free_vars, *lower_bounds, *upper_bounds;
    unsigned n_free = get_free_constraints(matrix, n_jolts, n_buts,
                         &free_vars, &lower_bounds, &upper_bounds);
    if (n_free == 0)
      sum += csum(matrix, n_jolts, n_buts);
    else {
      // fprintf(stderr, "%u free\n", n_free);
      for (int j = 0; j < n_free; ++j)
        fprintf(stderr, "%d <= x_%d <= %d\n", lower_bounds[j], free_vars[j], upper_bounds[j]);
      int *coefficients = malloc(sizeof(int) * (n_free));
      if (coefficients == NULL) {
        fprintf(stderr, "main(): malloc failed\n");
        return 2;
      }
      sum += min_free_vars(matrix, n_jolts, n_buts, n_free, 0,
                           free_vars, lower_bounds, upper_bounds, coefficients);
    }
    fprintf(stderr, "%llu\n", sum);
  }
  printf("%llu\n", sum);
}
