#ifndef TIMESERIESDBI_H

#define TIMESERIESDBI_H

#include <QtCore>
#include <QtTest/QTest>
#include <QtSql>

#include "TimeSeries.h"

typedef QList<TimeSeries> TimeSeriesList;

class TimeSeriesDBI
{
public:
    virtual ~TimeSeriesDBI();

    //запись временных рядов из файла в базу
    virtual void loadDataFromFile(const QString &path) = 0;

    //запись временных рядов из ts в базу
    virtual void write(const TimeSeriesList &ts) = 0;

    //чтение временных рядов из базы
    virtual TimeSeriesList read(const QList<TimeSeriesID> &ids) = 0;

    //удаление базы с именем databaseName
    virtual bool remove(const QString &databaseName) = 0;

    //открытие и получение указателя на интерфейс для базы с именем databaseName
    virtual TimeSeriesDBI* open(const QString &databaseName) = 0;
};

Q_DECLARE_METATYPE(TimeSeriesDBI*)

class TimeSeriesDocumentDBI : public TimeSeriesDBI
{
public:
    TimeSeriesDocumentDBI();

    //main constructor with creating DB
    TimeSeriesDocumentDBI(const QString path);

    //open json file and call insertIntoTable
    void loadDataFromJson(const QString path);

    //insert object into datbase table
    void insertIntoTable(const QHash <QString,QString> &ts);

    //convert list of strings into list of TimeSeries
    QList <TimeSeries> timeSeriesFromString(const QList<QString> &ids);

    //get strings from DB with set ids
    QHash <QString, QString> getStringFromDatBase(const QList<QString> &ids);

    //insert object (TimeSeriesList) into datbase table
    void insertIntoTableFromOriginalType(const TimeSeriesList &ts);

    TimeSeries timeSeriesFromQMap(const QString &strJsonValue, QMap <int, double> mapTS);

    void inhectionIn(const QHash <QString,QString> &tSLRecord, const QHash <QString,QString> &ts);

    QMap<int,double> getMapFromJson(const QString &strJsonValue);

    //return all names
    QList<QString> fetchAllIDs(const QList<QString> names);

    //close conection with DB and delete it
    static bool clear(const QString &databaseName);

    //запись временных рядов из файла в базу
    virtual void loadDataFromFile(const QString &path);

    //запись временных рядов из ts в базу
    virtual void write(const TimeSeriesList &ts);

    //чтение временных рядов из базы
    virtual TimeSeriesList read(const QList<TimeSeriesID> &ids);

    //удаление базы с именем databaseName
    virtual bool remove(const QString &databaseName);

    //открытие и получение указателя на интерфейс для базы с именем databaseName
    virtual TimeSeriesDBI* open(const QString &databaseName);

private:
    static QSqlDatabase m_db_;
};

#endif // TIMESERIESDBI_H
