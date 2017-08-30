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
    //void insertIntoTable(const QHash <QString,QString> &ts);
    void insertIntoTable(const QHash <QString, QHash <int, double> > &ts);

    //convert list of strings into list of TimeSeries
    //QList <TimeSeries> timeSeriesFromString(const QList<QString> &ids);

    //get strings from DB with set ids
    QList <TimeSeries> getStringFromDatBase(const QList<QString> &ids);

    //insert object (TimeSeriesList) into datbase table
    void insertIntoTableFromOriginalTypes(const TimeSeriesList &ts);

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
    virtual TimeSeriesInLongTable* open(const QString &databaseName);

private:
    static QSqlDatabase db_;
};
#endif // TIMESERIESINLONGTABLE_H