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

    void setId(const QString &id);

    TimeSeries fromJson( const QJsonArray &jarray);

    QString id() const;

    TimeSeries& operator <<(const double elem);

    TimeSeries& operator =(const QList<double> list);

    bool operator ==(const TimeSeries &series) const;


    QString toString() const;

private:
    QString id_;
};

Q_DECLARE_METATYPE(TimeSeries)

#endif // TIMESERIES_H
