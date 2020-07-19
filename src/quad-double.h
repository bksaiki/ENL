#ifndef _FLOATTYPES_QUAD_DOUBLE_H_
#define _FLOATTYPES_QUAD_DOUBLE_H_

#include "common.h"

struct qd_struct_t
{
    double data[4];
};

typedef struct qd_struct_t qd_t[1];

// **** Initialization functions **** //

void qd_init(qd_t qd);

void qd_set_d(qd_t qd, double x);
void qd_set_4d(qd_t qd, double a0, double a1, double a2, double a3);

void qd_set_zero(qd_t qd);

// **** Arithmetic operators **** //

void qd_neg(qd_t r, qd_t x);

void qd_add_d(qd_t r, qd_t a, double b);
void qd_sub_d(qd_t r, qd_t a, double b);
void qd_mul_d(qd_t r, qd_t a, double b);

void qd_add(qd_t r, qd_t a, qd_t b);
void qd_sub(qd_t r, qd_t a, qd_t b);
void qd_mul(qd_t r, qd_t a, qd_t b);
void qd_div(qd_t r, qd_t a, qd_t b);

// **** Miscellaneous **** //

void qd_renormalize(qd_t qd, double a0, double a1, double a2, double a3, double a4);

#endif