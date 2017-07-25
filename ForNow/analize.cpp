#include "analize.h"
#include <qmath.h>


Analizing::Analizing(double *ryad_in)
{
razm=sizeof(ryad_in);
double temp;
for(int i = 0; i<razm;i++)
{
    for(int j = 0; j<razm-i-1;i++)
        if(ryad_in[j] > ryad_in[j + 1])
        {
            temp = ryad_in[j];
            ryad_in[j] = ryad_in[j + 1];
            ryad_in[j + 1] = temp;
        }
}
Middle();
St_otklonenie();
Koef_variazii();
}


void Analizing::Middle()
{
 double summ=0;
 for(int i=0;i<razm;i++)
 {
     summ+=ryad_in[i];
 }
 middle=summ/razm;
}


void Analizing::St_otklonenie()
{
    double middle_of_arr=ryad_in[int(razm/2)];
    double summ=0;
    for (int i=0;i<razm;i++)
    {
        summ+=pow((ryad_in[i]-middle_of_arr),2);
    }
    summ/=razm-1;
    otklon= qSqrt(summ);
}


void Analizing::Koef_variazii()
{
  koef_var=  otklon/middle;
}
