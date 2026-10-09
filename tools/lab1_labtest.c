/*
 * labtest.c -- a portable re-implementation of the Data Lab test harness.
 *
 * The Data Lab ships `btest` as a 32-bit Linux ELF program together with the
 * `dlc` rule checker; neither can be executed on Windows.  This harness
 * reproduces the parts of btest that decide the score:
 *
 *   * the reference solutions are the ones from the course's tests.c,
 *   * the test values follow btest's gen_vals() strategy exactly
 *     (neighbourhoods of the boundaries, of zero, random values, and for the
 *     floating point puzzles the documented focus values),
 *   * the same score line and the same final "Total points: n/110" summary.
 *
 * Build:  gcc -O2 -o labtest.exe labtest.c bits.c
 * Usage:  labtest.exe [-f function] [-1 x] [-2 y] [-3 z]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include "bits.h"

#define TEST_RANGE 1000000
#define MAX_TEST_VALS (13 * TEST_RANGE)

/* ------------------------------------------------------------------ */
/* reference solutions, copied from the course's tests.c               */
/* ------------------------------------------------------------------ */

static float u2f(unsigned u) { union { unsigned u; float f; } a; a.u = u; return a.f; }
static unsigned f2u(float f) { union { unsigned u; float f; } a; a.f = f; return a.u; }

int test_signMask(void) { return 0x80000000; }

int test_bitXor(int x, int y) { return x ^ y; }

int test_negativePart(int x) { return x < 0 ? -x : 0; }

int test_copyByteWithin(int x, int src, int dst) {
  unsigned ux = (unsigned)x;
  unsigned byte = (ux >> (src * 8)) & 0xffu;
  unsigned mask = 0xffu << (dst * 8);
  return (int)((ux & ~mask) | (byte << (dst * 8)));
}

int test_logicalShift(int x, int n) {
  unsigned u = (unsigned)x;
  unsigned shifted = u >> n;
  return (int)shifted;
}

int test_swapNibblePairs(int x) {
  int result = 0;
  for (int i = 0; i < 32; i += 8) {
    int byte = (x >> i) & 0xFF;
    int lowNibble = byte & 0x0F;
    int highNibble = (byte >> 4) & 0x0F;
    int swapped = (lowNibble << 4) | highNibble;
    result |= (swapped << i);
  }
  return result;
}

int test_secondLowestZeroBit(int x) {
  int count = 0;
  for (int i = 0; i < 32; i++) {
    if (!((x >> i) & 1)) {
      count++;
      if (count == 2)
        return 1 << i;
    }
  }
  return 0;
}

int test_oddParity(int x) {
  int answer = 0;
  for (int i = 1; i <= 32; ++i) {
    answer += x % 2;
    x >>= 1;
  }
  return answer % 2 ? 0 : 1;
}

int test_rotateRightBits(int x, int n) {
  unsigned int ux = (unsigned int)x;
  unsigned int shift = (unsigned int)n & 31;
  unsigned int rightPart = ux >> shift;
  unsigned int leftPart = ux << ((32 - shift) & 31);
  return (int)(rightPart | leftPart);
}

int test_roundEvenPow2(int x, int n) {
  int block = 1 << n;
  int quotient = x / block;
  int remainder = x % block;
  int half = block / 2;
  if (remainder > half || (remainder == half && (quotient & 1)))
    quotient++;
  return quotient * block;
}

int test_midpointTowardFirst(int x, int y) {
  long long sum = (long long)x + (long long)y;
  long long lower = sum >= 0 ? sum / 2 : (sum - 1) / 2;
  if (sum % 2 != 0 && x > y)
    lower++;
  return (int)lower;
}

int test_isBetweenEitherOrder(int x, int a, int b) {
  return (a <= x && x <= b) || (b <= x && x <= a);
}

int test_mul5Sat(int x) {
  if (x > 0x7fffffff / 5)
    return 0x7fffffff;
  else if (x < (int)0x80000000 / 5)
    return 0x80000000;
  else
    return x * 5;
}

int test_classifyAdd3(int x, int y, int z) {
  long long sum = (long long)x + (long long)y + (long long)z;
  if (sum > INT_MAX)
    return 1;
  if (sum < INT_MIN)
    return -1;
  return 0;
}

