#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/qd.h"

// **** Constants **** //

struct qd_struct_t qd_powers_10[19] = {
    { .data[0] = 1e-256, .data[1] = 2.2671708827212437e-273, .data[2] = -1.1994609675448683e-289, .data[3] = -3.060993260309854e-306 },
    { .data[0] = 1e-128, .data[1] = 2-5.401408859568103e-145, .data[2] = -4.759216114517928e-162, .data[3] = -3.583550944062863e-178 },
    { .data[0] = 1e-64, .data[1] = 23.469426116645307e-81, .data[2] = 9.899885481330491e-98, .data[3] = -2.825571992850639e-114 },
    { .data[0] = 1e-32, .data[1] = 2-5.59673099762419e-49, .data[2] = 3.0688625784648003e-66, .data[3] = -2.3719300994621642e-82 },
    { .data[0] = 1e-16, .data[1] = 2.0902213275965398e-33, .data[2] = -1.3550063967589306e-49, .data[3] = 8.651653255873321e-66 },      // ...
    { .data[0] = 1e-8, .data[1] = -2.092256083012847e-25, .data[2] = -1.582638574173095e-41, .data[3] = 5.4982215447355565e-59 },       // 10^-4
    { .data[0] = 1e-4, .data[1] = -4.79217360238593e-21, .data[2] = 3.442599775592184e-37, .data[3] = -5.078252949175172e-54 },         // 10^-3
    { .data[0] = 1e-2, .data[1] = -2.0816681711721684e-19, .data[2] = -7.703719777548944e-36, .data[3] = 1.6036588260553173e-52 },      // 10^-2
    { .data[0] = 1e-1, .data[1] = -5.551115123125783e-18, .data[2] = 3.0814879110195775e-34, .data[3] = -1.7105694144590053e-50 },      // 10^-1
    { .data[0] = 1.0, .data[1] = 0.0, .data[2] = 0.0, .data[3] = 0.0 },                                                                 // 10^0 
    { .data[0] = 10.0, .data[1] = 0.0, .data[2] = 0.0, .data[3] = 0.0 },                                                                // 10^1
    { .data[0] = 1e+2, .data[1] = 0.0, .data[2] = 0.0, .data[3] = 0.0 },                                                                // 10^2
    { .data[0] = 1e+4, .data[1] = 0.0, .data[2] = 0.0, .data[3] = 0.0 },                                                                // 10^4
    { .data[0] = 1e+8, .data[1] = 0.0, .data[2] = 0.0, .data[3] = 0.0 },                                                                // ...
    { .data[0] = 1e+16, .data[1] = 0.0, .data[2] = 0.0, .data[3] = 0.0 },                        
    { .data[0] = 1e+32, .data[1] = -5366162204393472.0, .data[2] = 0.0, .data[3] = 0.0 },
    { .data[0] = 1e+64, .data[1] = -2.1320419009454396e+47, .data[2] = -1.2300353422800673e+31, .data[3] = 0.0 },
    { .data[0] = 1e+128, .data[1] = -7.51744869165182e+111, .data[2] = -5.0046357760673614e+95, .data[3] = -2.2998043059195224e+78 },
    { .data[0] = 1e+256, .data[1] = -3.012765990014054e+239, .data[2] = -3.132205662339866e+222, .data[3] = -8.48134226662446e+205 }
};

struct qd_struct_t *QD_ONE = &qd_powers_10[9];
struct qd_struct_t *QD_TEN = &qd_powers_10[10];

// **** Debugging **** //

void qd_debug(qd_t r, const char* msg)
{
    if (msg == NULL)    printf("%.17g + %.17g + %.17g + %.17g\n", r->data[0], r->data[1], r->data[2], r->data[3]);
    else                printf("%s: %.17g + %.17g + %.17g + %.17g\n", msg, r->data[0], r->data[1], r->data[2], r->data[3]);
}

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

