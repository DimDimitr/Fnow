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
