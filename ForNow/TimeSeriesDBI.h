#ifndef TIMESERIESDBI_H

#define TIMESERIESDBI_H

#include "Analyzer.h"

#include <QtCore>
#include <QtTest/QTest>

#include <QtSql>
#include <QMessageBox>
#include <QMainWindow>
typedef QList <TimeSeries> TimeSeriesList;

class TimeSeriesDBI
{
public:
    TimeSeriesDBI(const QString path);
    TimeSeriesDBI(const QString path, const int key);

    void write(const QList<TimeSeries> &ts);

    TimeSeries read(const QString &id);
    
    void loadDataFromJson (const QString path);

    void insertIntoTable(const QHash <QString,QString> &ts);

    QList <TimeSeries> timeSeriesFromString(const QList<QString> &ids);

    QHash <QString, QString> getStringFromDatBase(const QList<QString> &ids);

    void insertIntoTableFromOriginalType(const TimeSeriesList &ts);




    QList<QString> fetchAllIDs(const QList<QString> names)
        {
            return names;
        }

    static bool clear(const QString &databaseName)
    {
        m_db.close();
        QSqlQuery query(m_db);
        m_db = QSqlDatabase();
        QSqlDatabase::removeDatabase(databaseName);
        return true;
    }    
private:
    static QSqlDatabase m_db;
};

class TTimeSeriesDBI : public QObject
{
    Q_OBJECT
private slots:
    void TestWriteReadRewrite_data();
    void TestWriteReadRewrite();
};

#endif // TIMESERIESDBI_H