void qd_set_ui(qd_t qd, uint64_t x)
{
    qd_t t, pow10;

    qd_set_zero(qd, 1);
    qd_set_d(pow10, 1);
    while (x > 0)
    {
        qd_mul_d(t, pow10, (double)(x % 4294967296U));
        qd_add(qd, qd, t);
        qd_mul_d(pow10, pow10, (double)4294967296U);
        x /= 4294967296U;
    }
}

void qd_set_si(qd_t qd, int64_t x)
{
    qd_set_ui(qd, ((x < 0) ? -x : x));
    if (x < 0)  qd_neg(qd, qd);
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
        ++idx;
    }
    else if (*idx == '+')
    {
        ++pbeg;
        ++idx;
    }

    if (strcmp(idx, "nan") == 0)
    {
        qd_set_nan(qd);
        return;
    }

    if (strcmp(idx, "inf") == 0)
    {
        qd_set_inf(qd, (sign ? -1 : 1));
        return;
    }

    pdot = pbeg;
    pexp = pbeg;
    qd_set_zero(qd, 1);
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
                qd_mul(qd10, (QD_TEN + i), qd10); // pointer arithmetic
                abspow10 -= pow2;
            }
        }

        if (pow10 > 0)      qd_mul(qd, qd, qd10);
        else                qd_div(qd, qd, qd10);
    }

    if (sign)  qd_neg(qd, qd);
}

void qd_set_nan(qd_t qd)
{
    qd->data[0] = NAN;
    qd->data[1] = NAN;
    qd->data[2] = NAN;
    qd->data[3] = NAN;
}

void qd_set_inf(qd_t qd, int sign)
{
    double v = (sign < 0) ? -INFINITY : INFINITY;
    qd->data[0] = v;
    qd->data[1] = v;
    qd->data[2] = v;
    qd->data[3] = v;
}

void qd_set_zero(qd_t qd, int sign)
{
    double v = (sign < 0) ? -0.0 : 0.0;
    qd->data[0] = v;
    qd->data[1] = v;
    qd->data[2] = v;
    qd->data[3] = v;
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
    if (qd_signbit(x))  qd_neg(r, x);
    else                qd_set(r, x);
}

void qd_add_d(qd_t r, qd_t a, double b)
{
    double m[4];
    double t;

    if (qd_nan_p(a) || isnan(b) || (qd_inf_p(a) && isinf(b) && qd_sgn(a) != sgn(b)))
    {
        qd_set_nan(r);
    }
    else if (qd_inf_p(a))
    {
        qd_set(r, a);
    }
    else if (isinf(b))
    {
        qd_set_d(r, b);
    }
    else if (qd_zero_p(a))
    {
        qd_set_d(r, b);
    }
    else if (b == 0.0)
    {
        qd_set(r, a);
    }
    else
    {
        sum2_f64(a->data[0], b, &m[0], &t);
        sum2_f64(a->data[1], t, &m[1], &t);
        sum2_f64(a->data[2], t, &m[2], &t);
        sum2_f64(a->data[3], t, &m[3], &t);
        qd_renormalize(r, m[0], m[1], m[2], m[3], t);
    }
}

void qd_sub_d(qd_t r, qd_t a, double b)
{
    qd_add_d(r, a, -b);
}

