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
    class TimeSeriesStream
    {
    public:
        TimeSeriesStream(TimeSeriesDBI *dbi, QSqlQuery *query) :
            dbi_(dbi),
            query_(query)
        {
            //Q_ASSERT(query_ != Q_NULLPTR);
            Q_ASSERT(dbi_ != Q_NULLPTR);
        }
        TimeSeries current();

        void setQuery(QSqlQuery *query)
        {
            query_ = query;
        }

        bool next();

    private:
        TimeSeriesDBI *dbi_;
        QSqlQuery *query_;
    };

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

    virtual TimeSeriesStream* stream(const QList<TimeSeriesID> &ids) = 0;

protected:
    virtual TimeSeries fetchTimeSeriesFromQuery(QSqlQuery *query) = 0;
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

    virtual TimeSeries fetchTimeSeriesFromQuery(QSqlQuery *query);

    //convert list of strings into list of TimeSeries
    QList <TimeSeries> timeSeriesFromString(const QList<QString> &ids);

    TimeSeries fetchTimeSeriesFromQueryDBI(QSqlQuery *query);

    //get strings from DB with set ids
    QHash <QString, QString> getStringFromDatBase(const QList<TimeSeriesID> &ids);

    //insert object (TimeSeriesList) into datbase table
    void insertIntoTableFromOriginalType(const TimeSeriesList &timeSeriesList);

    TimeSeries timeSeriesFromQMap(const QString &strJsonValue, QMap <int, double> mapTS);

    void injectionIn(const QHash <TimeSeriesID,QString> &tSLRecord,
                     const QHash <TimeSeriesID,QString> &ts);

    QSqlQuery* getQueryForIndependQuery(const  QList<TimeSeriesID> &ids);

    QMap<int,double> getMapFromJson(const QString &strJsonValue);

    void insertIntoOriginalType(const TimeSeriesList &ts);

    void deleteFromOriginalTypes(const TimeSeriesList &ts);

    QString getPath()
    {
        return m_db_.databaseName();
    }
    //return all names
    QList<QString> fetchAllIDs(QList<QString> list);

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

    virtual TimeSeriesStream* stream(const QList<TimeSeriesID> &ids)
    {
        return new TimeSeriesStream(this, this->getQueryForIndependQuery(ids));
    }
private:
    static QSqlDatabase m_db_;
    //static QString pathTo;
};
#endif // TIMESERIESDBI_H
