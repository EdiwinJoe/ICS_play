/* 
 * CS:APP Data Lab 
 * 
 * 濠殿喗鎸搁幃鏇㈡晬濮橆厽缍嶉柛婊冩湰钘?   閻庢冻绠戣ぐ鍧楁晬?5300190071
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:

  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.
  4. Interprets integer expressions using the Data Lab 32-bit bit-vector
     model: results outside the signed range retain their low 32 bits.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
#include "bits.h"

/* The exact signed comparison "p < q" is used by three puzzles below.  It is
   expanded at each use site rather than defined as a function, because the lab
   forbids adding functions to this file.  The expression is

       ((((p ^ q) & p) | (~(p ^ q) & (p + ~q + 1))) >> 31) & 1

   Its meaning: when the signs of p and q differ, (p ^ q) has its top bit set
   and (p ^ q) & p is exactly the sign of p, which is the answer; when the signs
   agree, no overflow is possible in p - q, so the wraparound difference
   p + ~q + 1 is faithful and its sign bit is the answer.  The two cases are
   OR-ed together because exactly one of the masks is non-zero. */



// P1
/* 
 * signMask - return a mask with only the most significant bit set (0x80000000)
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 2
 *   Rating: 1
 */
int signMask(void) {
  /* 1 shifted left by 31 puts a single 1 into the sign position */
  return 1 << 31;
}

// P2
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1, bitXor(7, 7) = 0
 *   Legal ops: ~ &
 *   Max ops: 8
 *   Rating: 2
 */
int bitXor(int x, int y) {
  /* the two "different" cases: at least one of them is 0, and not both are 1 */
  return ~(~x & ~y) & ~(x & y);
}

// P3
/*
 * negativePart - return -x if x < 0, otherwise return 0
 *   Examples: negativePart(-10) = 10, negativePart(5) = 0
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 3
 */
int negativePart(int x){
  /* sign is all 1s exactly when x < 0; (x ^ -1) + 1 is -x */
  return (x >> 31) & ((x ^ ~0) + 1);
}

// P4
/*
 * copyByteWithin - copy byte src of x to byte dst, leaving all other bytes unchanged
 *   Bytes are numbered from 0 (least significant) to 3 (most significant).
 *   You can assume 0 <= src <= 3 and 0 <= dst <= 3.
 *   Example: copyByteWithin(0x11223344, 0, 2) = 0x11443344
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 */
int copyByteWithin(int x, int src, int dst) {
  /* bring the source byte down to the low 8 bits, build a 0xFF field at the
     destination, clear that field and OR the byte back in */
  int byte = (x >> (src << 3)) & 255;
  int field = 255 << (dst << 3);
  return (x & ~field) | (byte << (dst << 3));
}

// P5
/* 
 * logicalShift - shift x to the right by n bits, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 4
 */
int logicalShift(int x, int n) {
  /* >> is arithmetic, so the vacated high bits copy the sign; a mask that
     keeps only the low (32-n) bits removes them.  (1 << 31) >> 31 is all 1s and
     shifting it left by (31-n) puts 1s into bits [31-n, 31]. */
  int keep = ~((1 << 31) >> n << 1);
  return (x >> n) & keep;
}

// P6
/*
 * swapNibblePairs - swap the low and high 4 bits within each byte of x
 *   Examples: swapNibblePairs(0xAB) = 0xBA
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 18
 *   Rating: 4
 */
int swapNibblePairs(int x) {
  /* 0x0F0F0F0F is built from small constants: 0x0F | (0x0F << 8) gives the
     16-bit pattern and duplicating it at bit 16 gives the whole mask */
  int m = 0x0F | (0x0F << 8);
  int lowmask = m | (m << 16);
  int low = x & lowmask;
  int high = (x >> 4) & lowmask;
  return (low << 4) | high;
}

