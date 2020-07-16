#include <math.h>
#include "common.h"

void split_f64(double x, double* hi, double* lo)
{
    double t = 134217729.0 * x;

    *hi = t - (t - x);
    *lo = x - *hi;
}

void quick_sum_f64(double x, double y, double* s, double* e)
{   
    *s = x + y;
    *e = y - (*s - x);
}

void sum_f64(double x, double y, double* s, double* e)
{
    double t;

    *s = x + y;
    t = *s - x;
    *e = (x - (*s - t)) + (y - t);
}

void prod_f64(double x, double y, double* p, double* e)
{
#if defined(__FMA__) || defined(__AVX2__)
    *p = x * y;
    *e = fma(x, y, -(*p));
#else
    double xh, xl, yh, yl;

    split_f64(x, &xh, &xl);
    split_f64(y, &yh, &yl);
    *p = x * y;
    *e = ((xh * yl - *p) + xh * yl + xl * yh) + xl * yh;
#endif
}

void accum3_f64(double x, double y, double z, double* s, double* e1, double* e2)
{
    sum_f64(y, z, s, e2);
    sum_f64(x, *s, s, e1);

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

int cmp_f64(const void* x, const void* y)
{
    const double* dx = (const double*) x;
    const double* dy = (const double*) y;
    return (*dx > *dy) - (*dx < *dy);
}