#ifndef TIMESERIESINLONGTABLE_H

#define TIMESERIESINLONGTABLE_H

#include "TimeSeriesDBI.h"

class TimeSeriesInLongTable: public TimeSeriesDBI
{
public:
    TimeSeriesInLongTable();
    //main constructor with creating DB
    TimeSeriesInLongTable(const QString path);

    //open json file and call insertIntoTable
    void loadDataFromJson(const QString path);

    //insert object into datbase table
    void insertIntoTable(const QHash <QString, QHash <int, double> > &ts);

    //get strings from DB with set ids
    QList <TimeSeries> getStringFromDatBase(const QList<QString> &ids);

    //insert object (TimeSeriesList) into datbase table
    void insertIntoTableFromOriginalTypes(const TimeSeriesList &ts);

    //return all names
    QList<QString> fetchAllIDs(const QList<QString> names);

    QHash <QString, QMap<int, double> > hashFromDatBase(const  QList<QString> &ids);

    TimeSeries timeSeriesFromQMap(const QString &strJsonValue, QMap <int, double> mapTS);

    void inhectionIn(const QHash <QString, QMap<int, double> > &tSLRecord, const QHash <QString, QHash<int, double> > &ts);

    void deleteFromOriginalTypes(const TimeSeriesList &ts);

    QSqlQuery* getQueryForIndependQuery(const  QList<TimeSeriesID> &ids);

    //close conection with DB and delete it
    static bool clear(const QString &databaseName);

    //запись временных рядов из файла в базу
    virtual void loadDataFromFile(const QString &path);

    virtual TimeSeries fetchTimeSeriesFromQuery(QSqlQuery *query);

    //запись временных рядов из ts в базу
    virtual void write(const TimeSeriesList &ts);

    //чтение временных рядов из базы
    virtual TimeSeriesList read(const QList<TimeSeriesID> &ids);

    //удаление базы с именем databaseName
    virtual bool remove(const QString &databaseName);

    //открытие и получение указателя на интерфейс для базы с именем databaseName
    virtual TimeSeriesInLongTable* open(const QString &databaseName);

    virtual TimeSeriesStream* stream(const QList<TimeSeriesID> &ids)
    {
        return new TimeSeriesStream(this, this->getQueryForIndependQuery(ids));
    }

private:
    static QSqlDatabase db_;
};
#endif // TIMESERIESINLONGTABLE_H
