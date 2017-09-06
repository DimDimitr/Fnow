#ifndef DATAINMEMMORYMOC_H

#define DATAINMEMMORYMOC_H

#include <QtCore>

#include <QtTest/QTest>

#include "TimeSeriesDBI.h"

class DataInMemmoryMoc : public TimeSeriesDBI
{
public:
    DataInMemmoryMoc(const QString);

    DataInMemmoryMoc();

    //insert list of timeSeries
    void insertIntoTableFromOriginalType(const QList<TimeSeries> &tsl);

    //return list of value with input id
    QList<TimeSeries> timeSeriesFromString(const QList<QString> &id);

    //clear datBase
    static bool clear(const QString &databaseName);

    //return all names
    QList<QString> fetchAllIDs(QList<QString> names);

//*************

    virtual void loadDataFromFile(const QString &path)
    {
        Q_UNUSED(path);
    }

    //запись временных рядов из ts в базу
    virtual void write(const TimeSeriesList &ts)
    {
        insertIntoTableFromOriginalType(ts);
    }

    //чтение временных рядов из базы
    virtual TimeSeriesList read(const QList<TimeSeriesID> &ids)
    {
        return timeSeriesFromString(ids);
    }

    //удаление базы с именем databaseName
    virtual bool remove(const QString &databaseName)
    {
        return clear(databaseName);
    }

    //открытие и получение указателя на интерфейс для базы с именем databaseName
    virtual TimeSeriesDBI* open(const QString &databaseName)
    {
        return new DataInMemmoryMoc(databaseName);
    }

    virtual TimeSeriesStream* stream(const QList<TimeSeriesID> &ids)
    {
        return new TimeSeriesStream(ids);
    }

private:
    //main object in class
    static QHash<QString, TimeSeries> storage_;
};


class TDataInMemmoryMoc : public QObject
{
};

#endif // DATAINMEMMORYMOC_H
