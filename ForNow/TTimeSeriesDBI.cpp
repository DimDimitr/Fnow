#include "TTimeSeriesDBI.h"
#include "TimeSeriesDBI.h"
#include "DataInMemmoryMoc.h"
#include "Analyzer.h"
#include "TimeSeriesInArray.h"

TTimeSeriesDBI::TTimeSeriesDBI(int choose)
{
    switch ( choose )
    {
    case 0:
    {
        //dbiTable_.insert("json-doc", new TimeSeriesDocumentDBI());
        dbiTable_.insert("string_doc", new TimeSeriesInArray());
        break;
    }
    case 1:
    {
        //dbiTable_.insert("inmemmory", new DataInMemmoryMoc());
        dbiTable_.insert("string_doc", new TimeSeriesInArray());
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
                << (TimeSeriesList() << (TimeSeries("ts1") << 110.0))
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
        writeDBI->write(TimeSeriesList() << (TimeSeries("ts1") << 125.0));
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



char *toString(const TimeSeries &ts)
{
    using QTest::toString;
    return toString(ts.toString());
}




TBenchAnalyzer::TBenchAnalyzer(bool choose)
{
    if (choose == false)
    {
        //dbiTableBench_.insert("moc", new TimeSeriesDocumentDBI());
                dbiTableBench_.insert("string_doc", new TimeSeriesInArray());
    }
    else
    {
        //dbiTableBench_.insert("real", new DataInMemmoryMoc());
                dbiTableBench_.insert("string_doc", new TimeSeriesInArray());
    }
}




void TBenchAnalyzer::BenchmarkImportAnalizeExport_data()
{
    QTest::addColumn<TimeSeriesDBI*>("dbiT");
    QTest::addColumn<ComplexAnalyzer*>("analyzer");
    QTest::addColumn<int>("expectedResult");
    //1-st test

    foreach(const QString &dbiName, dbiTableBench_.keys())
    {
        TimeSeriesDBI *dbiT = dbiTableBench_.value(dbiName);
        QTest::newRow("TimeTests")
                <<dbiT
               << new ComplexAnalyzer(QList<Analyzer*>()
                                      << new AvgAnalyzer()
                                      << new DevAnalyzer()
                                      << new VarCoefAnalyzer()
                                      )
               << 1;
    }
}



void TBenchAnalyzer::BenchmarkImportAnalizeExport()
{
    typedef QList<TimeSeries> TimeSeriesList;
    QFETCH(int, expectedResult);
    QFETCH(ComplexAnalyzer*, analyzer);
    QFETCH(TimeSeriesDBI*, dbiT);
    const QString databaseName = "TestTimeIDs.db";
    QVERIFY(dbiT->remove(databaseName));

    TimeSeriesList generate;
    for (int i = 0; i < 4000; i ++)
    {
        int tag = qrand();
        TimeSeries ts(QString::number(tag));
        for(int j = 0; j < 1000; j++)
        {
            ts.append(((double)qrand()/(double)RAND_MAX));
        }
        generate.append(ts);
    }

    TimeSeriesDBI *dbi = dbiT->open(databaseName);
    QElapsedTimer timer;

    //1-st Import
    timer.start();
    dbi->write(generate);
    qWarning() << "Import operation took" << timer.elapsed() << "milliseconds";
    QList <QString> tags;
    for (int i = 0; i < generate.size(); i++)
    {
        tags.append(generate.value(i).id());
    }

    //////////////
    //2-nd Analise
    timer.start();
    AnalysisResult result;
    result = analyzer->analyzeForIDs(dbi, tags);
    qWarning() << "Analise operation took" << timer.elapsed() << "milliseconds";

    //3-rd Export
    QString path = "Test100x1000.json";
    timer.start();
    result.saveJson(path);
    qWarning() << "Export operation took" << timer.elapsed() << "milliseconds";
    int actualResult = 1;
    QCOMPARE(actualResult, expectedResult);
    delete analyzer;
}
