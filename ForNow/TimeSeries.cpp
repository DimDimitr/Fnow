#include "TimeSeries.h"

QString TimeSeries :: toString() const
{
    QStringList cells;
    foreach(const double elem, *this)
    {
        cells << QString::number(elem);
    }

    return "TimeSeries{id: " + id_ + ", data: " + cells.join(", ") + "}";
}

TimeSeries TimeSeries::fromJson(const QJsonArray &jarray)
{
    Q_UNUSED(jarray);
    return TimeSeries();
}

QString TimeSeries::id() const
{
    return id_;
}

TimeSeries::TimeSeries()
{

}

TimeSeries::TimeSeries(const QString &id) :
    id_(id)
{

}

void TimeSeries::setId(const QString &id)
{
    id_ = id;
}

TimeSeries& TimeSeries::operator <<(const double elem)
{
    append(elem);
    return *this;
}

TimeSeries &TimeSeries::operator <<(const QList<double> &elems)
{
    append(elems);
    return *this;
}

TimeSeries& TimeSeries::operator =(const QList<double> list)
{
    foreach (const double element, list)
    {
        append(element);
    }
    return *this;
}

