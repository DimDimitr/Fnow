#include "DataInMemmoryMoc.h"

QHash<QString, TimeSeries> DataInMemmoryMoc::storage_;

void TDataInMemmoryMoc::TestWriteReadRewrite_data()
{
    typedef QList<TimeSeries> TimeSeriesList;
    QTest::addColumn<ComplexAnalyzer*>("analyzer");
    QTest::addColumn<int>("expectedResult");


    //1-st test
    QTest::newRow("TimeTests")
            <<new ComplexAnalyzer(QList<Analyzer*>()
                                  << new AvgAnalyzer()
                                  << new DevAnalyzer()
                                  << new VarCoefAnalyzer()
                                  )
           <<1;
}

void TDataInMemmoryMoc::TestWriteReadRewrite()
{
    typedef QList<TimeSeries> TimeSeriesList;
    QFETCH(int, expectedResult);
    QFETCH(ComplexAnalyzer*, analyzer);

    const QString databaseName = "TestTimeIDs.db";

    QVERIFY(DataInMemmoryMoc::clear(databaseName));

    TimeSeriesList generate;
    for (int i = 0; i < 40; i ++)
    {
        int tag = qrand();
        TimeSeries ts(QString::number(tag));
        for(int j = 0; j < 1000; j++)
        {
           ts.append( ((double)qrand()/(double)RAND_MAX));
        }
        generate.append(ts);
    }


    DataInMemmoryMoc dbi(databaseName);
    QElapsedTimer timer;

//1-st Import
    timer.start();
    dbi.write(generate);
    qWarning() << "Import operation took" << timer.elapsed() << "milliseconds";

    QList <QString> tags;
    for (int i = 0; i < generate.size(); i++)
    {
        tags.append(generate.value(i).id());
    }

//2-nd Analise
    timer.start();
    AnalysisResult anResult = analyzer->analyzeForIDsTestMoc(&dbi, tags);
    qWarning() << "Analise operation took" << timer.elapsed() << "milliseconds";

    int actualResult = 1;
    QCOMPARE(actualResult, expectedResult);
    delete analyzer;
}
