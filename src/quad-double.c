#include <math.h>
#include <stdlib.h>
#include "quad-double.h"

struct qd_struct_t qd_powers_10[9] = {
    { .data[0] = 10.0, .data[1] = 0.0, .data[2] = 0.0, .data[3] = 0.0 },                      // 10^1
    { .data[0] = 1e+2, .data[1] = 0.0, .data[2] = 0.0, .data[3] = 0.0 },                      // 10^2
    { .data[0] = 1e+4, .data[1] = 0.0, .data[2] = 0.0, .data[3] = 0.0 },                      // 10^4
    { .data[0] = 1e+8, .data[1] = 0.0, .data[2] = 0.0, .data[3] = 0.0 },                      // ...
    { .data[0] = 1e+16, .data[1] = 0.0, .data[2] = 0.0, .data[3] = 0.0 },                        
    { .data[0] = 1.0000000000000001e+32, .data[1] = -5366162204393472, .data[2] = 0.0, .data[3] = 0.0 },
    { .data[0] = 1e+64, .data[1] = -2.1320419009454396e+47, .data[2] = -1.2300353422800673e+31, .data[3] = 0.0 },
    { .data[0] = 1.0000000000000001e+128, .data[1] = -7.5174486916518204e+111, .data[2] = -3.7746004337872939e+95, .data[3] = -2.7693106414954529e+79 },
    { .data[0] = 1e+256, .data[1] = -3.0127659900140538e+239, .data[2] = 1.2945860821658988e+223, .data[3] = 8.6460197918246651e+206 }
};

struct qd_struct_t qd_neg_powers_10[9] = {
    { .data[0] = 0.10000000000000001, .data[1] = -5.551115123125783e-18, .data[2] = 3.0814879110195775e-34, .data[3] = -1.7105694144590053e-50 },      // 10^-1
    { .data[0] = 0.01, .data[1] = -2.0816681711721689e-19, .data[2] = 9.6296497219361745e-36, .data[3] = -3.2073176521106395e-52 },                    // 10^-2
    { .data[0] = 0.0001, .data[1] = -4.7921736023859299e-21, .data[2] = -3.6833410186405893e-37, .data[3] = 9.6887720740842028e-54 },                  // 10^-3
    { .data[0] = 1e-08, .data[1] = -2.0922560830128485e-25, .data[2] = 1.62414527970063e-41, .data[3] = -1.2387181230735639e-57 },                     // 10^-4
    { .data[0] = 9.9999999999999998e-17, .data[1] = 2.0902213275965374e-33, .data[2] = -1.682610136834013e-50, .data[3] = -7.8169998873804196e-67 },   // ...
    { .data[0] = 1.0000000000000001e-32, .data[1] = -5.5967309976241948e-49, .data[2] = 5.3141961247582773e-66, .data[3] = -2.522370178408082e-82 },
    { .data[0] = 9.9999999999999997e-65, .data[1] = 3.4694261166452981e-81, .data[2] = -1.2979639752263831e-97, .data[3] = 1.7678114322863393e-114 },
    { .data[0] = 1.0000000000000001e-128, .data[1] = -5.4014088595681218e-145, .data[2] = 3.2759637959106361e-161, .data[3] = -2.3283480399724545e-179 },
    { .data[0] = 9.9999999999999998e-257, .data[1] = 2.2671708827212071e-273, .data[2] = -1.5349345474920539e-289, .data[3] = -7.0490515193067622e-306 }
};

qd_t QD_ONE = { { .data[0] = 1.0, .data[1] = 0.0, .data[2] = 0.0, .data[3] = 0.0 } };

// **** Initialization functions **** //

void qd_set(qd_t qd, qd_t x)
{
    qd->data[0] = x->data[0];
    qd->data[1] = x->data[1];
    qd->data[2] = x->data[2];
    qd->data[3] = x->data[3];
}

void qd_set_d(qd_t qd, double x)
{
    qd->data[0] = x;
    qd->data[1] = 0.0;
    qd->data[2] = 0.0;
    qd->data[3] = 0.0;
}

void qd_set_zero(qd_t qd)
{
    qd->data[0] = 0.0;
    qd->data[1] = 0.0;
    qd->data[2] = 0.0;
    qd->data[3] = 0.0;
}

// **** Arithmetic operators **** //

void qd_neg(qd_t r, qd_t x)
{
    r->data[0] = -x->data[0];
    r->data[1] = -x->data[1];
    r->data[2] = -x->data[2];
    r->data[3] = -x->data[3];
}

void qd_abs(qd_t r, qd_t x)
{
    if (qd_sgn(x))  qd_neg(r, x);
    else            qd_set(r, x);
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
    qd_t q;

    qd_set_d(q, b);
    qd_mul(r, a, q);
}

void qd_div_d(qd_t r, qd_t a, double b)
{
    qd_t q;

    qd_set_d(q, b);
    qd_div(r, a, q);
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

// **** Comparison functions **** //

int qd_cmp(qd_t a, qd_t b)
{
    for (int i = 0; i < 4; ++i)
    {
        if (a->data[i] > b->data[i])    return 1.0;
        if (a->data[i] < b->data[i])    return -1.0;
    }

    return 0.0;
}

int qd_sgn(qd_t x)
{
    return x->data[0] < 0.0;
}

int qd_is_zero(qd_t x)
{
    return (x->data[0] == 0.0);
}

int qd_is_nan(qd_t x)
{
    return isnan(x->data[0]);
}

int qd_is_inf(qd_t x)
{
    return isinf(x->data[0]);
}

int qd_is_number(qd_t x)
{
    return !isnan(x->data[0]) && !isinf(x->data[0]);
}

// **** I/O functions **** //

char *qd_to_str(qd_t qd, int len)
{
    const int MAX_DIGITS = 64;
    char *out, *pstr;
    int exp = 0;
    qd_t tmp;

    out = (char*)malloc(75);
    pstr = out;

    if (qd_sgn(qd))
    {
        out[0] = "-";
        ++pstr;
    }

    qd_abs(tmp, qd);  
    if (qd_cmp(tmp, &qd_powers_10[0]) >= 0) // qd > 10
    {
        for (int p10 = 8; p10 >= 0; --p10)
        {
            if (qd_cmp(tmp, &qd_powers_10[p10]) >= 0)
            {
                exp += powi(2, p10);
                qd_div(tmp, tmp, &qd_powers_10[p10]);
            }
        }

        qd_abs(tmp, qd);
        
    }

    if (qd_cmp(tmp, QD_ONE) <= 0) // qd < 1
    {
        for (int p10 = 8; p10 >= 0; --p10)
        {
            if (qd_cmp(tmp, &qd_neg_powers_10[p10]) <= 0)
            {
                exp -= powi(2, p10);
                qd_div(tmp, tmp, &qd_neg_powers_10[p10]);
            }
        }     
    }

    printf("(%.17g + %.17g + %.17g + %.17g) * 10^%i\n", tmp->data[0], tmp->data[1], tmp->data[2], tmp->data[3], exp);

    return out;
}

// **** Miscellaneous functions **** //

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