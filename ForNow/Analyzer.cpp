#include "Analyzer.h"

double Analyzer::avg(const TimeSeries &timeSeries)
{
    if(timeSeries.isEmpty())
    {
        return 0.0;
    }

    double sum = 0.0;
    foreach(const double e, timeSeries)
    {
        sum += e;
    }

    return sum / timeSeries.length();
}



double Analyzer::dev(const TimeSeries &timeSeries)
{

    if (timeSeries.isEmpty())
    {
        return 0;
    }
    else
    {
        const double aver = avg(timeSeries);
        double summ = 0;
        foreach(const double  element, timeSeries)
        {
            summ +=  pow((element - aver),2);
        }
        const double divider = (timeSeries.length() - 1);
        if(divider == 0)
        {
            return 0;
        }
        return sqrt(summ / divider);

    }
}

void TAnalyzer::TestAverage_data()
{
    QTest::addColumn<TimeSeries>("timeSeries");
    QTest::addColumn<double>("expected");

    QTest::newRow("empty-data") << TimeSeries() << 0.0;

    QTest::newRow("data-with-1 double")
            << (TimeSeries() << 1.0 << 2.0)
            << 1.5;

    QTest::newRow("data-with-3-double")
            << (TimeSeries() << 1.0 << 2.0 << 3.0)
            <<2.0;

    QTest::newRow("data-with-negative-and-positive-double")
            << (TimeSeries() << 100.0 << -200.0)
            << -50.0;

    for(int i = 0; i < 1000; i++)
    {
        double a = static_cast<double>(rand() % 100000) / static_cast<double>(rand() % 100000);
        double b = static_cast<double>(rand() % 100000) / static_cast<double>(rand() % 100000);
        QTest::newRow(QString("rand" + QString::number(i)).toLatin1()) << (TimeSeries() <<  a << b) << ((a + b) / 2.0);
    }
}

void TAnalyzer::TestAverage()
{
    QFETCH(TimeSeries, timeSeries);
    QFETCH(double, expected);

    const double actual = Analyzer::avg(timeSeries);

    QVERIFY(fuzzyCompare(actual, expected));
}

void TAnalyzer::TestDeviation_data()
{
    QTest::addColumn<TimeSeries>("timeSeries");
    QTest::addColumn<double>("expected");

    QTest::newRow("empty_data") << TimeSeries() << 0.0;
    QTest::newRow("data_with_1_double")<< (TimeSeries() << 1.0) << 0.0;
    QTest::newRow("data_with_2_double")<< (TimeSeries() << 1.0 << 2.0) << 0.70711;
    QTest::newRow("data_with_3_double")<< (TimeSeries() << 1.0 << 2.0 << 5.0) << 2.08167;
    QTest::newRow("data_with_negative_and_positive_double")<< (TimeSeries() << 1.0 << 2.0 << -5.0) << 3.78594;
}

void TAnalyzer::TestDeviation()
{
    QFETCH(TimeSeries, timeSeries);
    QFETCH(double, expected);

    const double actual = Analyzer::dev(timeSeries);

    QVERIFY2(fuzzyCompare(actual, expected), QString("actual:%1 expected:%2").arg(actual).arg(expected).toLatin1());
}

void TAnalyzer::TestAnalyze_data()
{
    QTest::addColumn<Analyzer*>("analyzer");
    QTest::addColumn<TimeSeries>("timeSeries");
    QTest::addColumn<AnalysisResult>("expectedResult");

    QTest::newRow("stupid-analyzer1") << (static_cast<Analyzer*>(new StupidAnalyzer()))
                                      << TimeSeries()
                                      << AnalysisResult();

    QTest::newRow("stupid-analyzer2") << (static_cast<Analyzer*>(new StupidAnalyzer()))
                                      << (TimeSeries() << 1.0)
                                      << AnalysisResult();

    QTest::newRow("avg-analyzer1") << (static_cast<Analyzer*>(new AvgAnalyzer()))
                                   << (TimeSeries() << 1.0)
                                   << AnalysisResult().insert("#avg", 1.0);

    QTest::newRow("avg-analyzer1") << (static_cast<Analyzer*>(new AvgAnalyzer()))
                                   << (TimeSeries() << 1.0 << 2.0)
                                   << AnalysisResult().insert("#avg", 1.5);

    QTest::newRow("dev-analyzer") << (static_cast<Analyzer*>(new DevAnalyzer()))
                                  << (TimeSeries()<< 1.0 << 2.0 << 5.0)
                                  << AnalysisResult().insert("#dev", 2.08167);

    QTest::newRow("complex-analyzer") << (static_cast<Analyzer*>(new ComplexAnalyzer(QList<Analyzer*>()
                                                                                     << new StupidAnalyzer()
                                                                                     << new AvgAnalyzer()
                                                                                     << new DevAnalyzer())))
                                      <<(TimeSeries()<< 1.0 << 2.0 << 5.0)
                                     <<AnalysisResult()
                                       .insert("#avg", (1.0 + 2.0 + 5.0) / 3.0)
                                       .insert("#dev", 2.08167);
}

void TAnalyzer::TestAnalyze()
{
    QFETCH(Analyzer*, analyzer);
    QFETCH(TimeSeries, timeSeries);
    QFETCH(AnalysisResult, expectedResult);

    AnalysisResult actualResult = analyzer->analyze(timeSeries);
    QCOMPARE(actualResult, expectedResult);

    delete analyzer;
}
