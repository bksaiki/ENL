#include <stdio.h>
#include "src/quad-double.h"

int main() 
{
    struct qd_t acc = to_qd(1.0);
    double f64 = 1.0;
    
    for (int i = 0; i < 10000000; ++i)
    {
        acc = mul_qd_d(&acc, 1.000001);
        f64 *= 1.000001;
    }

    printf("quad-double: %.17g + %.17g + %.17g + %.17g\n", acc.data[0], acc.data[1], acc.data[2], acc.data[3]);
    printf("quad-double (double approx): %.17g\n", acc.data[0]);
    printf("double: %.17g\n", f64);

    return 0;
}