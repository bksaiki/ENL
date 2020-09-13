#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../include/common.h"
#include "../include/qd.h"

#define _GNU_SOURCE

void log_test(const char* str)
{
    printf("FAILED: %s\n", str);
}

int main()
{
    bool status = true;
    bool res;

    {
        double x, x0, x1;
        
        x = M_PI;
        split_f64(x, &x0, &x1);
        res = (x == (x0 + x1));
        if (!res) printf("FAILED \"split_f64\": %.17g != %.17g + %.17g\n", x, x0, x1);
        status &= res;
    }
    
    return (int)(!status);
}