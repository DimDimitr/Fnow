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

    //insert list of timeSeries
    void insertIntoTableFromOriginalType(const QList<TimeSeries> &tsl);

    //return list of value with input id
    QList<TimeSeries> timeSeriesFromString(const QList<QString> &id);

    //clear datBase
    static bool clear(const QString &databaseName);

    //return all names
    QList<QString> fetchAllIDs(QList<QString> names);

private:
    //main object in class
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
