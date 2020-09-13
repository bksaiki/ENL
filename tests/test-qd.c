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

    {   // I/O
        qd_t qd;
        char *istr, *ostr;
        
        istr = "3.1415926535897932384626433832795028841971693993751058209749445923e+00";
        qd_set_str(qd, istr);
        ostr = qd_get_str(qd, 0);
        res = (strcmp(istr, ostr) == 0);
        if (!res) printf("FAILED \"qd I/O\": %s != %s\n", istr, ostr);
        status &= res;
    }

    {   // 4 function inf/nan
        qd_t a, b, c, d, r;
        char* str;

        qd_set_nan(a);
        qd_set_zero(b, 1);
        qd_set_inf(c, 1);
        qd_set_inf(d, -1);

        qd_add_d(r, a, 0);
        str = qd_get_str(r, 0);
        res = (strcmp(str, "nan") == 0);
        if (!res) printf("FAILED \"qd inf/nan edgecases 1\": %s != nan\n", str);
        free(str);

        qd_add_d(r, b, NAN);
        str = qd_get_str(r, 0);
        res = (strcmp(str, "nan") == 0);
        if (!res) printf("FAILED \"qd inf/nan edgecases 2\": %s != nan\n", str);
        free(str);

        qd_add_d(r, c, 0);
        str = qd_get_str(r, 0);
        res = (strcmp(str, "inf") == 0);
        if (!res) printf("FAILED \"qd inf/nan edgecases 3\": %s != inf\n", str);
        free(str);

        qd_add_d(r, b, INFINITY);
        str = qd_get_str(r, 0);
        res = (strcmp(str, "inf") == 0);
        if (!res) printf("FAILED \"qd inf/nan edgecases 4\": %s != inf\n", str);
        free(str);

        qd_add_d(r, c, INFINITY);
        str = qd_get_str(r, 0);
        res = (strcmp(str, "inf") == 0);
        if (!res) printf("FAILED \"qd inf/nan edgecases 5\": %s != inf\n", str);
        free(str);
        
        qd_add_d(r, c, -INFINITY);
        str = qd_get_str(r, 0);
        res = (strcmp(str, "nan") == 0);
        if (!res) printf("FAILED \"qd inf/nan edgecases 6\": %s != nan\n", str);
        free(str);
    }

    {   // 4-function
        qd_t r, x, y;
        char* str;
        bool res;

        qd_set(x, QD_PI);
        qd_set(y, QD_E);
        
        qd_add(r, x, y);
        res = (r->data[0] == 5.8598744820488387 && r->data[1] == -1.7705984076240228e-16 &&
               r->data[2] == -5.1224869177565164e-33 && r->data[3] == 2.6280843807045844e-49);
        str = qd_get_str(r, 0);
        if (!res) printf("FAILED \"PI + E\": %s\n", str);
        free(str);

        qd_sub(r, x, y);
        res = (r->data[0] == 0.42331082513074803 && r->data[1] == -2.2100009258189695e-17 &&
               r->data[2] == -8.670527016801629e-34 && r->data[3] == -4.0317593897785386e-50);
        str = qd_get_str(r, 0);
        if (!res) printf("FAILED \"PI - E\": %s\n", str);
        free(str);

        qd_mul(r, x, y);
        res = (r->data[0] == 8.5397342226735677 && r->data[1] == -6.7738152905024243e-16 &&
               r->data[2] == 1.6082340642907152e-32 && r->data[3] == -1.1166810263562522e-48);
        str = qd_get_str(r, 0);
        if (!res) printf("FAILED \"PI * E\": %s\n", str);
        free(str);

        qd_div(r, x, y);
        res = (r->data[0] == 1.1557273497909217 && r->data[1] == -1.3998972600526045e-17 &&
               r->data[2] == 2.1870490326442286e-34 && r->data[3] == 2.1272253359624188e-50);
        str = qd_get_str(r, 0);
        if (!res) printf("FAILED \"PI / E\": %s\n", str);
        free(str);
    }

    {   // sqrt
        qd_t r, x, y;
        char* str;
        bool res;

        qd_set(x, QD_PI);
        qd_set_d(y, 1e100);

        qd_sqrt(r, x);
        res = (r->data[0] == 1.7724538509055161 && r->data[1] == -7.6665864998257987e-17 &&
               r->data[2] == -1.3058334907945429e-33 && r->data[3] == -2.6110142087827155e-50);
        str = qd_get_str(r, 0);
        if (!res) printf("FAILED \"sqrt(PI)\": %s\n", str);
        free(str);

        qd_sqrt(r, y);
        res = (r->data[0] == 1.0000000000000001e+50 && r->data[1] == -6.8346252856038912e+33 &&
               r->data[2] == 48263607927321032 && r->data[3] == 1.4598009349434045);
        str = qd_get_str(r, 0);
        if (!res) printf("FAILED \"sqrt(1e100)\": %s\n", str);
        free(str);

    }

    {   // exp
        qd_t r, x, y;
        char* str;
        bool res;

        qd_set(x, QD_PI);
        qd_set_d(y, 100.0);

        qd_exp(r, x);
        res = (r->data[0] == 23.14069263277927 && r->data[1] == -1.3488747091995788e-15 &&
               r->data[2] == -2.9023466929581103e-32 && r->data[3] == 1.4929357911022652e-48);
        str = qd_get_str(r, 0);
        if (!res) printf("FAILED \"e^PI\": %s\n", str);
        free(str);

        qd_exp(r, y);
        res = (r->data[0] == 2.6881171418161356e+43 && r->data[1] == -1.6101271449201627e+27 &&
               r->data[2] == -101381290641.22626 && r->data[3] == -7.5336605839144333e-07);
        str = qd_get_str(r, 0);
        if (!res) printf("FAILED \"e^100\": %s\n", str);
        free(str);
    }

    {   // log
        qd_t r, x, y;
        char* str;
        bool res;

        qd_set(x, QD_PI);
        qd_set_d(y, 1e100);

        qd_log(r, x);
        res = (r->data[0] == 1.1447298858494002 && r->data[1] == 1.0265951162707826e-17 &&
               r->data[2] == -1.3722612652165766e-34 && r->data[3] == 1.054882475902608e-50);
        str = qd_get_str(r, 0);
        if (!res) printf("FAILED \"ln(PI)\": %s\n", str);
        free(str);

        qd_log(r, y);
        res = (r->data[0] == 230.25850929940458 && r->data[1] == -1.1033518306311232e-14 &&
               r->data[2] == -4.5381475487706792e-32 && r->data[3] == -2.4149222729490046e-48);
        str = qd_get_str(r, 0);
        if (!res) printf("FAILED \"ln(1e100)\": %s\n", str);
        free(str);
    }

    {   // pow
        qd_t r, x, y;
        char* str;
        bool res;

        qd_set(x, QD_PI);
        qd_set(y, QD_E);

        qd_pow(r, x, y);
        res = (r->data[0] == 22.459157718361045 && r->data[1] == 7.8681027359723335e-16 &&
               r->data[2] == 3.2133972257501365e-32 && r->data[3] == 2.5910742757774823e-49);
        str = qd_get_str(r, 0);
        if (!res) printf("FAILED \"PI^E\": %s\n", str);
        free(str);

        qd_pow(r, y, x);
        res = (r->data[0] == 23.14069263277927 && r->data[1] == -1.3488747091995788e-15 &&
               r->data[2] == -2.9023466929581103e-32 && r->data[3] == 1.4929357911019963e-48);
        str = qd_get_str(r, 0);
        if (!res) printf("FAILED \"E^PI\": %s\n", str);
        free(str);
    }
}