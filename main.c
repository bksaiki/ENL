#include <stdio.h>
#include "src/quad-double.h"

int main() 
{
    struct qd_t acc = to_qd(0.0);
    struct qd_t acc2 = to_qd(0.0);
    struct qd_t step = to_qd(0.1);
    double f64 = 0.0;
    
    for (int i = 0; i < 10000000; ++i)
    {
        acc = add_qd_d(&acc, 0.1);
        acc2 = add_qd2(&acc2, &step);
        f64 += 0.1;
    }

    printf("quad-double: %.17g + %.17g + %.17g + %.17g\n", acc.data[0], acc.data[1], acc.data[2], acc.data[3]);
    printf("quad-double (double approx): %.17g\n", acc.data[0]);
    printf("quad-double: %.17g + %.17g + %.17g + %.17g\n", acc2.data[0], acc2.data[1], acc2.data[2], acc2.data[3]);
    printf("quad-double (double approx): %.17g\n", acc2.data[0]);
    printf("double: %.17g\n", f64);

    return 0;
}