// P7
/*
 * secondLowestZeroBit - return a mask that marks the position of the second least significant 0 bit
 *   Examples: secondLowestZeroBit(0xFFFFFFFA) = 0x4, secondLowestZeroBit(0x7FFFFFFF) = 0
 *             secondLowestZeroBit(-1) = 0
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 4
 */
int secondLowestZeroBit(int x) {
  /* ~x marks every zero bit of x.  low = z & -z isolates the lowest set bit of
     z (the lowest zero bit of x); removing it from z with z & ~low leaves the
     rest, whose lowest set bit is precisely the second lowest zero bit of x.
     If x has fewer than two zero bits, z holds at most that one bit and the
     result is 0, which is the required answer for x == -1 and 0x7FFFFFFF. */
  int z = ~x;
  int low = z & (~z + 1);       /* z & -z : the lowest set bit of z */
  int rest = z ^ low;           /* z without its lowest bit */
  return rest & (~rest + 1);    /* rest & -rest : lowest bit of the rest */
}

// P8
/*
 * oddParity - return the odd parity bit of x, that is,
 *      when the number of 1s in the binary representation of x is even, then the return 1, otherwise return 0.
 *   Examples: oddParity(5) = 1, oddParity(7) = 0
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 56
 *   Rating: 5
 */
int oddParity(int x) {
  /* fold the word half by half with XOR; the low bit ends up holding the XOR
     of all 32 bits, i.e. 1 exactly for an odd number of 1s; negate it so that
     an even count (including x == 0) returns 1 */
  x = x ^ (x >> 16);
  x = x ^ (x >> 8);
  x = x ^ (x >> 4);
  x = x ^ (x >> 2);
  x = x ^ (x >> 1);
  return !(x & 1);
}

// P9
/* 
 * rotateRightBits - rotate x to right by n bits
 *   you can assume n >= 0
 *   Examples: rotateRightBits(0x12345678, 8) = 0x78123456
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 5
 */
int rotateRightBits(int x, int n) {
  /* reduce the shift to [0,31] so that the complementary shift is legal too,
     then glue the low s bits in front of a logical right shift by s */
  int s = n & 31;
  int lost = x << ((32 + ~s + 1) & 31);
  int keep = ~((1 << 31) >> s << 1);
  return lost | ((x >> s) & keep);
}

// P10
/*
 * roundEvenPow2 - round nonnegative x to the nearest multiple of 2^n.
 *   If x is exactly halfway between two multiples, choose the multiple whose
 *   quotient by 2^n is even.
 *   You can assume 0 <= x <= 0x3fffffff and 1 <= n <= 16.
 *   Examples: roundEvenPow2(10, 2) = 8, roundEvenPow2(14, 2) = 16
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 5
 */
int roundEvenPow2(int x, int n) {
  /* the result is the truncated part plus optionally one more block.  The
     remainder is classified by its top bit (rem >= half) and by the n-1 lower
     bits (rem == half); a tie goes to the even quotient. */
  int m1 = n + ~0;                        /* n - 1        */
  int mask = (1 << n) + ~0;               /* low n bits   */
  int lowmask = (1 << m1) + ~0;           /* low n-1 bits */
  int rem = x & mask;
  int cmp = (rem >> m1) & 1;              /* rem >= half  */
  int sticky = !!(rem & lowmask);         /* rem != half  */
  int odd = (x >> n) & 1;
  int up = cmp & (sticky | odd);
  return (x & ~mask) + (up << n);
}

// P11
/* 
 * midpointTowardFirst - return the exact mathematical midpoint (x+y)/2
 *   without overflow. If the exact midpoint lies halfway between two
 *   integers, choose the adjacent integer that is closer to the first
 *   argument x.
 *   Examples: midpointTowardFirst(4, 7) = 5,
 *             midpointTowardFirst(7, 4) = 6
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 32
 *   Rating: 5
 */
