#ifndef DATAINMEMMORYMOC_H
#define DATAINMEMMORYMOC_H

#include "Analyzer.h"

#include <QtCore>
#include <QtTest/QTest>



class DataInMemmoryMoc
{
public:
    DataInMemmoryMoc(const QString)
    {

    }

    void insertIntoTableFromOriginalType(const QList<TimeSeries> &tsl)
    {
        foreach (const TimeSeries ts,tsl)
        {
        storage_.insert(ts.id(), ts);
        }
        //qWarning() << storage_.size();
    }

    QList<TimeSeries> timeSeriesFromString(const QList<QString> &id)
    {
        QList<TimeSeries> result;
        foreach (QString oneId, id)
        {
           result.append(storage_.value(oneId, TimeSeries(oneId)));
        }
        return result;
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
    static QHash<QString, TimeSeries> storage_;
};

class TDataInMemmoryMoc : public QObject
{
    Q_OBJECT
private slots:
    void TestWriteReadRewrite_data();
    void TestWriteReadRewrite();
};

#endif // DATAINMEMMORYMOC_H
