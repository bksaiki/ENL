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
#if !defined(__FMA__) && defined(__AVX2__)
    *p = x * y;
    *e = fma(x, y, -(*p));
#else
    double xh, xl, yh, yl;

    split_f64(x, &xh, &xl);
    split_f64(y, &yh, &yl);
    *p = x * y;
    *e = ((xh * yh - *p) + xh * yl + xl * yh) + xl * yl;
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

void sum3_f64_2(double x, double y, double z, double* s, double* e1, double* e2)
{
    double u, v, w;

    sum_f64(x, y, &u, &v);
    sum_f64(u, z, s, &w);
    sum_f64(v, w, e1, e2);
}

void sum3_f64(double x, double y, double z, double* s, double* e)
{
    double u, v, w;

    sum_f64(x, y, &u, &v);
    sum_f64(u, z, s, &w);
    *e = v + w;
}

int cmp_f64(const void* x, const void* y)
{
    const double* dx = (const double*) x;
    const double* dy = (const double*) y;
    return (*dx > *dy) - (*dx < *dy);
}