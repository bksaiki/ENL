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
    { .data[0] = 1e+32, .data[1] = -5366162204393472.0, .data[2] = 0.0, .data[3] = 0.0 },
    { .data[0] = 1e+64, .data[1] = -2.1320419009454396e+47, .data[2] = -1.2300353422800673e+31, .data[3] = 0.0 },
    { .data[0] = 1e+128, .data[1] = -7.51744869165182e+111, .data[2] = -5.0046357760673614e+95, .data[3] = -2.2998043059195224e+78 },
    { .data[0] = 1e+256, .data[1] = -3.012765990014054e+239, .data[2] = -3.132205662339866e+222, .data[3] = -8.48134226662446e+205 }
};

struct qd_struct_t qd_neg_powers_10[9] = {
    { .data[0] = 1e-1, .data[1] = -5.551115123125783e-18, .data[2] = 3.0814879110195775e-34, .data[3] = -1.7105694144590053e-50 },      // 10^-1
    { .data[0] = 1e-2, .data[1] = -2.0816681711721684e-19, .data[2] = -7.703719777548944e-36, .data[3] = 1.6036588260553173e-52 },      // 10^-2
    { .data[0] = 1e-4, .data[1] = -4.79217360238593e-21, .data[2] = 3.442599775592184e-37, .data[3] = -5.078252949175172e-54 },         // 10^-3
    { .data[0] = 1e-8, .data[1] = -2.092256083012847e-25, .data[2] = -1.582638574173095e-41, .data[3] = 5.4982215447355565e-59 },       // 10^-4
    { .data[0] = 1e-16, .data[1] = 2.0902213275965398e-33, .data[2] = -1.3550063967589306e-49, .data[3] = 8.651653255873321e-66 },      // ...
    { .data[0] = 1e-32, .data[1] = 2-5.59673099762419e-49, .data[2] = 3.0688625784648003e-66, .data[3] = -2.3719300994621642e-82 },
    { .data[0] = 1e-64, .data[1] = 23.469426116645307e-81, .data[2] = 9.899885481330491e-98, .data[3] = -2.825571992850639e-114 },
    { .data[0] = 1e-128, .data[1] = 2-5.401408859568103e-145, .data[2] = -4.759216114517928e-162, .data[3] = -3.583550944062863e-178 },
    { .data[0] = 1e-256, .data[1] = 2.2671708827212437e-273, .data[2] = -1.1994609675448683e-289, .data[3] = -3.060993260309854e-306 }
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
    if (*idx == '-')
    {
        sign = true;
        ++pbeg;
    }
    else if (*idx == '+')
    {
        ++pbeg;
    }