int midpointTowardFirst(int x, int y) {
  /* x + y = 2*(floor(x/2)) + 2*(floor(y/2)) + (x&1) + (y&1), and the arithmetic
     shift IS floor(v/2) for every int, so
         floor((x+y)/2) = (x>>1) + (y>>1) + ((x&1) + (y&1))/2 .
     The exact midpoint is a half integer exactly when x + y is odd; then it has
     to be rounded towards x, i.e. up when x > y.  "x > y" is spelled out with
     the comparison trick described above the first puzzle that needs it. */
  int d = ~( ( ((x ^ y) & x) | (~(x ^ y) & (x + ~y + 1)) ) >> 31 ) & 1;  /* x >= y */
  int gt = d & !!(x ^ y);                 /* x > y  */
  int carry = (x & 1) + (y & 1);
  return (x >> 1) + (y >> 1) + (carry >> 1) + (carry & gt);
}

// P12
/* 
 * isBetweenEitherOrder - return 1 when x lies in the inclusive interval whose
 *   endpoints are a and b. The endpoints may be given in either order.
 *   Example: isBetweenEitherOrder(5, 8, 3) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 48
 *   Rating: 7
 */
int isBetweenEitherOrder(int x, int a, int b) {
  /* Reduce the unordered interval [a,b] to the ordered one [lo,hi] first.  A
     direct signed comparison is available without any wider type: for values of
     the same sign the difference cannot overflow, and for values of opposite
     signs the sign of the operand already decides the order.  Both facts give
     "p < q" as  (same-sign ? q-p<0 : p<0)  with sign(v) = v >> 31:
         ((sp ^ sq) & sp) | (~(sp ^ sq) & ((p + ~q + 1) >> 31))
     which is all ones exactly when p < q.  Applying it to (a,b) picks lo and
     hi, and applying it to (x,lo) and (hi,x) bounds x from either side; the
     inclusive interval accepts equality through the two negations. */
  int sa = a >> 31, sb = b >> 31, sx = x >> 31;
  int ab = sa ^ sb;                      /* sign(a) != sign(b) */
  int alessb = (ab & sa) | (~ab & ((a + ~b + 1) >> 31));
  int lo = (alessb & a) | (~alessb & b);
  int hi = (alessb & b) | (~alessb & a);
  int sl = lo >> 31, sh = hi >> 31;
  int lx = sl ^ sx, hx = sh ^ sx;
  int xlesslo = (lx & sx) | (~lx & ((x + ~lo + 1) >> 31));
  int hilessx = (hx & sh) | (~hx & ((hi + ~x + 1) >> 31));
  return (!xlesslo) & (!hilessx);
}

// P13
/* 
 * mul5Sat - return x*5, and if x*5 overflow, change the result to 
 * INT_MAX(0x7fffffff) or INT_MIN(0x80000000) correspondingly
 *   Examples: mul5Sat(1) = 0x5, mul5Sat(0x40000000) = 0x7fffffff
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 30
 *   Rating: 7
 */
int mul5Sat(int x) {
  /* x*5 saturates exactly when |x| > INT_MAX/5 = 0x19999999.  The bound is
     assembled from pieces small enough not to overflow, |x| is computed without
     overflow, and the comparison is exact for every pair: for equal signs the
     difference cannot overflow, and for opposite signs the sign of the first
     operand settles it. */
  int bnd = (0x19 << 24) + (0x99 << 16) + (0x99 << 8) + 0x99;
  int sx = x >> 31;
  int ax = (x ^ sx) + ~sx + 1;                       /* |x| */
  int sb = bnd >> 31, sa = ax >> 31;
  int d = sb ^ sa;
  int over = ((d & sb) | (~d & ((bnd + ~ax + 1) >> 31))) & 1;
  int limit = (sx & (int)0x80000000u) | ((~sx) & 0x7FFFFFFF);
  return over ? limit : ((x << 2) + x);
}

// P14
/* 
 * classifyAdd3 - classify the exact mathematical sum x+y+z.
 *   Return 1 if the sum is greater than INT_MAX, -1 if it is less than
 *   INT_MIN, and 0 otherwise. You may not use a wider integer type.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 52
 *   Rating: 7
 */