unsigned test_floatScaleThreeHalves(unsigned uf) {
  float f = u2f(uf);
  if (isnan(f))
    return uf;
  return f2u(f * 1.5f);
}

unsigned test_floatRoundEven(unsigned uf) {
  float f = u2f(uf);
  if (isnan(f) || isinf(f))
    return uf;
  return f2u(nearbyintf(f));
}

unsigned test_float_i2f(int x) {
  float f = (float)x;
  return f2u(f);
}

int test_bitCount(int x) {
  int result = 0;
  for (int i = 0; i < 32; i++)
    result += (x >> i) & 0x1;
  return result;
}

int test_bitReverse(int x) {
  int ans = 0;
  for (int i = 0; i < 32; i++) {
    ans <<= 1;
    ans |= (x & 1);
    x >>= 1;
  }
  return ans;
}

/* ------------------------------------------------------------------ */
/* puzzle table                                                        */
/* ------------------------------------------------------------------ */

typedef int (*f0_t)(void);
typedef int (*f1_t)(int);
typedef int (*f2_t)(int, int);
typedef int (*f3_t)(int, int, int);
typedef unsigned (*fu1_t)(unsigned);

typedef struct {
  const char *name;
  int args;             /* number of arguments                */
  int rating;           /* weight used for the score          */
  int isfloat;          /* 1 when the argument is a float bits pattern */
  int lo[3], hi[3];     /* value ranges of the arguments      */
  void *sol;
  void *ref;
} puzzle_t;

