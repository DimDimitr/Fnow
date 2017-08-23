#ifndef TIMESERIESDBI_H

#define TIMESERIESDBI_H

#include "Analyzer.h"

#include <QtCore>
#include <QtTest/QTest>

#include <QtSql>

typedef QList <TimeSeries> TimeSeriesList;


class TimeSeriesDBI
{
public:
    //main constructor with creating DB
    TimeSeriesDBI(const QString path);

    //open json file and call insertIntoTable
    void loadDataFromJson (const QString path);

    //insert object into datbase table
    void insertIntoTable(const QHash <QString,QString> &ts);

    //convert list of strings into list of TimeSeries
    QList <TimeSeries> timeSeriesFromString(const QList<QString> &ids);

    //get strings from DB with set ids
    QHash <QString, QString> getStringFromDatBase(const QList<QString> &ids);

    //insert object (TimeSeriesList) into datbase table
    void insertIntoTableFromOriginalType(const TimeSeriesList &ts);

    //return all names
    QList<QString> fetchAllIDs(const QList<QString> names);

    //close conection with DB and delete it
    static bool clear(const QString &databaseName);

private:
    static QSqlDatabase m_db;
};

#endif // TIMESERIESDBI_H
