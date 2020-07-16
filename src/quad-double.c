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

void zero_qd(struct qd_t* x)
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

    quick_sum_f64(a3, a4, &s, &t[4]);
    quick_sum_f64(a2, s, &s, &t[3]);
    quick_sum_f64(a1, s, &s, &t[2]);
    quick_sum_f64(a0, s, &t[0], &t[1]);

    s = t[0];
    zero_qd(&q);
    for (int i = 1; i < 5 && k < 4; ++i)
    {
        quick_sum_f64(s, t[i], &s, &t[0]); 
        if (t[0] != 0)
        {
            q.data[k] = s;
            s = t[0];
            ++k;
        }
    }

    if (k < 4) q.data[k] = s;
    return q;
}

struct qd_t neg_qd(struct qd_t* x)
{
    struct qd_t q;
    q.data[0] = - x->data[0];
    q.data[1] = - x->data[1];
    q.data[2] = - x->data[2];
    q.data[3] = - x->data[3];

    return q;
}

struct qd_t add_qd_d(struct qd_t* a, double b)
{
    double s, e, t;
    double m[5];

    t = b;
    for (int i = 0; i < 4; ++i)
    {
        sum_f64(t, a->data[i], &s, &e);
        m[i] = s;
        t = e;
    }

    m[4] = s;
    return renormalize_qd(m[0], m[1], m[2], m[3], m[4]);
}

struct qd_t add_qd2(struct qd_t* a, struct qd_t* b)
{
    double x[8] = { a->data[0], a->data[1], a->data[2], a->data[3],
                      b->data[0], b->data[1], b->data[2], b->data[3] };
    double s = 0.0, u = 0.0, v = 0.0;
    int i = 0, k = 0;
    struct qd_t q;

    qsort(x, 8, 8, cmp_f64);
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
    return q;
}

struct qd_t sub_qd_d(struct qd_t* a, double b)
{
    return add_qd_d(a, -b);
}

struct qd_t sub_qd2(struct qd_t* a, struct qd_t* b)
{
    struct qd_t nb = neg_qd(b);
    return add_qd2(a, &nb);
}

