#ifndef TIMESERIES_H

#define TIMESERIES_H

#include <QtCore>

typedef QString TimeSeriesID;

//class time seris - named list
class TimeSeries : public QList<double>
{
public:
    TimeSeries();

    TimeSeries(const QString &id);

    //set name for TimeSeries
    void setId(const QString &id);

    //get TimeSeries from fromJson
    TimeSeries fromJson( const QJsonArray &jarray);

    //return name of TimeSeries
    QString id() const;

    TimeSeries& operator <<(const double elem);

    TimeSeries& operator =(const QList<double> list);

    bool operator ==(const TimeSeries &series) const;

    //return string from this
    QString toString() const;

private:
    QString id_;
};

Q_DECLARE_METATYPE(TimeSeries)

#endif // TIMESERIES_H