int classifyAdd3(int x, int y, int z) {
  /* Split every addend at bit 15: x = (x>>15)*2^15 + (x&0x7FFF), the high part
     a signed value and the low part an unsigned 15-bit one.  Summing the
     columns separately keeps every intermediate small, and folding the low
     carry into the high column gives the exact sum as
         S = H * 2^15 + L,   L unsigned in [0, 2^15)
     with  H = (x>>15)+(y>>15)+(z>>15) + ((low sum) >> 15).
     Because |S| < 3*2^31 the value H determines the range completely: since
     0 <= L < 2^15, S is at most INT_MAX exactly when H <= 2^16-1, and at least
     INT_MIN exactly when H >= -2^16.  Adding 2^16 shifts that window onto
     [0, 2^17) and pushes every other H outside it, so the test is simply
     whether (H + 2^16) has any bit at or above bit 17 set. */
  int hs = (x >> 15) + (y >> 15) + (z >> 15);
  int ls = (x & 0x7FFF) + (y & 0x7FFF) + (z & 0x7FFF);
  int H = hs + (ls >> 15);
  unsigned q = (unsigned)(H + 0x10000);
  unsigned hi = q >> 17;
  unsigned inrange = (((hi | (0u + ~hi + 1)) >> 31) ^ 1u) & 1u;
  unsigned positive = ((unsigned)H >> 31) ^ 1u;
  return (int)(inrange) ? 0 : (int)(positive ? 1u : 0xFFFFFFFFu);
}

// P15
/*
 * floatScaleThreeHalves - Return bit-level equivalent of expression f*3/2 for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   Use round-to-nearest-even. Preserve the sign of both +0 and -0.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer / unsigned operations incl. ||, &&. also if, while
 *   Max ops: 60
 *   Rating: 7
 */
unsigned floatScaleThreeHalves(unsigned uf) {
  /* A finite float is M * 2^ex with M the 24-bit significand (the hidden bit
     included for normals) and ex = E - 150 for normals, -149 for subnormals.
     Then f*3/2 is exactly num * 2^ex with num = 3*M, a 24-, 25- or 26-bit value,
     so a single correctly-rounded encoding step is all that is left.
     num's exponent is P = ex + (bl - 1), and a float whose value has exponent P
     uses the exponent field P + 127.  Dropping bl - 24 bits puts the leading one
     on bit 23, where a float keeps its implicit bit; the dropped bits are the
     round and sticky bits for round-to-nearest-even.  A field below 1 means the
     result is subnormal, i.e. frac * 2^-149, which is num rounded against
     2^-(ex+149). */
  unsigned sign = uf & 0x80000000u;
  int E = (int)((uf >> 23) & 0xFFu);
  unsigned M = (uf & 0x7FFFFFu) | (E ? 0x800000u : 0u);
  unsigned num, m, rem, half;
  int ex, field, sh, bl;

  if (E == 0xFF) return uf;              /* infinity and NaN are unchanged */
  if (E == 0 && M == 0u) return uf;      /* +0 and -0 keep their sign      */

  num = M * 3u;
  ex = (E ? E - 150 : -149);
  bl = 24 + ((num >> 24) != 0u) + ((num >> 25) != 0u);
  field = bl + ex + 125;
  sh = field - 150 - ex + 1;             /* bits to drop: 0, 1 or 2       */
  m = (sh >= 0) ? (num >> sh) : (num << (-sh));

  if (field >= 1) {                      /* normal result */
    rem = num & ((1u << sh) - 1u);
    half = 1u << (sh - 1);
    if (rem > half || (rem == half && (m & 1u))) m += 1u;
    if (m >= 0x1000000u) { m >>= 1; field += 1; }
    if (field >= 0xFF) return sign | 0x7F800000u;
    return sign | ((unsigned)field << 23) | (m & 0x7FFFFFu);
  }

  /* Subnormal result: value = frac * 2^-149, so frac is num with one bit
     rounded off (the layout is already the fraction, not a 1.xxx form). */
  sh = 1;
  m = num >> sh;
  rem = num & ((1u << sh) - 1u);
  half = 1u << (sh - 1);
  if (rem > half || (rem == half && (m & 1u))) m += 1u;
  if (m >= 0x800000u) return sign | 0x00800000u;   /* rounded up to normal */
  return sign | (m & 0x7FFFFFu);
}

