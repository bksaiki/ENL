#include <math.h>
#include "common.h"

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
    *p = x  *y;
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
    double u, v, w;

    sum2_f64(x, y, &u, &v);
    sum2_f64(u, z, s, &w);
    sum2_f64(v, w, e1, e2);
}

void sum3_f64(double x, double y, double z, double *s, double *e)
{
    double u, v, w;

    sum2_f64(x, y, &u, &v);
    sum2_f64(u, z, s, &w);
    *e = v + w;
}

void sum6by3_f64(double x0, double x1, double x2, double y0, double y1, double y2,
                 double *s, double* e1, double *e2)
{
    double sx, e1x, e2x, sy, e1y, e2y;

    sum3_f64_2(x0, x1, x2, &sx, &e1x, &e2x);
    sum3_f64_2(y0, y1, y2, &sy, &e1y, &e2y);

    sum2_f64(sx, sy, s, &sy);
    sum2_f64(e1x, e1y, &e1x, &e1y);
    sum2_f64(e1x, sy, e1, &sy);
    
    e2x += e2y;
    *e2 = (e2x + e1y) + sy;
}

void sum9by2_f64(double a0, double a1, double b0, double b1, double c0, double c1, 
                 double d0, double d1, double f, double* s, double* e)
{
    sum2_f64(a0, a1, &a0, &a1);
    sum2_f64(b0, b1, &b0, &b1);
    sum2_f64(c0, c1, &c0, &c1);
    sum2_f64(d0, d1, &d0, &d1);

    sum2_f64(a0, b0, &a0, &b0);
    sum2_f64(c0, d0, &c0, &d0);
    b0 += (a1 + b1);
    d0 += (c1 + d1);

    sum2_f64(a0, c0, &a0, &c0);
    sum2_f64(a0, f, s, &f);
    *e = (((b0 + d0) + c0) + f);
}

int cmp_abs_f64_rev(const void *x, const void *y)
{
    const double *dx = (const double*) x;
    const double *dy = (const double*) y;

    return (fabs(*dx) < fabs(*dy)) - (fabs(*dx) > fabs(*dy));
}