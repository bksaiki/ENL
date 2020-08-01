#ifndef _FLOATTYPES_QUAD_DOUBLE_H_
#define _FLOATTYPES_QUAD_DOUBLE_H_

#include "common.h"

struct qd_struct_t
{
    double data[4];
};

typedef struct qd_struct_t qd_t[1];

// **** Initialization functions **** //

void qd_set(qd_t qd, qd_t x);
void qd_set_d(qd_t qd, double x);
void qd_set_zero(qd_t qd);
void qd_set_str(qd_t qd, char* str);

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

// **** Comparison functions **** //

int qd_cmp(qd_t a, qd_t b);
int qd_sgn(qd_t x);

int qd_is_zero(qd_t x);
int qd_is_nan(qd_t x);
int qd_is_inf(qd_t x);
int qd_is_number(qd_t x);

// **** I/O functions **** //

char* qd_to_str(qd_t qd, int len);

// **** Miscellaneous functions **** //

void qd_renormalize(qd_t qd, double a0, double a1, double a2, double a3, double a4);

#endif