// P16
/* 
 * floatRoundEven - round the floating-point value represented by uf to the
 *   nearest integer, with halfway cases rounded to the even integer. Return
 *   the bit-level representation of that integer as a single-precision float.
 *   If rounding produces zero, preserve the input sign; thus a negative
 *   value that rounds to zero returns -0. When uf is NaN or infinity,
 *   return uf unchanged.
 *   Legal ops: Any integer / unsigned operations incl. ||, &&. also if, while
 *   Max ops: 65
 *   Rating: 10
 */
unsigned floatRoundEven(unsigned uf) {
  /* Work out the integer the value rounds to first, then encode it.  A float
     with exponent field E has the value M * 2^(E-150) with M = 1.m * 2^23, so
     it has 150-E fractional bits and the integer part is M >> (150-E).  Above
     2^23 there is no fractional part, and below 0.5 the value rounds to a
     signed zero.  One round bit plus the sticky bits give nearest-even; the
     integer k is then normalized into [2^23, 2^24) and the exponent follows. */
  unsigned sign = uf & 0x80000000u;
  unsigned exp = (uf >> 23) & 0xFFu;
  unsigned frac = uf & 0x7FFFFFu;
  unsigned mant, shift, i, k, y, r, sticky;
  int B, s;

  if (exp == 0xFFu)
    return uf;                            /* NaN and infinity keep the input */
  if (exp < 126u)
    return sign;                          /* |value| < 0.5 rounds to signed 0 */
  if (exp > 150u)
    return uf;                            /* already an integer */

  mant = frac | 0x800000u;
  shift = 150u - exp;                     /* fractional bits, 0..24 */
  i = mant >> shift;                      /* the truncated integer part */
  if (shift == 0u) {
    r = 0u;
    sticky = 0u;
  } else if (shift == 24u) {
    r = (mant >> 23) & 1u;
    sticky = !! (mant & 0x7FFFFFu);
  } else {
    r = (mant >> (shift - 1u)) & 1u;
    sticky = !! (mant & ((1u << (shift - 1u)) - 1u));
  }
  k = i;
  if (r && (sticky || (k & 1u)))
    k += 1u;                              /* nearest, ties to even */

  if (k == 0u)
    return sign;                          /* a negative value rounding to 0 */

  /* k/2^s is the rounded integer expressed as y * 2^-(s+23), and k has the same
     exponent as the original value scaled by 2^shift, so the unbiased exponent
     of the result is exp - 150 + shift - s - 23.  Adding the bias 127 gives the
     field below. */
  y = k;
  s = 0;
  while ((y & 0x800000u) == 0u) {         /* normalize k into [2^23, 2^24) */
    y <<= 1;
    s += 1;
  }
  /* y equals k * 2^s and k * 2^(exp-150+shift) is the rounded integer, so y is
     worth 2^(exp-150+shift-s).  Writing y as (2^23 + frac) * 2^(field-150) gives
     field = exp + shift - s directly. */
  B = (int)exp + (int)shift - s;
  return sign | ((unsigned)B << 23) | (y & 0x7FFFFFu);
}

// P17
/*
 * float_i2f - Return bit-level equivalent of expression (float) x.
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of
 *   a single-precision floating point values.
 *   Legal ops: Any integer / unsigned operations incl. ||, &&. also if, while
 *   Max ops: 40
 *   Rating: 10
 */
