#include "TimeSeriesDatabase.h"

QHash<QString, TimeSeries> TimeSeriesDBI::storage_;

TimeSeriesDatabase::TimeSeriesDatabase()
{

}

void TTimeSeriesDatabase::TestWriteReadRewrite_data()
{
    QTest::addColumn<TimeSeries>("initSeries1");
    QTest::addColumn<TimeSeries>("initSeries2");
    QTest::addColumn<TimeSeries>("series1");
    QTest::addColumn<TimeSeries>("series2");

    QTest::newRow("empty-data") << TimeSeries()
                                << TimeSeries()
                                << TimeSeries()
                                << TimeSeries();

    QTest::newRow("trivial-data1") << (TimeSeries("id1") << 1)
                                   << (TimeSeries("id2") << 2)
                                   << (TimeSeries("id1") << 1)
                                   << (TimeSeries("id2") << 2);

    QTest::newRow("trivial-data2") << (TimeSeries("id1") << 1 << 2 << 3)
                                   << (TimeSeries("id2") << 3 << 2 << 1 << 0 << -1)
                                   << (TimeSeries("id1") << 1 << 2 << 3)
                                   << (TimeSeries("id2") << 3 << 2 << 1 << 0 << -1);

    QTest::newRow("conflict-data1") << (TimeSeries("id1") << 1 << 2 << 3)
                                    << (TimeSeries("id1") << 3 << 2 << 1 << 0 << -1)
                                    << (TimeSeries("id1") << 3 << 2 << 1 << 0 << -1)
                                    << (TimeSeries("id2"));
}

void TTimeSeriesDatabase::TestWriteReadRewrite()
{
    QFETCH(TimeSeries, initSeries1);
    QFETCH(TimeSeries, initSeries2);
    QFETCH(TimeSeries, series1);
    QFETCH(TimeSeries, series2);

    QString databaseName = "TTimeSeriesDatabase.TestWriteReadRewrite.db";

    QVERIFY(TimeSeriesDBI::clear(databaseName));

//    if(QFile::exists(databaseName))
//    {
//        QFile::remove(databaseName);
//    }
//    QVERIFY(!QFile::exists(databaseName));

    {
        TimeSeriesDBI dbi(databaseName);
        dbi.write(initSeries1);
    }

    {
        TimeSeriesDBI dbi(databaseName);
        const TimeSeries actual = dbi.read(initSeries1.id());
        const TimeSeries expected = initSeries1;

        QCOMPARE(actual, expected);
    }

    {
        TimeSeriesDBI dbi(databaseName);
        dbi.write(initSeries2);
    }

    {
        TimeSeriesDBI dbi(databaseName);
        const TimeSeries actual = dbi.read(initSeries2.id());
        const TimeSeries expected = initSeries2;

        QCOMPARE(actual, expected);
    }

    {
        TimeSeriesDBI dbi(databaseName);
        const TimeSeries actual = dbi.read(series1.id());
        const TimeSeries expected = series1;

        QCOMPARE(actual, expected);
    }

    {
        TimeSeriesDBI dbi(databaseName);
        const TimeSeries actual = dbi.read(series2.id());
        const TimeSeries expected = series2;

        QCOMPARE(actual, expected);
    }
}