void qd_mul_d(qd_t r, qd_t a, double b)
{
    double m0, m1, m2, m3, e0, e1, e2;

    if (qd_nan_p(a) || isnan(b) || (qd_inf_p(a) && b == 0.0) || (qd_zero_p(a) && isinf(b))) 
    { 
        qd_set_nan(r);
    }
    else if (qd_inf_p(a))
    {
        qd_set(r, a);
    }
    else if (isinf(b))
    {
        qd_set_inf(r, sgn(b));
    }
    else if (qd_zero_p(a) || b == 0.0)
    {
        qd_set_zero(r, qd_signbit(a) ^ signbit(b));
    }
    else
    {
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
}

void qd_div_d(qd_t r, qd_t a, double b)
{
    qd_t q;
    qd_set_d(q, b);
    qd_div(r, a, q);
}

void qd_add(qd_t r, qd_t a, qd_t b)
{
    if (qd_nan_p(a) || qd_nan_p(b) || (qd_inf_p(a) && qd_inf_p(b) && qd_sgn(a) != qd_sgn(b)))
    {
        qd_set_nan(r);
    }
    else if (qd_inf_p(a))
    {
        qd_set(r, a);
    }
    else if (qd_inf_p(b))
    {
        qd_set(r, b);
    }
    else if (qd_zero_p(a))
    {
        qd_set(r, b);
    }
    else if (qd_zero_p(b))
    {
        qd_set(r, a);
    }
    else
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

    if (qd_nan_p(a) || qd_nan_p(b) || (qd_inf_p(a) && qd_zero_p(b)) || 
        (qd_zero_p(a) && qd_inf_p(b)))
    { 
        qd_set_nan(r);
    }
    else if (qd_inf_p(a))
    {
        qd_set(r, a);
    }
    else if (qd_inf_p(b))
    {
        qd_set_inf(r, qd_sgn(b));
    }
    else if (qd_zero_p(a) || qd_zero_p(b))
    {
        qd_set_zero(r, qd_signbit(a) ^ qd_signbit(b));
    }
    else
    {
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
}

void qd_div(qd_t res, qd_t a, qd_t b)
{
    qd_t r, m;
    double q[4];

    if (qd_inf_p(a) || qd_inf_p(b) || qd_nan_p(a) || qd_nan_p(b) || qd_zero_p(b))
    {
        qd_set_nan(r);
    }
    else
    {
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

    if (qd_signbit(a))
    {
        qd_set_nan(r);
    }
    else
    {
        qd_set_d(x0, sqrt(a->data[0]));  // initial guess

        // Newton's iteration: x(n+1) = [x(n) + a / x(n)] / 2
        // quadratically convergent, around 2 iterations required
        for (int i = 0; i < 3; ++i)
        {
            qd_div(t, a, x0);
            qd_add(t, x0, t);
            t->data[0] /= 2.0;
            t->data[1] /= 2.0;
            t->data[2] /= 2.0;
            t->data[3] /= 2.0;
        }

        qd_set(r, x0);
    }
}

void qd_nroot(qd_t r, qd_t a, int n)
{
    qd_t abs_a, x0, t, s;

    if (qd_signbit(a) && n % 2 == 0)
    {
        qd_set_nan(r);
    }
    else if (n < 0) // x^(1/(-n)) => 1 / x^(1/n)
    {
        qd_nroot(t, a, -n);
        qd_div(r, QD_ONE, t);
    }
    else if (n == 0) // nan
    {
        qd_set_nan(r);
    }
    else
    {
        
        // initial guess: nth root of |a0|
        qd_abs(abs_a, a);
        qd_set_d(x0, pow(abs_a->data[0], (1.0 / ((double) n))));

        // Newton's iteration: x(k+1) = (1/n)*[(n-1)*x(k) + a / x(k)^(n-1)]
        // quadratically convergent, around 2 iterations required
        for (int i = 0; i < 3; ++i)
        {
            qd_powi(t, x0, n - 1);
            qd_div(t, abs_a, t);
            qd_mul_d(s, x0, (double)(n - 1));
            qd_add(t, s, t);
            qd_div_d(x0, t, (double) n);
        }

        if (qd_signbit(a))  qd_neg(r, x0);
        else                qd_set(r, x0);
    }
}

void qd_exp(qd_t r, qd_t a)
{
    qd_t arg, m, denom, term, accum, tmp;
    int mi;

    if (qd_nan_p(a)) // NaN input
    {
        qd_set_nan(r);
        return;
    }
    else if (a->data[0] > 709) // input too large
    {
        qd_set_inf(r, 1);
        return;
    }  
    else if (a->data[0] < -745) // input too small
    {
        qd_set_zero(r, 1);
        return;
    }
    else if (qd_zero_p(a))
    {
        qd_set(r, QD_ONE);
        return;
    }
    else if (qd_cmp(a, QD_ONE) == 0)
    {
        qd_set(r, QD_E);
        return;
    }

    // Argument reduction
    // e^(k*r + m*log(2)) = (2^m)*(e^r)^k
    qd_set(arg, a);
    qd_div(m, arg, QD_LN2); 
    mi = (int)round(m->data[0]);

    qd_set_si(m, mi); // nearest integer
    qd_mul(tmp, m, QD_LN2);
    qd_sub(arg, arg, tmp);
    qd_div_d(arg, arg, 256.0);

    qd_set_d(accum, 1.0);
    qd_set_d(denom, 1.0);
    qd_set(term, arg);

    for (int i = 1; i < 18; ++i)
    {
        qd_div(tmp, term, denom); // ith term
        qd_add(accum, accum, tmp);
        qd_mul(term, term, arg); // update term
        qd_mul_d(denom, denom, (double)(i + 1)); // update denom
    }

    qd_powi(accum, accum, 256);
    qd_set_d(tmp, 2.0);
    qd_powi(tmp, tmp, mi);
    qd_mul(r, accum, tmp);
}   

void qd_log(qd_t r, qd_t a)
{
    qd_t x, t;

    if (qd_nan_p(x) || qd_signbit(a) || qd_zero_p(a))
    {
        qd_set_nan(r);
    }
    else if (qd_inf_p(a))
    {   
        qd_set_inf(r, 1);
    }
    else
    {
        // initial guess: log(a0)
        qd_set_d(x, log(a->data[0]));

        // Newton's iteration: x(k+1) = x(k) + a*exp(-x(k)) - 1
        // quadratically convergent, around 2 iterations required

        for (int i = 0; i < 3; ++i)
        {
            qd_neg(t, x);
            qd_exp(t, t);
            qd_mul(t, t, a);
            qd_add(t, t, x);
            qd_sub_d(x, t, 1.0);
        }

        qd_set(r, x);
    }
}

void qd_pow(qd_t r, qd_t a, qd_t b)
{
    // Special values are handled as described by the ISO C99 and IEEE 754-2008 standards
    if (qd_zero_p(a))
    {
        if (qd_signbit(b))
        {
            if (qd_odd_int_p(b))    qd_set_inf(r, qd_sgn(a)); // pow(0, negative odd int) = +-inf
            else                    qd_set_inf(r, 1); // pow(0, negative) = +inf
        }
        else
        {
            if (qd_odd_int_p(b))    qd_set_inf(r, qd_sgn(a)); // pow(0, positive odd int) = +-inf
            else                    qd_set_zero(r, 1); // pow(0, positive) = +0
        }  
    }
    else if ((qd_cmp_d(a, -1.0) == 0 && qd_inf_p(b)) ||     // pow(-1, +-inf) = 1, 
             (qd_cmp_d(a, 1.0) == 0)  ||                    // pow(1, any) = 1, even for NaN, 
             qd_zero_p(b))                                  // pow(any, 0) = 1, even for NaN
    {
        qd_set_d(r, 1.0);                       
    }
    else if (qd_inf_p(b))
    {
        if (qd_signbit(b))
        {
            if (qd_cmp_d(a, -1.0) > 0 && qd_cmp_d(a, 1.0) < 0)
                qd_set_inf(r, 1);   // pow(-1 < x < 1, -inf) = +inf
            else    
                qd_set_zero(r, 1);  // pow(abs(x) > 1, -inf) = +0
        }
        else
        {
            if (qd_cmp_d(a, -1.0) > 0 && qd_cmp_d(a, 1.0) < 0)
                qd_set_zero(r, 1);   // pow(-1 < x < 1, -inf) = +0
            else    
                qd_set_inf(r, 1);    // pow(abs(x) > 1, -inf) = +inf
        }  
    }
    else if (qd_inf_p(a))
    {
        if (qd_signbit(a))
        {
            if (qd_signbit(b))
            {
                if (qd_odd_int_p(b))    qd_set_zero(r, -1);     // pow(-inf, negative odd int) = -0
                else                    qd_set_zero(r, 1);      // pow(-inf, negative) = 0
            }
            else
            {
                if (qd_odd_int_p(b))    qd_set_inf(r, -1);      // pow(-inf, positive odd int) = -inf
                else                    qd_set_inf(r, 1);       // pow(-inf, positive) = +inf
            }
        }
        else
        {
            if (qd_signbit(b))  qd_set_zero(r, 1);  // pow(+inf, negative) = +0
            else                qd_set_inf(r, 1);   // pow(+inf, positive) = +inf
        }
    }
    else if (qd_signbit(a))
    {
        if (qd_int_p(b))
        {
            qd_t t;

            qd_abs(t, a);
            qd_pow(t, t, b);
            if (qd_odd_int_p(b))    qd_neg(r, t); // pow(-x, odd int) = -(x^(odd int))
            else                    qd_set(r, t); // pow(-x, even int) = x^(even int)
        }
        else
        {
            qd_set_nan(r); // pow(finite negative, finite non-integer) = NaN
        }
    }
    else
    {
        qd_t t, u, xn, la;

        qd_set_d(xn, pow(a->data[0], b->data[0]));
        qd_log(la, a);

        // Newton's method: x(k+1) = x(k) * (b*log(a) - log(x(k)) + 1)
        for (int i = 0; i < 3; ++i)
        {
            qd_log(u, xn);
            qd_sub_d(u, u, 1.0);
            qd_mul(t, b, la);
            qd_sub(t, t, u);
            qd_mul(xn, xn, t);
        }

        qd_set(r, xn);
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

int qd_cmpabs(qd_t a, qd_t b)
{
    qd_t neg;

    if (qd_signbit(a))
    {
        qd_neg(neg, b);
        return qd_cmp(a, neg);
    }
    else
    {
        return qd_cmp(a, b);
    }  
}

int qd_sgn(qd_t x)
{
    return (0.0 < x->data[0]) - (x->data[0] < 0.0);
}

int qd_zero_p(qd_t x)
{
    return (x->data[0] == 0.0);
}

int qd_nan_p(qd_t x)
{
    return isnan(x->data[0]);
}

int qd_inf_p(qd_t x)
{
    return isinf(x->data[0]);
}

int qd_number_p(qd_t x)
{
    return !isnan(x->data[0]) && !isinf(x->data[0]);
}

int qd_cmp_d(qd_t x, double d)
{
    qd_t y;
    qd_set_d(y, d);
    return qd_cmp(x, y);
}

// **** Conversion functions **** //

double qd_get_d(qd_t qd)
{
    return qd->data[0];
}

uint64_t qd_get_ui(qd_t qd)
{
    qd_t tmp, acc, lim;
    uint64_t r, t;

    r = 0;
    qd_set_ui(lim, UINT64_MAX);
    if (qd_signbit(qd) || qd_nan_p(qd))     return 0;
    if (qd_cmp(qd, lim) > 0)                return UINT64_MAX;

    // first pass, correct if first component is too large
    qd_set(tmp, qd);
    t = (tmp->data[0] == 1.8446744073709552e+19) ? UINT64_MAX : (uint64_t)tmp->data[0];         
    qd_set_ui(acc, t);
    qd_sub(tmp, tmp, acc);
    r += t;

    // second pass when (qd > 2^53)
    t = (uint64_t)tmp->data[0];      
    qd_set_ui(acc, t);
    qd_sub(tmp, tmp, acc);
    return r + t;
}

int64_t qd_get_si(qd_t qd)
{
    qd_t lo, hi, tmp;

    if (qd_nan_p(qd))   return 0;

    qd_set_si(lo, INT64_MIN);
    qd_set_si(hi, INT64_MAX);
    if (qd_cmp(qd, lo) < 0)     return INT64_MIN;
    if (qd_cmp(qd, hi) > 0)     return INT64_MAX;

    qd_abs(tmp, qd);
    return (int64_t)qd_get_ui(tmp) * qd_sgn(qd);
}

char* qd_get_str(qd_t qd, int len)
{
    qd_t tmp;
    char *out, *pstr, *pend;
    int digit, abs_exp, exp_len;
    int exp = 0;

    if (qd_nan_p(qd))
    {
        out = malloc(4 * sizeof(char));
        strncpy(out, "nan", 4);
        return out;
    }
    
    if (qd_inf_p(qd))
    {
        if (qd_signbit(qd))    
        {
            out = malloc(5 * sizeof(char));
            strncpy(out, "-inf", 5);
            return out;
        }
        else
        {
            out = malloc(4 * sizeof(char));
            strncpy(out, "inf", 4);
            return out;
        }
    }

    // normalize qd => x * 10^n where 1 <= x < 10
    qd_abs(tmp, qd);  
    if (qd_cmp(tmp, QD_TEN) >= 0) // qd >= 10
    {
        for (int p10 = 8; p10 >= 0; --p10)
        {
            if (qd_cmp(tmp, (QD_TEN + p10)) >= 0)
            {
                exp += powi(2, p10);
                qd_div(tmp, tmp, (QD_TEN + p10));
            }
        }
    }

    if (!qd_zero_p(tmp) && qd_cmp(tmp, QD_ONE) < 0)
    {
        for (int p10 = 9; p10 >= 0; --p10)
        {
            if (qd_cmp(tmp, (QD_ONE - p10)) < 0)
            {
                exp -= powi(2, p10 - 1);
                qd_div(tmp, tmp, (QD_ONE - p10));
            }
        }     
    }

    if (len == 0)           len = 66; // default string length
    if (qd_signbit(tmp))    ++len;

    abs_exp = abs(exp);
    if (abs_exp >= 100)     exp_len = 4; // exponent sign
    else                    exp_len = 3; // leading zero if needed  

    len += (exp_len + 2); // decimal point, exponent mark

    out = (char*)malloc(len);
    pstr = out;
    pend = out + len - 1;

    if (qd_signbit(qd))
    {
        *pstr = '-';
        ++pstr;
    }

    // round down if next significand negative and current significand is exact integer
    digit = (int)tmp->data[0];
    if (((double)digit == tmp->data[0]) && (tmp->data[1] < 0.0)) --digit;
    *pstr = (char)digit + '0';
    ++pstr;

    qd_sub_d(tmp, tmp, (double)digit);
    qd_mul_d(tmp, tmp, 10.0);
    *pstr = '.';
    ++pstr;

    while (!qd_zero_p(tmp) && (pstr - out) < (len - exp_len - 2))
    {
        digit = (int)tmp->data[0];
        if (digit == 10.0 && tmp->data[1] < 0)  --digit;
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

int qd_signbit(qd_t qd)
{
    return (qd->data[0] < 0.0);
}

int qd_int_p(qd_t qd)
{
    // qd is an integer if every component is an integer
    return (qd->data[0] == trunc(qd->data[0]) && qd->data[1] == trunc(qd->data[1]) &&
            qd->data[2] == trunc(qd->data[2]) && qd->data[3] == trunc(qd->data[3]));
}

int qd_odd_int_p(qd_t qd)
{
    if (!qd_int_p(qd))  return false;

    for (int i = 0; i < 4; ++i)
    {
        if ((qd->data[i] <= 2.251799813685248e15 && qd->data[i] >= 1) ||
            (qd->data[i] <= -1 && qd->data[i] >= -2.251799813685248e15))
        {
            return ((int64_t) qd->data[i]) % 2;
        }
    }

    return 1;
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
    qd_set_zero(qd, 1);
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