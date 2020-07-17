#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include "../src/common.h"

#define _GNU_SOURCE

void log_test(const char* str)
{
    printf("FAILED: %s\n", str);
}

int main()
{
    bool status = true;

    {
        double x, x0, x1;
        bool res;
        
        x = M_PI;
        split_f64(x, &x0, &x1);
        res = (x == (x0 + x1));
        if (!res) printf("FAILED \"split_f64\": %.17g != %.17g + %.17g\n", x, x0, x1);
        status &= res;
    }

    double a, b;
    split_f64(3.0, &a, &b);

    return (int)(!status);
}