#include "TTimeSeriesDBI.h"
#include "TimeSeriesDBI.h"
#include "DataInMemmoryMoc.h"

TTimeSeriesDBI::TTimeSeriesDBI(int choose)
{
    switch ( choose )
    {
    case 0:
    {
        dbiTable_.insert("json-doc", new TimeSeriesDocumentDBI());
        break;
    }
    case 1:
    {
            dbiTable_.insert("inmemmory", new DataInMemmoryMoc());
        break;
    }
}

}

void TTimeSeriesDBI::TestWriteReadRewireRead_data()
{
    QTest::addColumn<TimeSeriesDBI*>("dbi");
    QTest::addColumn<TimeSeriesList>("initTimeSeries");
    QTest::addColumn< QList<TimeSeriesID> >("initIDs");

    foreach(const QString &dbiName, dbiTable_.keys())
    {
        TimeSeriesDBI *dbi = dbiTable_.value(dbiName);

        QTest::newRow(QString("empty_" + dbiName).toLatin1())
                << dbi
                << TimeSeriesList()
                << QList<TimeSeriesID>();

        QTest::newRow(QString("single_series_" + dbiName).toLatin1())
                << dbi
                << (TimeSeriesList() << (TimeSeries("ts1") << 100.0))
                << (QList<TimeSeriesID>() << "ts1");
    }
}

void TTimeSeriesDBI::TestWriteReadRewireRead()
{
    QFETCH(TimeSeriesDBI*, dbi);
    QFETCH(TimeSeriesList, initTimeSeries);
    QFETCH(QList<TimeSeriesID>, initIDs);

    const QString databaseName = QString(QTest::currentDataTag()) + "TTimeSeriesDBI.TestWriteReadRewireRead.db";
    QVERIFY(dbi->remove(databaseName));

    {
        TimeSeriesDBI *writeDBI = dbi->open(databaseName);
        writeDBI->write(initTimeSeries);
        delete writeDBI;
    }

    {
        TimeSeriesDBI *readDBI = dbi->open(databaseName);
        const TimeSeriesList actualInitTimeSeries = readDBI->read(initIDs);
        delete readDBI;

        QCOMPARE(actualInitTimeSeries, initTimeSeries);
    }
}
