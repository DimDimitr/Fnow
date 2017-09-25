#include "TimeSeries.h"

QString TimeSeries :: toString() const
{
    QStringList cells;
    foreach(const int point, keys())
    {
        cells << ("(" + QString::number(point) + "," + QString::number(value(point)) + ")");
    }
    return "TimeSeries{id: " + id_ + ", data: " + cells.join(", ") + "}";
}

TimeSeries TimeSeries::fromJson(const QJsonArray &jarray)
{
    Q_UNUSED(jarray);
    return TimeSeries();
}

int TimeSeries::lastKey() const
{
    return data_.lastKey();
}

QList<int> TimeSeries::keys() const
{
    return data_.keys();
}

bool TimeSeries::isEmpty() const
{
    return data_.isEmpty();
}

int TimeSeries::size() const
{
    return data_.size();
}

double TimeSeries::value(const int key) const
{
    return data_.value(key);
}

QList<double> TimeSeries::values() const
{
    return data_.values();
}

double TimeSeries::last() const
{
    return data_.last();
}


QMap<int, double> TimeSeries::getData()
{
    return data_;
}

//int TimeSeries::end() const
//{
//    return data_.end();
//}

TimeSeries TimeSeries::append(const int elem)
{
    int size;
    if (!isEmpty())
    {
        size = lastKey();
    }
    else
    {
        size = 0;
    }
    data_.insert(size+1, elem);
    return *this;
}

TimeSeries TimeSeries::insert(const int point, double value)
{
    data_.insert(point, value);
    return *this;
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

TimeSeries::TimeSeries(const QString &id, const QMap<int, double> data) :
    id_(id),
    data_(data)
{

}

void TimeSeries::setId(const QString &id)
{
    id_ = id;
}

void TimeSeries::setData(const QMap<int, double> data)
{
    data_ = data;
}

TimeSeries& TimeSeries::operator <<(const double elem)
{
    int size;
    if (!isEmpty())
    {
        size = lastKey();
    }
    else
    {
        size = 0;
    }
    data_.insert(size + 1, elem);
    return *this;
}

TimeSeries& TimeSeries::operator <<(const TimeSeries &elems)
{
    data_.unite(elems.data_);
    return *this;
}

TimeSeries& TimeSeries::operator = (const QMap<int, double> list)
{
    data_ = list;
//    foreach (const double element, list)
//    {
//        append(element);
//    }
    return *this;
}

