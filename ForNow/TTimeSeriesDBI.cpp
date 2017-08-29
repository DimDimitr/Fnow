#include "TTimeSeriesDBI.h"
#include "TimeSeriesDBI.h"
#include "DataInMemmoryMoc.h"
#include "Analyzer.h"
#include "TimeSeriesInArray.h"
#include "TimeSeriesInLongTable.h"

TTimeSeriesDBI::TTimeSeriesDBI(int choose)
{
    switch ( choose )
    {
    case 0:
    {
        //dbiTable_.insert("json-doc", new TimeSeriesDocumentDBI());
        //dbiTable_.insert("string_doc", new TimeSeriesInArray());
        //dbiTable_.insert("string_doc", new TimeSeriesInLongTable());
        break;
    }
    case 1:
    {
        //dbiTable_.insert("inmemmory", new DataInMemmoryMoc());
        //dbiTable_.insert("string_doc", new TimeSeriesInArray());
        //dbiTable_.insert("string_doc", new TimeSeriesInLongTable());
        break;
    }


    }
    dbiTable_.insert("strArray", new TimeSeriesInArray());
    dbiTable_.insert("longTable", new TimeSeriesInLongTable());
    dbiTable_.insert("json-doc", new TimeSeriesDocumentDBI());

}


void TTimeSeriesDBI::TestWriteReadRewireRead_data()
{
    QTest::addColumn<TimeSeriesDBI*>("dbi");
    QTest::addColumn<TimeSeriesList>("initTimeSeries");
    QTest::addColumn< QList<TimeSeriesID> >("initIDs");

    foreach(const QString &dbiName, dbiTable_.keys())
    {
        TimeSeriesDBI *dbi = dbiTable_.value(dbiName);

        /*QTest::newRow(QString("empty_" + dbiName).toLatin1())
                << dbi
                << TimeSeriesList()
                << QList<TimeSeriesID>();
*/
        QTest::newRow(QString("single_series_" + dbiName).toLatin1())
                << dbi
                << (TimeSeriesList() << (TimeSeries("ts1") << 110.0))
                << (QList<TimeSeriesID>() << "ts1");

        QTest::newRow(QString("Bench_" + dbiName).toLatin1())
                << dbi
                << (TimeSeriesList () << (TimeSeries("ts1")
                                          << 110.0 << 12 << 14 << 14
                                          << 104 << 45 << 17615
                                          << 140 << 45 << 17645
                                          << 114 << 425 << 17765
                                          << 124 << 451 << 17965
                                          << 134 << 4571 << 18765)
                    << (TimeSeries("ts2")
                        << 110.0 << 12 << 114 << 714
                        << 104 << 45 << 17615
                        << 140 << 45 << 17645
                        << 114 << 425 << 17765
                        << 124 << 451 << 17965
                        << 134 << 457 << 18765))
                << (QList<TimeSeriesID>() << "ts1" << "ts2");

    }
}

void TTimeSeriesDBI::TestWriteReadRewireRead()
{
    QFETCH(TimeSeriesDBI*, dbi);
    QFETCH(TimeSeriesList, initTimeSeries);
    QFETCH(QList<TimeSeriesID>, initIDs);

    qWarning() << "I'm Enter";
    const QString databaseName = QString(QTest::currentDataTag()) + "_TTimeSeriesDBI.TestWriteReadRewireRead.db";


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
        //qWarning() << "Actual" << actualInitTimeSeries << "Expected" << initTimeSeries;

        QVERIFY(actualInitTimeSeries == initTimeSeries);
        qWarning() << "Sucsess!";
    }
    {
        TimeSeriesDBI *readDBI = dbi->open(databaseName);
        QList<TimeSeriesID> inIdR;
        inIdR.append(initIDs.at(0));
        const TimeSeriesList actualInitTimeSeries = readDBI->read(inIdR);
        delete readDBI;
        TimeSeriesList initTSR;
        initTSR.append(initTimeSeries.at(0));
        QVERIFY(actualInitTimeSeries == initTSR);
    }
    {
        TimeSeriesDBI *writeDBI = dbi->open(databaseName);
        writeDBI->write(initTimeSeries);
        delete writeDBI;
    }
    {
        TimeSeriesDBI *readDBI = dbi->open(databaseName);
        QList<TimeSeriesID> inIdR;
        inIdR.append(initIDs.at(0));
        const TimeSeriesList actualInitTimeSeries = readDBI->read(inIdR);
        delete readDBI;
        TimeSeriesList initTSR;
        initTSR.append(initTimeSeries.at(0));
        QCOMPARE(actualInitTimeSeries, initTSR);
    }

    //dbi->remove(databaseName);

}



char *toString(const TimeSeries &ts)
{
    using QTest::toString;
    return toString(ts.toString());
}

char *toString(const TimeSeriesList &ts)
{
    using QTest::toString;
    QString result;
    foreach (TimeSeries tss, ts)
    {
        result.append(tss.toString());
    }
    return toString(result);
}


TBenchAnalyzer::TBenchAnalyzer(bool choose)
{
    if (choose == false)
    {
        //dbiTableBench_.insert("moc", new TimeSeriesDocumentDBI());
        //dbiTableBench_.insert("string_doc", new TimeSeriesInArray());
        //dbiTableBench_.insert("string_doc", new TimeSeriesInLongTable());
    }
    else
    {
        //dbiTableBench_.insert("real", new DataInMemmoryMoc());
        //dbiTableBench_.insert("moc", new TimeSeriesDocumentDBI());
        //dbiTableBench_.insert("string_doc", new TimeSeriesInArray());
        //dbiTableBench_.insert("string_doc", new TimeSeriesInLongTable());
    }
    dbiTableBench_.insert("long_doc_", new TimeSeriesInLongTable());
    dbiTableBench_.insert("string_doc_", new TimeSeriesInArray());
    dbiTableBench_.insert("json_", new TimeSeriesDocumentDBI());

}




void TBenchAnalyzer::BenchmarkImportAnalizeExport_data()
{
    QTest::addColumn<TimeSeriesDBI*>("dbiT");
    QTest::addColumn<QString>("databaseName");
    QTest::addColumn<ComplexAnalyzer*>("analyzer");
    QTest::addColumn<int>("expectedResult");
    //1-st test

    foreach(const QString &dbiName, dbiTableBench_.keys())
    {
        TimeSeriesDBI *dbiT = dbiTableBench_.value(dbiName);
        QTest::newRow("TimeTests")
                << dbiT
                <<(dbiName + ".db")
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
    QFETCH(QString, databaseName);
    //const QString databaseName = "TestTimeIDs.db";
    QVERIFY(dbiT->remove(databaseName));
    TimeSeriesList generate;
    for (int i = 0; i < 4000; i++)
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
    qWarning() << databaseName;
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
    QString path = databaseName + "Test100x4000.json";
    timer.start();
    result.saveJson(path);
    qWarning() << "Export operation took" << timer.elapsed() << "milliseconds";
    int actualResult = 1;
    QCOMPARE(actualResult, expectedResult);
    dbiT->remove(databaseName);
    delete analyzer;
}
