#include <stdio.h>
#include <stdlib.h>
#include "../include/qd.h"

int main(int argc, char** argv) 
{
    qd_t accum, t0, t1, t2, t3, coeff, c, s, p16, one, two, four;
    int iter;

    if (argc == 1)  iter = 60;
    else            iter = atoi(argv[1]);

    if (iter < 0)
    {
        printf("Cannot take negative iterations: %i. Changing to 60.\n", iter);
        iter = 60;
    }
    else if (iter > 250)
    {
        printf("Capped at 60 iterations (max precision reached): %i. Limiting to 60.\n", iter);
        iter = 60;
    }

    printf("Calculating pi with quad-double precision using 1997 Bailey, Borwein, Plouffe algorithm w/ %i iterations\n", iter);
    
    qd_set_d(one, 1.0);
    qd_set_d(two, 2.0);
    qd_set_d(four, 4.0);
    qd_set_zero(accum, 1);

    for (int i = 0; i < iter; ++i)
    {
        qd_set_d(c, (double)i);
        qd_mul_d(coeff, c, 8.0);

        if (i == 0)         qd_set_d(p16, 1.0);
        else if (i == 1)    qd_set_d(p16, 16.0);
        else                qd_mul_d(p16, p16, 16.0);

        qd_add_d(t0, coeff, 1.0);
        qd_div(t0, four, t0);
        
        qd_add_d(t1, coeff, 4.0);
        qd_div(t1, two, t1);
        
        qd_add_d(t2, coeff, 5.0);
        qd_div(t2, one, t2);

        qd_add_d(t3, coeff, 6.0);
        qd_div(t3, one, t3);

        qd_sub(s, t0, t1);
        qd_sub(s, s, t2);
        qd_sub(s, s, t3);

        qd_div(coeff, one, p16);
        qd_mul(s, s, coeff);
        qd_add(accum, accum, s);
    }

    if (accum->data[0] == 3.1415926535897931 && accum->data[1] == 1.2246467991473532e-16 &&
        accum->data[2] == -2.994769809718341e-33 && accum->data[3] == 5.2289406217977859e-50)
    {
        printf("pi = %.17g + %.17g + %.17g + %.17g\n", accum->data[0], accum->data[1], accum->data[2], accum->data[3]);
        return 0;
    }
    else
    {
        printf("FAILED \"pi\": %.17g + %.17g + %.17g + %.17g\n", accum->data[0], accum->data[1], accum->data[2], accum->data[3]);
        return 1;
    }
    
}