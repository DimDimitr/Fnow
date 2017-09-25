#ifndef TIMESERIES_H

#define TIMESERIES_H

#include <QtCore>

typedef QString TimeSeriesID;

//class time seris - named list
class TimeSeries
{
public:
    TimeSeries();

    TimeSeries(const QString &id);

    TimeSeries(const QString &id, const QMap<int, double> data);

    //set name for TimeSeries
    void setId(const QString &id);

    void setData(const QMap<int, double> data);

    //get TimeSeries from fromJson
    TimeSeries fromJson( const QJsonArray &jarray);

    TimeSeries append(const int elem);

    TimeSeries insert(const int point, double value);

//    TimeSeries append(const double elem);

    //return name of TimeSeries
    QString id() const;

    int lastKey() const;

    QList<int> keys() const;

    bool isEmpty() const;

    int size() const;

    double last() const;

    QMap<int, double> getData();

    double value(const int key) const;

    QList<double> values() const;

    TimeSeries& operator <<(const double elem);
    TimeSeries& operator <<(const TimeSeries &elems);
    //TimeSeries& operator <<(const QList<double> &elems);

//    TimeSeries& operator =(const QList<double> list);

    TimeSeries& operator = (const QMap<int, double> list);

    bool operator ==(const TimeSeries &series) const;
    bool operator <(const TimeSeries &series) const;

    //return string from this
    QString toString() const;

private:
    QString id_;
    QMap<int, double> data_;
};

Q_DECLARE_METATYPE(TimeSeries)

inline QDebug operator<<(QDebug debug, const TimeSeries &ts)
{
    debug << qPrintable(ts.toString());
    return debug;
}

#endif // TIMESERIES_H
