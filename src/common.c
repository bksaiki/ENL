#include <math.h>
#include "../include/common.h"

void split_f64(double x, double *hi, double *lo)
{
    double t = 134217729.0  *x;
    *hi = t - (t - x);
    *lo = x - *hi;
}

void quick_sum2_f64(double x, double y, double *s, double *e)
{   
    *s = x + y;
    *e = y - (*s - x);
}

void sum2_f64(double x, double y, double *s, double *e)
{
    double t;
    *s = x + y;
    t = *s - x;
    *e = (x - (*s - t)) + (y - t);
}

void prod_f64(double x, double y, double *p, double *e)
{
#if !defined(__FMA__) && defined(__AVX2__)
    *p = x * y;
    *e = fma(x, y, -(*p));
#else
    double xh, xl, yh, yl;

    split_f64(x, &xh, &xl);
    split_f64(y, &yh, &yl);
    *p = x  *y;
    *e = ((xh  *yh - *p) + xh  *yl + xl  *yh) + xl  *yl;
#endif
}

void accum3_f64(double x, double y, double z, double *s, double *e1, double *e2)
{
    sum2_f64(y, z, s, e2);
    sum2_f64(x, *s, s, e1);

    if (*e1 == 0)
    {
        *e1 = *s;
        *s = 0.0;
    }

    if (*e2 == 0.0)
    {
        *e2 = *e1;
        *e1 = *s;
        *s = 0.0;
    }
}

void sum3_f64_2(double x, double y, double z, double *s, double *e1, double *e2)
{
    sum2_f64(x, y, &x, &y);
    sum2_f64(x, z, s, &z);
    sum2_f64(y, z, e1, e2);
}

void sum3_f64(double x, double y, double z, double *s, double *e)
{
    sum2_f64(x, y, &x, &y);
    sum2_f64(x, z, s, &z);
    *e = y + z;
}

void sum6by3_f64(double x0, double x1, double x2, double y0, double y1, double y2,
                 double *s, double* e1, double *e2)
{
    sum3_f64_2(x0, x1, x2, &x0, &x1, &x2);
    sum3_f64_2(y0, y1, y2, &y0, &y1, &y2);

    sum2_f64(x0, y0, s, &y0);
    sum2_f64(x1, y1, &x1, &y1);
    sum2_f64(x1, y0, e1, &y0);

    x2 += y2;
    *e2 = x2 + y1 + y0;
}

void sum9by2_f64(double a0, double a1, double a2, double a3, double b0, double b1, 
                 double b2, double b3, double c, double* s, double* e)
{
    sum2_f64(a0, a1, &a0, &a1);
    sum2_f64(a2, a3, &a2, &a3);
    sum2_f64(b0, b1, &b0, &b1);
    sum2_f64(b2, b3, &b2, &b3);

    sum2_f64(a0, a2, &a0, &a2);
    sum2_f64(b0, b2, &b0, &b2);

    sum2_f64(a0, b0, &a0, &b0);
    a1 += (a2 + a3);
    b1 += (b2 + b3);

    sum2_f64(a0, c, s, &c);
    *e = a1 + b1 + c;
}

int cmp_abs_f64_rev(const void *x, const void *y)
{
    const double *dx = (const double*) x;
    const double *dy = (const double*) y;
    return ((fabs(*dx) < fabs(*dy)) - (fabs(*dx) > fabs(*dy)));
}

int sgn(double val)
{
    return (0.0 < val) - (val < 0.0);
}

int powi(int b, int e)
{
    if (e < 0)      return -1;
    if (e == 0)     return 1;
    if (e == 1)     return b;
    if (e % 2)      return powi(b, e - 1) * b;
    
    int i = powi(b, e / 2);
    return i * i;
}