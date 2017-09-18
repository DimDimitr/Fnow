#ifndef TIMESERIESINARRAY_H

#define TIMESERIESINARRAY_H

#include "TimeSeriesDBI.h"

class TimeSeriesInArray: public TimeSeriesDBI
{
public:
    TimeSeriesInArray();
    //main constructor with creating DB
    TimeSeriesInArray(const QString path);

    //open json file and call insertIntoTable
    void loadDataFromJson(const QString path);

    //insert object into datbase table
    void insertIntoTable(const QHash <QString,QString> &ts);

    virtual TimeSeries fetchTimeSeriesFromQuery(QSqlQuery *query);

    TimeSeries fetchTimeSeriesFromQueryARR(QSqlQuery *query);

    QMap<int,double> getMapFromStr(const QString &strOfValue);

    TimeSeries timeSeriesFromQMap(const QString &strJsonValue, QMap <int, double> mapTS);

    void injectionIn(const QHash<TimeSeriesID, QString> &init,
                                        const QHash<TimeSeriesID, QString> &additional);

    void deleteFromOriginalTypes(const TimeSeriesList &ts);

    //convert list of strings into list of TimeSeries
    QList <TimeSeries> timeSeriesFromString(const QList<QString> &ids);

    //get strings from DB with set ids
    QHash <QString, QString> getStringFromDatBase(const QList<QString> &ids);

    //insert object (TimeSeriesList) into datbase table
    void insertIntoTableFromOriginalTypes(const TimeSeriesList &ts);

    //return all names
    QList<QString> fetchAllIDs(const QList<QString> names);

    QSqlQuery* getQueryForIndependQuery(const  QList<TimeSeriesID> &ids);

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
    virtual TimeSeriesInArray* open(const QString &databaseName);

    virtual TimeSeriesStream* stream(const QList<TimeSeriesID> &ids)
    {
        return new TimeSeriesStream(this, this->getQueryForIndependQuery(ids));
    }

private:
    static QSqlDatabase m_db_;
};

#endif // TIMESERIESINARRAY_H