#define P_INT(name, n, rat, l0, h0, l1, h1, l2, h2) \
  { #name, n, rat, 0, {l0, l1, l2}, {h0, h1, h2}, (void *)name, (void *)test_##name }

static puzzle_t puzzles[] = {
  P_INT(signMask, 0, 1, 0, 0, 0, 0, 0, 0),
  P_INT(bitXor, 2, 2, INT_MIN, INT_MAX, INT_MIN, INT_MAX, 0, 0),
  P_INT(negativePart, 1, 3, INT_MIN, INT_MAX, 0, 0, 0, 0),
  P_INT(copyByteWithin, 3, 4, INT_MIN, INT_MAX, 0, 3, 0, 3),
  P_INT(logicalShift, 2, 4, INT_MIN, INT_MAX, 0, 31, 0, 0),
  P_INT(swapNibblePairs, 1, 4, INT_MIN, INT_MAX, 0, 0, 0, 0),
  P_INT(secondLowestZeroBit, 1, 4, INT_MIN, INT_MAX, 0, 0, 0, 0),
  P_INT(oddParity, 1, 5, INT_MIN, INT_MAX, 0, 0, 0, 0),
  P_INT(rotateRightBits, 2, 5, INT_MIN, INT_MAX, 0, INT_MAX, 0, 0),
  P_INT(roundEvenPow2, 2, 5, 0, 1073741823, 1, 16, 0, 0),
  P_INT(midpointTowardFirst, 2, 5, INT_MIN, INT_MAX, INT_MIN, INT_MAX, 0, 0),
  P_INT(isBetweenEitherOrder, 3, 7, INT_MIN, INT_MAX, INT_MIN, INT_MAX, INT_MIN, INT_MAX),
  P_INT(mul5Sat, 1, 7, INT_MIN, INT_MAX, 0, 0, 0, 0),
  P_INT(classifyAdd3, 3, 7, INT_MIN, INT_MAX, INT_MIN, INT_MAX, INT_MIN, INT_MAX),
  {"floatScaleThreeHalves", 1, 7, 1, {1, 0, 0}, {1, 0, 0}, (void *)floatScaleThreeHalves, (void *)test_floatScaleThreeHalves},
  {"floatRoundEven", 1, 10, 1, {1, 0, 0}, {1, 0, 0}, (void *)floatRoundEven, (void *)test_floatRoundEven},
  {"float_i2f", 1, 10, 0, {INT_MIN, 0, 0}, {INT_MAX, 0, 0}, (void *)float_i2f, (void *)test_float_i2f},
  P_INT(bitCount, 1, 10, INT_MIN, INT_MAX, 0, 0, 0, 0),
  P_INT(bitReverse, 1, 10, INT_MIN, INT_MAX, 0, 0, 0, 0),
  {NULL, 0, 0, 0, {0, 0, 0}, {0, 0, 0}, NULL, NULL}
};

/* ------------------------------------------------------------------ */
/* test value generation, following btest's gen_vals()                  */
/* ------------------------------------------------------------------ */

static int has_arg[3] = {0, 0, 0};
static unsigned argval[3] = {0, 0, 0};

static unsigned rng_state = 12345u;
static unsigned rng_next(void) {
  rng_state = rng_state * 1103515245u + 12345u;
  return (rng_state >> 16) & 0x7FFFu;
}
static int random_val(int min, int max) {
  double weight = rng_next() / (double)0x7FFF;
  return (int)(min * (1 - weight) + max * weight);
}

static int gen_vals(unsigned test_vals[], int min, int max, int test_range, int arg)
{
  int i, test_count = 0;

  if (has_arg[arg]) {
    test_vals[0] = argval[arg];
    return 1;
  }

  if (min == 1 && max == 1) {          /* floating point bits pattern */
    unsigned smallest_norm = 0x00800000;
    unsigned one = 0x3f800000;
    unsigned largest_norm = 0x7f000000;
    unsigned inf = 0x7f800000;
    unsigned nan = 0x7fc00000;
    unsigned sign = 0x80000000;
    unsigned focus[] = {
      0x003fffff, 0x00555556, 0x00800000,
      0x3e800000, 0x3f000000, 0x3f800000, 0x3faaaaab,
      0x3fc00000,
      0x40200000, 0x40600000, 0x4affffff, 0x4b000000,
      0x7f2aaaab, 0x7f7fffff
    };
    int focus_count = (int)(sizeof(focus) / sizeof(focus[0]));
    int j, delta;

    if (test_range > (1 << 23))
      test_range = 1 << 23;

    for (i = 0; i < test_range; i++) {
      test_vals[test_count++] = i;
      test_vals[test_count++] = sign | i;
      test_vals[test_count++] = smallest_norm + i;
      test_vals[test_count++] = smallest_norm - i;
      test_vals[test_count++] = sign | (smallest_norm + i);
      test_vals[test_count++] = sign | (smallest_norm - i);
      test_vals[test_count++] = one + i;
      test_vals[test_count++] = one - i;
      test_vals[test_count++] = sign | (one + i);
      test_vals[test_count++] = sign | (one - i);
      test_vals[test_count++] = largest_norm - i;
      test_vals[test_count++] = sign | (largest_norm - i);
    }
    test_vals[test_count++] = inf;
    test_vals[test_count++] = sign | inf;
    test_vals[test_count++] = nan;
    test_vals[test_count++] = sign | nan;
    for (j = 0; j < focus_count; j++) {
      for (delta = -2; delta <= 2; delta++) {
        unsigned value = focus[j] + delta;
        test_vals[test_count++] = value;
        test_vals[test_count++] = sign | value;
      }
    }
    return test_count;
  }

  if ((long long)max - (long long)MAX_TEST_VALS <= min) {
    for (i = min; i <= max; i++)
      test_vals[test_count++] = (unsigned)i;
    return test_count;
  }

  for (i = 0; i < test_range; i++) {
    test_vals[test_count++] = (unsigned)(min + i);
    test_vals[test_count++] = (unsigned)(max - i);
    if (i >= min && i <= max)
      test_vals[test_count++] = (unsigned)i;
    if (-i >= min && -i <= max)
      test_vals[test_count++] = (unsigned)(-i);
    test_vals[test_count++] = (unsigned)random_val(min, max);
  }
  return test_count;
}

/* ------------------------------------------------------------------ */
/* running the tests                                                   */
/* ------------------------------------------------------------------ */

static const char *g_name = "";
static int g_errors = 0;
static int quiet = 0;

static void report(const char *fmt, ...)
{
  if (!quiet)
    printf("%s", fmt);
}

static int test_function(puzzle_t *t)
{
  static unsigned vals[3][MAX_TEST_VALS];
  int counts[3];
  int range[3] = {0, 0, 0};
  int i, a1, a2, a3;
  int errors = 0;

  if (t->args == 1) range[0] = TEST_RANGE;
  else if (t->args == 2) { range[0] = (int)pow((double)TEST_RANGE, 0.5); range[1] = range[0]; }
  else if (t->args == 3) {
    range[0] = (int)pow((double)TEST_RANGE, 1.0 / 3.0);
    range[1] = range[0]; range[2] = range[0];
  }
  for (i = 0; i < 3; i++) if (range[i] < 1) range[i] = 1;

  for (i = 0; i < t->args; i++)
    counts[i] = gen_vals(vals[i], t->lo[i], t->hi[i], range[i], i);

  if (t->args == 0) {
    int r = ((f0_t)t->sol)();
    int rt = ((f0_t)t->ref)();
    if (r != rt) {
      quiet || printf("ERROR: Test %s() failed...\n...Gives %d[0x%x]. Should be %d[0x%x]\n",
                      t->name, r, r, rt, rt);
      errors++;
    }
    return errors;
  }

  for (a1 = 0; a1 < counts[0]; a1++) {
    if (t->args == 1) {
      int x = (int)vals[0][a1];
      int r, rt;
      if (t->isfloat) {
        unsigned u = vals[0][a1];
        r = (int)((fu1_t)t->sol)(u);
        rt = (int)((fu1_t)t->ref)(u);
      } else {
        r = ((f1_t)t->sol)(x);
        rt = ((f1_t)t->ref)(x);
      }
      if (r != rt) {
        quiet || printf("ERROR: Test %s(%d[0x%x]) failed...\n...Gives %d[0x%x]. Should be %d[0x%x]\n",
                        t->name, x, x, r, r, rt, rt);
        return errors + 1;
      }
    } else {
      for (a2 = 0; a2 < counts[1]; a2++) {
        if (t->args == 2) {
          int x = (int)vals[0][a1], y = (int)vals[1][a2];
          int r = ((f2_t)t->sol)(x, y);
          int rt = ((f2_t)t->ref)(x, y);
          if (r != rt) {
            quiet || printf("ERROR: Test %s(%d[0x%x],%d[0x%x]) failed...\n...Gives %d[0x%x]. Should be %d[0x%x]\n",
                            t->name, x, x, y, y, r, r, rt, rt);
            return errors + 1;
          }
        } else {
          for (a3 = 0; a3 < counts[2]; a3++) {
            int x = (int)vals[0][a1], y = (int)vals[1][a2], z = (int)vals[2][a3];
            int r = ((f3_t)t->sol)(x, y, z);
            int rt = ((f3_t)t->ref)(x, y, z);
            if (r != rt) {
              quiet || printf("ERROR: Test %s(%d[0x%x],%d[0x%x],%d[0x%x]) failed...\n...Gives %d[0x%x]. Should be %d[0x%x]\n",
                              t->name, x, x, y, y, z, z, r, r, rt, rt);
              return errors + 1;
            }
          }
        }
      }
    }
  }
  return errors;
}

int main(int argc, char **argv)
{
  const char *only = NULL;
  int i;
  int p;
  double points = 0.0, max_points = 0.0;
  int total_errors = 0;

  for (i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-f") && i + 1 < argc) only = argv[++i];
    else if (!strcmp(argv[i], "-1") && i + 1 < argc) { has_arg[0] = 1; argval[0] = (unsigned)strtoul(argv[++i], NULL, 0); }
    else if (!strcmp(argv[i], "-2") && i + 1 < argc) { has_arg[1] = 1; argval[1] = (unsigned)strtoul(argv[++i], NULL, 0); }
    else if (!strcmp(argv[i], "-3") && i + 1 < argc) { has_arg[2] = 1; argval[2] = (unsigned)strtoul(argv[++i], NULL, 0); }
    else if (!strcmp(argv[i], "-q")) quiet = 1;
  }

  printf("Score\tRating\tErrors\tFunction\n");
  for (p = 0; puzzles[p].name; p++) {
    int terrors;
    double tpoints;
    if (only && strcmp(puzzles[p].name, only)) continue;
    terrors = test_function(&puzzles[p]);
    total_errors += terrors;
    tpoints = terrors == 0 ? puzzles[p].rating : 0;
    points += tpoints;
    max_points += puzzles[p].rating;
    if (terrors < 1)
      printf(" %.0f\t%d\t%d\t%s\n", tpoints, puzzles[p].rating, terrors, puzzles[p].name);
  }
  printf("Total points: %.0f/%.0f\n", points, max_points);
  return total_errors ? 1 : 0;
}