unsigned float_i2f(int x) {
  unsigned sign = 0u, mag, e, mant, rest;

  if (x == 0)
    return 0u;
  if (x < 0) {
    sign = 0x80000000u;
    mag = 0u - (unsigned)x;        /* also correct for INT_MIN */
  } else {
    mag = (unsigned)x;
  }

  e = 31u;
  while ((mag & 0x80000000u) == 0u) {     /* position of the leading 1 */
    mag <<= 1;
    e -= 1u;
  }
  /* the leading 1 is at bit 31 now; a float keeps 23 fraction bits, so drop
     the low 8 bits with round-to-nearest-even */
  rest = mag & 0x000000FFu;
  mant = mag >> 8;
  if (rest > 0x80u || (rest == 0x80u && (mant & 1u)))
    mant += 1u;
  if (mant & 0x01000000u) {        /* the significand carried out */
    mant >>= 1;
    e += 1u;
  }
  return sign | ((e + 127u) << 23) | (mant & 0x007FFFFFu);
}



// P18
/*
 * bitCount - return count of number of 1's in the binary representation of x
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 10
 */
int bitCount(int x) {
  /* parallel counters.  Every mask is built by OR-ing a small constant with a
     copy of itself one byte higher, so all constants stay within 0..255. */
  int m1 = 0x55 | (0x55 << 8);
  int m2 = 0x33 | (0x33 << 8);
  int m4 = 0x0F | (0x0F << 8);
  m1 = m1 | (m1 << 16);                  /* 0x55555555 */
  m2 = m2 | (m2 << 16);                  /* 0x33333333 */
  m4 = m4 | (m4 << 16);                  /* 0x0F0F0F0F */

  x = (x & m1) + ((x >> 1) & m1);        /* bit pairs  */
  x = (x & m2) + ((x >> 2) & m2);        /* nibbles    */
  x = (x + (x >> 4)) & m4;               /* bytes      */
  x = x + (x >> 8);                      /* two bytes  */
  x = (x + (x >> 16)) & 0x3F;            /* total      */
  return x;
}

// P19
/*
 * bitReverse - Reverse bits in an 32-bit integer
 *   Examples: bitReverse(0x80000004) = 0x20000001
 *             bitReverse(0x7FFFFFFF) = 0xFFFFFFFE
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 34
 *   Rating: 10
 */
int bitReverse(int x)
{
  /* Five parallel swap stages: neighbouring bits, pairs, nibbles, bytes and the
     two halves.  A stage that exchanges fields s apart keeps the bits below the
     field boundary in m, moves them up by s, and moves the bits above down by s;
     masking both halves with the same m works because after the shift the bits
     that belong together sit in the low part of their field.
     The three repeating masks are produced from a single byte pattern: 0x01010101
     times 0x55, 0x33 and 0x0F gives 0x55555555, 0x33333333 and 0x0F0F0F0F.  The
     pattern itself is grown from the small constant 0x01 by OR-ing a copy of
     itself one byte higher, so every literal stays inside 0..255. */
  int a = 0x01 | (0x01 << 8);
  int m1, m2, m4, m8, m16;
  a = a | (a << 16);                     /* 0x01010101 */
  m1 = a * 0x55;                         /* 0x55555555 */
  m2 = a * 0x33;                         /* 0x33333333 */
  m4 = a * 0x0F;                         /* 0x0F0F0F0F */
  m8 = 0xFF | (0xFF << 16);              /* 0x00FF00FF */
  m16 = 0xFF | (0xFF << 8);              /* 0x0000FFFF */

  x = ((x & m1) << 1) | ((x >> 1) & m1);
  x = ((x & m2) << 2) | ((x >> 2) & m2);
  x = ((x & m4) << 4) | ((x >> 4) & m4);
  x = ((x & m8) << 8) | ((x >> 8) & m8);
  return (x << 16) | ((x >> 16) & m16);
}



