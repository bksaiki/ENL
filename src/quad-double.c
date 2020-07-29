#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include "quad-double.h"

struct qd_struct_t qd_powers_10[9] = {
    { .data[0] = 10.0, .data[1] = 0.0, .data[2] = 0.0, .data[3] = 0.0 },                      // 10^1
    { .data[0] = 1e+2, .data[1] = 0.0, .data[2] = 0.0, .data[3] = 0.0 },                      // 10^2
    { .data[0] = 1e+4, .data[1] = 0.0, .data[2] = 0.0, .data[3] = 0.0 },                      // 10^4
    { .data[0] = 1e+8, .data[1] = 0.0, .data[2] = 0.0, .data[3] = 0.0 },                      // ...
    { .data[0] = 1e+16, .data[1] = 0.0, .data[2] = 0.0, .data[3] = 0.0 },                        
    { .data[0] = 1.0000000000000000537e+32, .data[1] = -5366162204393472, .data[2] = 0.0, .data[3] = 0.0 },
    { .data[0] = 1.0000000000000000213e+64, .data[1] = -2.1320419009454395642e+47, .data[2] = -1.2300353422800672893e+31, .data[3] = 0.0 },
    { .data[0] = 1.0000000000000000752e+128, .data[1] = -7.5174486916518203623e+111, .data[2] = -3.7746004337872938512e+95, .data[3] = -2.7693106414954529128e+79 },
    { .data[0] = 1.0000000000000000301e+256, .data[1] = -3.0127659900140537563e+239, .data[2] = 1.2945860821658988444e+223, .data[3] = 8.6460197918246650621e+206 }
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

void qd_set_str(qd_t qd, char* str)
{
    char *idx, *pbeg, *pdot, *pexp, *pexpn, *pend;
    qd_t qd10;
    int pow10, abspow10;
    bool sign = false, exp_sign = false;

    idx = str;
    while (isspace(*idx)) ++idx;

    pbeg = idx;
    pend = idx;
    pexpn = idx;
    if (*idx == '-')        sign = true;
    else if (*idx != '+')   pbeg = idx - 1;

    pdot = pbeg;
    pexp = pbeg;
    for (; *idx != '\0'; ++idx)
    {
        if (*idx == '.')
        {
            if (pdot != pbeg)    return;     // ill-formed
            pdot = idx;
        }
        else if (*idx >= '0' && *idx <= '9')
        {
            qd_mul_d(qd, qd, 10.0);
            qd_add_d(qd, qd, (double)(*idx - '0'));
        }
        else if (*idx == 'e')
        {   
            if (pexp != pbeg)    return;     // ill-formed
            pexp = idx;
            pexpn = idx + 1;

            if (*(idx + 1) == '+')
            {
                ++pexpn;
                ++idx;
            }
            else if (*(idx + 1) == '-')
            {
                ++pexpn;
                ++idx;
                exp_sign = true;
            }
            else if (*(idx + 1) <= '0' || *(idx + 1) >= '9')
            {
                return;     // ill-formed
            }    

            while (*idx != '\0')  ++idx; // find the end
            break;   // and exit
        }      
        else
        {
            return;     // ill-formed
        }
    }

    pend = idx;
    pow10 = 0;
    if (pexp != pbeg)
    {
        for (char* it = pexpn; it != pend; ++it)
        {
            pow10 *= 10;
            pow10 += (int)(*it - '0');
        }

        if (exp_sign) pow10 *= -1; // negate pow10 if exp sign negative
        pow10 += (int)(pdot - pexp + 1);
    }
    else
    {
        pow10 += (int)(pdot - pend + 1);
    }

    printf("%.20g + %.20g + %.20g + %.20g\n", qd->data[0], qd->data[1], qd->data[2], qd->data[3]);

    abspow10 = abs(pow10);
    if (abspow10 != 0)
    {
        qd_set_d(qd10, 1.0);
        for (int i = 8, pow2 = 256; i >= 0; --i, pow2 /= 2)
        {
            if (abspow10 >= pow2)
            {
                qd_mul(qd10, qd10, &qd_powers_10[i]);
                abspow10 -= pow2;
            }
        }

        printf("10^|%i| = %.20g + %.20g + %.20g + %.20g\n", pow10, qd10->data[0], qd10->data[1], qd10->data[2], qd10->data[3]);

        if (pow10 > 0)      qd_mul(qd, qd, qd10);
        else                qd_div(qd, qd, qd10);
    }

    if (sign)  qd_neg(qd, qd);
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
    double m[4];
    double t;

    sum2_f64(a->data[0], b, &m[0], &t);
    sum2_f64(a->data[1], t, &m[1], &t);
    sum2_f64(a->data[2], t, &m[2], &t);
    sum2_f64(a->data[3], t, &m[3], &t);

    qd_renormalize(r, m[0], m[1], m[2], m[3], t);
}

void qd_sub_d(qd_t r, qd_t a, double b)
{
    qd_add_d(r, a, -b);
}

void qd_mul_d(qd_t r, qd_t a, double b)
{
    double e0, e1, e2;

    prod_f64(a->data[0], b, &r->data[0], &e0);
    prod_f64(a->data[1], b, &r->data[1], &e1);
    prod_f64(a->data[2], b, &r->data[2], &e2);
    a->data[3] *= b;

    sum2_f64(r->data[1], e0, &r->data[1], &e0);
    sum3_f64_2(r->data[2], e1, e0, &r->data[2], &e1, &e0);
    sum3_f64(a->data[3], e2, e1, &r->data[3], &e1);
    e0 += e1;

    qd_renormalize(r, r->data[0], r->data[1], r->data[2], r->data[3], e0);
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
    int k = 0;

    qsort(x, 8, sizeof(double), cmp_abs_f64_rev);
    for (int i = 0; k < 4 && i < 8; ++i)
    {
        accum3_f64(u, v, x[i], &s, &u, &v);
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
    double p00, p10, p01, p20, p11, p02, p03, p12, p21, p30, p13, p22, p31,
           q00, q10, q01, q20, q11, q02, q03, q12, q21, q30;

    prod_f64(a->data[0], b->data[0], &p00, &q00); // a0b0
    prod_f64(a->data[1], b->data[0], &p10, &q10); // a1b0
    prod_f64(a->data[0], b->data[1], &p01, &q01); // a0b1
    prod_f64(a->data[2], b->data[0], &p20, &q20); // a2b0
    prod_f64(a->data[1], b->data[1], &p11, &q11); // a1b1
    prod_f64(a->data[1], b->data[2], &p02, &q02); // a0b2
    prod_f64(a->data[3], b->data[0], &p30, &q30); // a3b0
    prod_f64(a->data[2], b->data[1], &p21, &q21); // a2b1
    prod_f64(a->data[1], b->data[2], &p12, &q12); // a1b2
    prod_f64(a->data[0], b->data[3], &p03, &q03); // a0b3
    p31 = a->data[3] * b->data[1]; // a3b1
    p22 = a->data[2] * b->data[2]; // a2b2
    p13 = a->data[1] * b->data[3]; // a1b3

    sum3_f64_2(p10, p01, q00, &p10, &p01, &q00);
    sum6by3_f64(p01, q10, q01, p20, p11, p02, &p20, &q10, &q01);
    sum9by2_f64(q00, q20, q11, q02, p30, p21, p12, p03, q10, &p30, &q20);
    q01 += (q20 + p31 + p22 + p13 + q30 + q21 + q12 + q03);

    qd_renormalize(r, p00, p10, p20, p30, q01);
}

void qd_div(qd_t res, qd_t a, qd_t b)
{
    qd_t r, m;
    double q[4];

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
    
    qd_renormalize(res, q[0], q[1], q[2], q[3], r->data[0] / b->data[0]);
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

char* qd_to_str(qd_t qd, int len)
{
    char *out, *pstr, *pend;
    int exp = 0, exp_len = 0;
    int digit;
    qd_t tmp;

    qd_abs(tmp, qd);  
    if (qd_cmp(tmp, &qd_powers_10[0]) >= 0) // qd >= 10
    {
        for (int p10 = 8; p10 >= 0; --p10)
        {
            if (qd_cmp(tmp, &qd_powers_10[p10]) >= 0)
            {
                exp += powi(2, p10);
                qd_div(tmp, tmp, &qd_powers_10[p10]);
            }
        }
    }

    if (qd_cmp(tmp, QD_ONE) < 0) // qd < 1
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

    if (len == 0)           len = 64;
    if (qd_sgn(tmp))        ++len;
    ++len; // decimal point, TODO: non_scientific format
    ++len; // exponent mark

    if (exp >= 100)         exp_len = 3;
    else if (exp >= 10)     exp_len = 2;
    else                    exp_len = 1;
    len += exp_len;

    out = (char*)malloc(len);
    pstr = out;
    pend = out + (len - 1);

    if (qd_sgn(qd)) out[pstr++ - out] = '-';
    digit = (int)tmp->data[0];
    *pstr = (char)digit + '0';
    ++pstr;

    qd_sub_d(tmp, tmp, (double)digit);
    qd_mul_d(tmp, tmp, 10.0);
    *pstr = '.';
    ++pstr;

    while (!qd_is_zero(tmp) && (pstr - out) < (len - exp_len - 2))
    {
        digit = (int)tmp->data[0];
        *pstr = (char)digit + '0';
        qd_sub_d(tmp, tmp, (double)digit);
        qd_mul_d(tmp, tmp, 10.0);
        ++pstr;
    }

    while ((pstr - out) < (len - exp_len - 2))
    {
        *pstr = '0';
        ++pstr;
    }

    *pstr = 'e';
    ++pstr;

    pstr = pend - 1;
    for (int i = 0; i < exp_len; ++i)
    {
        *pstr = ((char)(exp % 10) + '0');
        exp /= 10;
        --pstr;
    } 

    *pend = '\0';
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