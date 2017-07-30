#include "analize.h"

///review20170727 всё что можно инклудить в cpp инклудить в cpp
///review20170727 порядок инклудов:
/// свой h файл
/// файлы от qt
/// внешние h файлы
/// внутренние h файлы

#include <qmath.h>


Analizing::Analizing(double *range_in)
{
    razm=sizeof(range_in);
    double temp;
    for(int i = 0; i<razm;i++)
    {
        for(int j = 0; j<razm-i-1;i++)
            if(range_in[j] > range_in[j + 1])
            {
                temp = range_in[j];
                range_in[j] = range_in[j + 1];
                range_in[j + 1] = temp;
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
        summ+=range_in[i];
    }
    middle=summ/razm;
}


void Analizing::St_otklonenie()
{
    double middle_of_arr=range_in[int(razm/2)];
    double summ=0;
    for (int i=0;i<razm;i++)
    {
        summ+=pow((range_in[i]-middle_of_arr),2);
    }
    summ/=razm-1;
    otklon= qSqrt(summ);
}


void Analizing::Koef_variazii()
{
    koef_var=  otklon/middle;
}
