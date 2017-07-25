#ifndef ANALIZE_H
#define ANALIZE_H

class Analizing
{
public:
    Analizing(double *ryad_in);
    int razm;
    double *ryad_in;
    double otklon;
    double middle;
    double koef_var;
    void Middle();
    void St_otklonenie();
    void Koef_variazii();
};
#endif // ANALIZE_H
