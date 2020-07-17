#ifndef _FLOATTYPES_QUAD_DOUBLE_H_
#define _FLOATTYPES_QUAD_DOUBLE_H_

#include "common.h"

struct qd_t
{
    double data[4];
};

// **** Initialization functions **** //

struct qd_t to_qd(double x);

struct qd_t to_qd4(double a0, double a1, double a2, double a3);

void zero_qd(struct qd_t* x);

// **** Arithmetic operators **** //

struct qd_t neg_qd(struct qd_t* x);

struct qd_t add_qd_d(struct qd_t* a, double b);

struct qd_t sub_qd_d(struct qd_t* a, double b);

struct qd_t mul_qd_d(struct qd_t* a, double b);

struct qd_t add_qd2(struct qd_t* a, struct qd_t* b);

struct qd_t sub_qd2(struct qd_t* a, struct qd_t* b);





// **** Miscellaneous **** //

struct qd_t renormalize_qd(double a0, double a1, double a2, double a3, double a4);

#endif