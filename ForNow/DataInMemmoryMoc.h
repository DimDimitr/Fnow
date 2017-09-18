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

    TimeSeries timeSeriesFromQMap(const TimeSeriesID &strJsonValue, QMap <int, double> mapTS);

    virtual TimeSeries fetchTimeSeriesFromQuery(QSqlQuery *query);

    //clear datBase
    static bool clear(const QString &databaseName);

    //return all names
    QList<QString> fetchAllIDs(QList<QString> names);

    //*************

    virtual void loadDataFromFile(const QString &path);

    //запись временных рядов из ts в базу
    virtual void write(const TimeSeriesList &ts);

    //чтение временных рядов из базы
    virtual TimeSeriesList read(const QList<TimeSeriesID> &ids);

    //удаление базы с именем databaseName
    virtual bool remove(const QString &databaseName);

    void nextID()
    {
        listOfId_.removeFirst();
        currentID_ = listOfId_.first();
    }

    //открытие и получение указателя на интерфейс для базы с именем databaseName
    virtual TimeSeriesDBI* open(const QString &databaseName);

    virtual TimeSeriesStream* stream(const QList<TimeSeriesID> &ids)
    {
        foreach (TimeSeriesID id, ids)
        {
            listOfId_.append(id);
        }
        currentID_ = listOfId_.first();
        qWarning() << listOfId_;
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(":memory:");
        db.open();
        QSqlQuery *query = new QSqlQuery(db);
        QString execStr;
        for (int i = 0; i < ids.size()-2; i++)
        {
            execStr += "SELECT " + QString::number(i) + " UNION ";
        }
        execStr += ("select -1");
        query->exec(execStr);

        return new TimeSeriesStream(this, query);
    }



private:
    //main object in class
    static QHash<QString, QMap <int, double> > storage_;
    QString currentID_;
    QList<TimeSeriesID> listOfId_;

};


class TDataInMemmoryMoc : public QObject
{
};

#endif // DATAINMEMMORYMOC_H
