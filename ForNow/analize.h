#ifndef ANALIZE_H

#define ANALIZE_H

class Analizing
{
public:
///review20170727 как можно больше избегать указателей и использовать стандартные классы коллекций qt
/// везде где можно и имеет смысл для аргумента делать const& то делать const&, узнать что такое константные ссылки
    Analizing(double *ryad_in);
    int rsize;
    double *RangeIn;
    double Deviation;
    double Middle;
    double CoefVar;


///review20170727 никаких имён на транслите
/// все имена в верблюжей нотации
/// имена классов с большой буквы
/// име методов и полей с маленькой
/// для private полей суффикс _, например, int privateField_;
/// всё что можно сделать private делать private
/// всё что можно сделать const делать const
    void Midle();
    void St_Otklonenie();
    void Koef_Variazii();
};
#endif // ANALIZE_H