    pdot = pbeg;
    pexp = pbeg;
    qd_set_zero(qd);
    for (; *idx != '\0'; ++idx)
    {
        if (*idx == '.')
        {
            if (pdot != pbeg)    return;     // ill-formed
            pdot = idx + 1;
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
            else if (*(idx + 1) < '0' || *(idx + 1) > '9')
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

        // negate pow10 if exp sign negative
        if (exp_sign) pow10 *= -1;
        if (pdot != pbeg)   pow10 -= (int)(pexp - pdot);
    }
    else
    {
        if (pdot != pbeg)   pow10 -= (int)(pend - pdot);
    }

    abspow10 = abs(pow10);
    if (abspow10 != 0)
    {
        qd_set_d(qd10, 1.0);
        for (int i = 8, pow2 = 256; i >= 0; --i, pow2 /= 2)
        {
            if (abspow10 >= pow2)
            {
                qd_mul(qd10, &qd_powers_10[i], qd10);
                abspow10 -= pow2;
            }
        }

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
    double m0, m1, m2, m3, e0, e1, e2;

    prod_f64(a->data[0], b, &m0, &e0);
    prod_f64(a->data[1], b, &m1, &e1);
    prod_f64(a->data[2], b, &m2, &e2);
    m3 = a->data[3] * b;

    sum2_f64(m1, e0, &m1, &e0);
    sum3_f64_2(m2, e1, e0, &m2, &e1, &e0);
    sum3_f64(m3, e2, e1, &m3, &e1);
    e0 += e1;

    qd_renormalize(r, m0, m1, m2, m3, e0);
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
    prod_f64(a->data[0], b->data[2], &p02, &q02); // a0b2
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

void qd_powi(qd_t r, qd_t a, int b)
{
    qd_t p;

    if (b < 0)
    {
        qd_powi(r, a, -b);
        qd_div(r, QD_ONE, r);
    }
    else if (b == 0)     
    {
        qd_set(r, QD_ONE);
    }
    else if (b == 1)    
    {
        qd_set(r, a);
    }
    else if (b % 2)
    {
        qd_powi(p, a, b - 1);
        qd_mul(r, p, a);
    }
    else
    {
        qd_powi(p, a, b / 2);
        qd_mul(r, p, p);
    }
}

void qd_sqrt(qd_t r, qd_t a)
{
    qd_t x0, t;

    qd_set_d(x0, sqrt(a->data[0]));  // initial guess

    // Newton's iteration: x(n+1) = x(n) + [a / x(n) -  x(n)] / 2
    // quadratically convergent, around 2 iterations required
    for (int i = 0; i < 3; ++i)
    {
        qd_div(t, a, x0);
        qd_sub(t, t, x0);

        t->data[0] /= 2.0;
        t->data[1] /= 2.0;
        t->data[2] /= 2.0;
        t->data[3] /= 2.0;

        qd_add(x0, t, x0);
    }

    qd_set(r, x0);
}

void qd_nroot(qd_t r, qd_t a, int n)
{
    qd_t x0, t, s;
    char* str;

    if (n < 0) // x^(1/(-n)) => 1 / x^(1/n)
    {
        qd_nroot(t, a, -n);
        qd_div(r, QD_ONE, t);
    }
    else if (n == 0) // nan
    {

    }
    else
    {
        // initial guess: nth root of a0
        qd_set_d(x0, pow(a->data[0], (1.0 / ((double) n)))); 
    
        str = qd_to_str(x0, 0);
	    printf("nroot init: %.17g -> %s\n", a->data[0], str);
	    free(str);

        // Newton's iteration: x(k+1) = (1/n)*[(n-1)*x(k) + a / x(k)^(n-1)]
        // quadratically convergent, around 2 iterations required
        for (int i = 0; i < 3; ++i)
        {
            qd_powi(t, x0, n - 1);
            qd_div(t, a, t);
            qd_mul_d(s, x0, (double)(n - 1));
            qd_add(t, s, t);
            qd_div_d(x0, t, (double) n);
        }

        qd_set(r, x0);
    }
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
    return (x->data[0] < 0.0);
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
    qd_t tmp;
    char *out, *pstr, *pend;
    int digit, abs_exp, exp_len;
    int exp = 0;

    // normalize qd => x * 10^n where 1 <= x < 10
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

    if (qd_cmp(tmp, QD_ONE) < 0)
    {
        for (int p10 = 8; p10 >= 0; --p10)
        {
            if (qd_cmp(tmp, &qd_neg_powers_10[p10]) <= 0)
            {
                exp -= powi(2, p10);
                qd_div(tmp, tmp, &qd_neg_powers_10[p10]);
            }
        }     

        if (qd_cmp(tmp, QD_ONE) < 0)
        {
            exp -= 1;
            qd_mul_d(tmp, tmp, 10);
        }
    }

    if (len == 0)           len = 66; // default string length
    if (qd_sgn(tmp))        ++len;

    abs_exp = abs(exp);
    if (abs_exp >= 100)     exp_len = 4; // exponent sign
    else                    exp_len = 3; // leading zero if needed  

    len += (exp_len + 2); // decimal point, exponent mark

    out = (char*)malloc(len);
    pstr = out;
    pend = out + len - 1;

    if (qd_sgn(qd))
    {
        *pstr = '-';
        ++pstr;
    }

    // round down if next significand negative and current significand is exact integer
    digit = (int)tmp->data[0];
    if (((double)digit == tmp->data[0]) && (tmp->data[1] < 0.0))  --digit;
    *pstr = (char)digit + '0';
    ++pstr;

    qd_sub_d(tmp, tmp, (double)digit);
    qd_mul_d(tmp, tmp, 10.0);
    *pstr = '.';
    ++pstr;

    while (!qd_is_zero(tmp) && (pstr - out) < (len - exp_len - 2))
    {
        digit = (int)tmp->data[0];
        if (((double)digit == tmp->data[0]) && (tmp->data[1] < 0.0))  --digit;
        *pstr = (char)digit + '0';
        qd_sub_d(tmp, tmp, (double)digit);
        qd_mul_d(tmp, tmp, 10.0);
        ++pstr;
    }

    while ((pstr - out) < (len - exp_len - 2)) // trailing zeros
    {
        *pstr = '0';
        ++pstr;
    }

    *pstr = 'e';
    ++pstr;

    if (exp < 0)  *pstr = '-';
    else          *pstr = '+';
    ++pstr;

    pstr = pend - 1;
    for (int i = 0; i < (exp_len - 1); ++i)
    {
        *pstr = ((char)(abs_exp % 10) + '0');
        abs_exp /= 10;
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