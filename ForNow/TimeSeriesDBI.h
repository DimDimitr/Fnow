#ifndef TIMESERIESDBI_H

#define TIMESERIESDBI_H

#include "Analyzer.h"

#include <QtCore>
#include <QtTest/QTest>



class TimeSeriesDBI
{
public:
    TimeSeriesDBI(const QString)
    {

    }

    void write(const TimeSeries &ts)
    {
        storage_.insert(ts.id(), ts);
    }

    TimeSeries read(const QString &id)
    {
        return storage_.value(id, TimeSeries(id));
    }

    static bool clear(const QString &databaseName)
    {
        storage_.clear();
        return true;
    }

    QList<QString> fetchAllIDs(QList<QString> names)
    {
        return names;
    }
    
private:

    /*****************************************/

    static QHash<QString, TimeSeries> storage_;

    /*****************************************/

};

class TTimeSeriesDBI : public QObject
{
    Q_OBJECT
private slots:
    void TestWriteReadRewrite_data();
    void TestWriteReadRewrite();
};

#endif // TIMESERIESDBI_H
