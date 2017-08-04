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
    rsize=sizeof(range_in);
    double temp;
    for(int i = 0; i<rsize;i++)
    {
        for(int j = 0; j<rsize-i-1;i++)
            if(range_in[j] > range_in[j + 1])
            {
                temp = range_in[j];
                range_in[j] = range_in[j + 1];
                range_in[j + 1] = temp;
            }
    }
    Midle();
    St_Otklonenie();
    Koef_Variazii();
}


void Analizing::Midle()
{
    double summ=0;
    for(int i=0;i<rsize;i++)
    {
        summ+=RangeIn[i];
    }
    Middle=summ/rsize;
}


void Analizing::St_Otklonenie()
{
    double middle_of_arr=RangeIn[int(rsize/2)];
    double summ=0;
    for (int i=0;i<rsize;i++)
    {
        summ+=pow((RangeIn[i]-middle_of_arr),2);
    }
    summ/=rsize-1;
    Deviation= qSqrt(summ);
}


void Analizing::Koef_Variazii()
{
    CoefVar=  Deviation/Middle;
}
