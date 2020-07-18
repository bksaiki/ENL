#ifndef _FLOATTYPES_H_
#define _FLOATTYPES_H_

#include <stdio.h> // debugging

// Splits a double x such that hi contains the upper 26 bits of significand and
// lo contains the lower 26 bits of significand.
void split_f64(double x, double *hi, double *lo);

// Adds two doubles together and stores the result at s and the approximated 
// error (in double precision) at e, assuming that |x| > |y|.
void quick_sum2_f64(double x, double y, double *s, double *e);

// Adds two doubles together and stores the result at s and the approximated 
// error (in double precision) at e.
void sum2_f64(double x, double y, double *s, double *e);

// Multiplies two doubles together and stores the result at p and the approximated
// error (in double precision) at e. Two different algorithms are used depending on
// the existence of FMA instructions when compiled.
void prod_f64(double x, double y, double *p, double *e);

// Computes the sum of (x + y) + z and stores the significand at s, and the two error
// components at e1 and e2.
void accum3_f64(double x, double y, double z, double *s, double *e1, double *e2);

// Computes the sum of x + y + z. The significand is stored at s while the two error
// components are stored at e1 and e2.
void sum3_f64_2(double x, double y, double z, double *s, double *e1, double *e2);

// Computes the sum of x + y + z. Like 'sum2_f64', the significand is stored at s while
// the approximated error is stored at e.
void sum3_f64(double x, double y, double z, double *s, double *e);

// Computes the sum of 6 doubles, grouping them in 2 groups of 3. Like 'sum2_f64', the significand
// is stored at s while the approximated error is stored at e.
void sum6by3_f64(double x0, double x1, double x2, double y0, double y1, double y2,
                 double *s, double* e1, double *e2);

// Computes the sum of 9 doubles, grouping them into pairs, with 'f' being the lone double.
// Like 'sum2_f64', the significand is stored at s while the approximated error is stored at e.
void sum9by2_f64(double a0, double a1, double b0, double b1, double c0, double c1, 
                 double d0, double d1, double f, double* s, double* e);

// Compares two doubles and returns positive if |x| < |y|, negative if |x| > |y|,
// or 0 if |x| == |y|.
int cmp_abs_f64_rev(const void *x, const void *y);

#endif