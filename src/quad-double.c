#include <stdlib.h>
#include "quad-double.h"
#include <mpfr.h>

void qd_set_d(qd_t qd, double x)
{
    qd->data[0] = x;
    qd->data[1] = 0.0;
    qd->data[2] = 0.0;
    qd->data[3] = 0.0;
}

void qd_set_4d(qd_t qd, double a0, double a1, double a2, double a3)
{
    qd->data[0] = a0;
    qd->data[1] = a1;
    qd->data[2] = a2;
    qd->data[3] = a3;
}

void qd_set_zero(qd_t qd)
{
    qd->data[0] = 0.0;
    qd->data[1] = 0.0;
    qd->data[2] = 0.0;
    qd->data[3] = 0.0;
}

void qd_renormalize(qd_t qd, double a0, double a1, double a2, double a3, double a4)
{
    double t[5];
    double s;
    int k = 0;

    quick_sum2_f64(a3, a4, &s, &t[4]);
    quick_sum2_f64(a2, s, &s, &t[3]);
    quick_sum2_f64(a1, s, &s, &t[2]);
    quick_sum2_f64(a0, s, &t[0], &t[1]);

    s = t[0];
    qd_set_zero(qd);
    for (int i = 1; i < 5 && k < 4; ++i)
    {
        quick_sum2_f64(s, t[i], &s, &t[0]); 
        if (t[0] != 0.0)
        {
            qd->data[k] = s;
            s = t[0];
            ++k;
        }
    }

    if (k < 4) qd->data[k] = s;
}

void qd_neg(qd_t r, qd_t x)
{
    r->data[0] = -x->data[0];
    r->data[1] = -x->data[1];
    r->data[2] = -x->data[2];
    r->data[3] = -x->data[3];
}

void qd_add_d(qd_t r, qd_t a, double b)
{
    double s, e, t;
    double m[5];

    t = b;
    for (int i = 0; i < 4; ++i)
    {
        sum2_f64(t, a->data[i], &s, &e);
        m[i] = s;
        t = e;
    }

    m[4] = s;
    qd_renormalize(r, m[0], m[1], m[2], m[3], m[4]);
}

void qd_sub_d(qd_t r, qd_t a, double b)
{
    qd_add_d(r, a, -b);
}

void qd_mul_d(qd_t r, qd_t a, double b)
{
    double a0b, a1b, a2b;
    double m[5];

    prod_f64(a->data[0], b, &m[0], &a0b);
    prod_f64(a->data[1], b, &m[1], &a1b);
    prod_f64(a->data[2], b, &m[2], &a2b);
    m[3] = a->data[3] * b;

    sum2_f64(m[1], a0b, &m[1], &a0b);
    sum3_f64_2(m[2], a1b, a0b, &m[2], &a1b, &a0b);
    sum3_f64(m[3], a2b, a1b, &m[3], &a2b);
    m[4] = a0b + a2b;
    
    qd_renormalize(r, m[0], m[1], m[2], m[3], m[4]);
}

void qd_add(qd_t r, qd_t a, qd_t b)
{
    double x[8] = { a->data[0], a->data[1], a->data[2], a->data[3],
                    b->data[0], b->data[1], b->data[2], b->data[3] };
    double m[4] = { 0.0, 0.0, 0.0, 0.0 };
    double s = 0.0, u = 0.0, v = 0.0;
    int i = 0, k = 0;

    qsort(x, 8, sizeof(double), cmp_abs_f64_rev);
    while (k < 4 && i < 8)
    {
        accum3_f64(u, v, x[i++], &s, &u, &v);
        if (s != 0.0)
        {
            m[k] = s;
            ++k;
        }
    }

    if (k < 2) m[k + 1] = v;
    if (k < 3) m[k] = u;
    qd_renormalize(r, m[0], m[1], m[2], m[3], 0.0);
}

void qd_sub(qd_t r, qd_t a, qd_t b)
{
    qd_t nb;

    qd_neg(nb, b);
    qd_add(r, a, nb);
}

void qd_mul(qd_t r, qd_t a, qd_t b)
{
    double m00, m10, m01, m20, m11, m02, m03, m12, m21, m30, m13, m22, m31,
           q00, q10, q01, q20, q11, q02, q03, q12, q21, q30;

    prod_f64(a->data[0], b->data[0], &m00, &q00);
    prod_f64(a->data[1], b->data[0], &m10, &q10);
    prod_f64(a->data[0], b->data[1], &m01, &q01);
    prod_f64(a->data[2], b->data[0], &m20, &q20);
    prod_f64(a->data[1], b->data[1], &m11, &q11);
    prod_f64(a->data[1], b->data[2], &m02, &q02);
    prod_f64(a->data[3], b->data[0], &m30, &q30);
    prod_f64(a->data[2], b->data[1], &m21, &q21);
    prod_f64(a->data[1], b->data[2], &m12, &q12);
    prod_f64(a->data[0], b->data[3], &m03, &q03);
    m31 = a->data[3] * b->data[1];
    m22 = a->data[2] * b->data[2];
    m13 = a->data[1] * b->data[3];

    sum3_f64_2(m10, m01, q00, &m10, &m01, &q00);
    sum6by3_f64(m01, m20, m11, m02, q10, q01, &m20, &q10, &q01);
    sum9by2_f64(q00, q10, m30, m21, m12, m03, q20, q11, q02, &m30, &q20);
    q01 += (q20 + m31 + m22 + m13 + q30 + q21 + q12 + q03);

    qd_renormalize(r, m00, m10, m20, m30, q01);
}

void qd_div(qd_t res, qd_t a, qd_t b)
{
    qd_t r, m;
    double q[5];

    q[0] = a->data[0] / b->data[0];
    qd_mul_d(m, b, q[0]);
    qd_sub(r, a, m);
    
    q[1] = r->data[0] / b->data[0];
    qd_mul_d(m, b, q[1]);
    qd_sub(r, r, m);

    q[2] = r->data[0] / b->data[0];
    qd_mul_d(m, b, q[2]);
    qd_sub(r, r, m);

    q[3] = r->data[0] / b->data[0];
    qd_mul_d(m, b, q[3]);
    qd_sub(r, r, m);
    
    q[4] = r->data[0] / b->data[0];
    qd_renormalize(res, q[0], q[1], q[2], q[3], q[4]);
}