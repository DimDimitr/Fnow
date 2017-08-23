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
TimeSeries results;
}

QString TimeSeries::id() const
{
    return id_;
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

TimeSeries& TimeSeries::operator =(const QList<double> list)
{
    foreach (const double element, list) {
        append(element);
    }
    return *this;
}
