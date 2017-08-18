#ifndef TIMESERIES_H

#define TIMESERIES_H

#include <QtCore>

class TimeSeries : public QList<double>
{
public:
    TimeSeries()
    {

    }

    TimeSeries(const QString &id) :
        id_(id)
    {

    }

    TimeSeries setId(const QString &id)
    {
        id_=id;
    }

    TimeSeries fromJson( const QJsonArray &jarray);

    QString id() const
    {
        return id_;
    }

    TimeSeries& operator <<(const double elem)
    {
        append(elem);
        return *this;
    }
    TimeSeries& operator =(const QList<double> list)
    {
        foreach (double element, list) {
            append(element);
        }
        return *this;
    }
    bool operator ==(const TimeSeries &series) const;


    QString toString() const;

private:
    QString id_;
};

Q_DECLARE_METATYPE(TimeSeries)

#endif // TIMESERIES_H
