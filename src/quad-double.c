#include <stdlib.h>
#include "quad-double.h"

struct qd_t to_qd(double x)
{
    struct qd_t quad;
    quad.data[0] = x;
    quad.data[1] = 0.0;
    quad.data[2] = 0.0;
    quad.data[3] = 0.0;

    return quad;
}

struct qd_t to_qd4(double a0, double a1, double a2, double a3)
{
    struct qd_t quad;
    quad.data[0] = a0;
    quad.data[1] = a1;
    quad.data[2] = a2;
    quad.data[3] = a3;
    
    return quad;
}

void zero_qd(struct qd_t *x)
{
    x->data[0] = 0.0;
    x->data[1] = 0.0;
    x->data[2] = 0.0;
    x->data[3] = 0.0;
}

struct qd_t renormalize_qd(double a0, double a1, double a2, double a3, double a4)
{
    struct qd_t q;
    double t[5];
    double s;
    int k = 0;

    quick_sum2_f64(a3, a4, &s, &t[4]);
    quick_sum2_f64(a2, s, &s, &t[3]);
    quick_sum2_f64(a1, s, &s, &t[2]);
    quick_sum2_f64(a0, s, &t[0], &t[1]);

    s = t[0];
    zero_qd(&q);
    for (int i = 1; i < 5 && k < 4; ++i)
    {
        quick_sum2_f64(s, t[i], &s, &t[0]); 
        if (t[0] != 0.0)
        {
            q.data[k] = s;
            s = t[0];
            ++k;
        }
    }

    if (k < 4) q.data[k] = s;
    return q;
}

struct qd_t neg_qd(struct qd_t *x)
{
    struct qd_t q;
    q.data[0] = - x->data[0];
    q.data[1] = - x->data[1];
    q.data[2] = - x->data[2];
    q.data[3] = - x->data[3];

    return q;
}

struct qd_t add_qd_d(struct qd_t *a, double b)
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
    return renormalize_qd(m[0], m[1], m[2], m[3], m[4]);
}

struct qd_t sub_qd_d(struct qd_t *a, double b)
{
    return add_qd_d(a, -b);
}

struct qd_t mul_qd_d(struct qd_t *a, double b)
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
    
    return renormalize_qd(m[0], m[1], m[2], m[3], m[4]);
}

struct qd_t add_qd2(struct qd_t *a, struct qd_t *b)
{
    double x[8] = { a->data[0], a->data[1], a->data[2], a->data[3],
                    b->data[0], b->data[1], b->data[2], b->data[3] };
    double s = 0.0, u = 0.0, v = 0.0;
    int i = 0, k = 0;
    struct qd_t q;

    qsort(x, 8, sizeof(double), cmp_abs_f64_rev);
    zero_qd(&q);

    while (k < 4 && i < 8)
    {
        accum3_f64(u, v, x[i++], &s, &u, &v);
        if (s != 0.0)
        {
            q.data[k] = s;
            ++k;
        }
    }

    if (k < 2) q.data[k + 1] = v;
    if (k < 3) q.data[k] = u;
    return renormalize_qd(q.data[0], q.data[1], q.data[2], q.data[3], 0.0);
}

struct qd_t sub_qd2(struct qd_t *a, struct qd_t *b)
{
    struct qd_t nb = neg_qd(b);
    return add_qd2(a, &nb);
}

struct qd_t mul_qd2(struct qd_t *a, struct qd_t *b)
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

    return renormalize_qd(m00, m10, m20, m30, q01);
}

struct qd_t div_qd2(struct qd_t *a, struct qd_t *b)
{
    struct qd_t r, m;
    double q[5];

    q[0] = a->data[0] / b->data[0];
    m = mul_qd_d(b, q[0]);
    r = sub_qd2(a, &m);
    
    q[1] = r.data[0] / b->data[0];
    m = mul_qd_d(b, q[1]);
    r = sub_qd2(&r, &m);

    q[2] = r.data[0] / b->data[0];
    m = mul_qd_d(b, q[2]);
    r = sub_qd2(&r, &m);

    q[3] = r.data[0] / b->data[0];
    m = mul_qd_d(b, q[3]);
    r = sub_qd2(&r, &m);
    
    q[4] = r.data[0] / b->data[0];
    return renormalize_qd(q[0], q[1], q[2], q[3], q[4]);
}