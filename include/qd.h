#ifndef _ENL_QD_H_
#define _ENL_QD_H_

#include <stdint.h>
#include "common.h"

struct qd_struct_t
{
    double data[4];
};

typedef struct qd_struct_t qd_t[1];

// **** Initialization functions **** //

void qd_set(qd_t qd, qd_t x);
void qd_set_d(qd_t qd, double x);
void qd_set_ui(qd_t qd, uint64_t x);
void qd_set_si(qd_t qd, int64_t x);
void qd_set_str(qd_t qd, char* str);

void qd_set_nan(qd_t qd);
void qd_set_inf(qd_t qd, int sign);
void qd_set_zero(qd_t qd, int sign);

// **** Arithmetic operators **** //

void qd_neg(qd_t r, qd_t x);
void qd_abs(qd_t r, qd_t x);

void qd_add_d(qd_t r, qd_t a, double b);
void qd_sub_d(qd_t r, qd_t a, double b);
void qd_mul_d(qd_t r, qd_t a, double b);
void qd_div_d(qd_t r, qd_t a, double b);

void qd_add(qd_t r, qd_t a, qd_t b);
void qd_sub(qd_t r, qd_t a, qd_t b);
void qd_mul(qd_t r, qd_t a, qd_t b);
void qd_div(qd_t r, qd_t a, qd_t b);

void qd_powi(qd_t r, qd_t a, int b);
void qd_sqrt(qd_t r, qd_t a);
void qd_nroot(qd_t r, qd_t a, int n);

void qd_exp(qd_t r, qd_t a);
void qd_log(qd_t r, qd_t a);

// **** Comparison functions **** //

int qd_cmp(qd_t a, qd_t b);
int qd_cmpabs(qd_t a, qd_t b);
int qd_sgn(qd_t x);

int qd_zero_p(qd_t x);
int qd_nan_p(qd_t x);
int qd_inf_p(qd_t x);
int qd_number_p(qd_t x);

// **** Conversion functions **** //

double qd_get_d(qd_t qd);
uint64_t qd_get_ui(qd_t qd);
int64_t qd_get_si(qd_t qd);
char* qd_get_str(qd_t qd, int len);

// **** Miscellaneous functions **** //

int qd_signbit(qd_t qd);

void qd_renormalize(qd_t qd, double a0, double a1, double a2, double a3, double a4);

// **** Constants **** //

extern qd_t QD_E,
            QD_LOG2E,
            QD_LOG10E,
            QD_LN2,
            QD_LN10,
            QD_PI,
            QD_PI_2,
            QD_PI_4,
            QD_1_PI,
            QD_2_PI,
            QD_2_SQRTPI,
            QD_SQRT2,
            QD_SQRT1_2;

#endif