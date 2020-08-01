#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../src/common.h"
#include "../src/quad-double.h"

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

    {
        qd_t qd;
        char *istr, *ostr;
        
        istr = "3.1415926535897932384626433832795028841971693993751058209749445923e+00";
        qd_set_str(qd, istr);
        ostr = qd_to_str(qd, 0);
        res = (strcmp(istr, ostr) == 0);
        if (!res) printf("FAILED \"qd I/O\": %s != %s\n", istr, ostr);
        status &= res;
    }
    
    return (int)(!status);
}