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

QString Analyzer::nameOfTS()
{

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

double Analyzer::var(const TimeSeries &timeSeries)
{
    double avg_=avg(timeSeries);
    double dev_=dev(timeSeries);
    if (avg==0 || dev_==0)
    {
        return 0;
    }
    return dev_/avg_;
}

bool TimeSeries :: operator ==(const TimeSeries &series) const
{
    if(size() != series.size())
    {
        return false;
    }

    for(int i = 0; i < series.size(); i++)
    {
        if(!fuzzyCompare(at(i), series.at(i)))
        {
            return false;
        }
    }

    return id_ == series.id_;
}

double AnalysisResult::value(const QString &id,const QString &tag) const
{
    return table_.value(id).value(tag, 0.0);
}

bool AnalysisResult:: operator == (const AnalysisResult &result) const
{
    foreach(const QString &id, table_.keys())
    foreach(const QString &tag, (table_.keys() + result.table_.keys()).toSet())
    {
        if(!table_.contains(tag) || !result.table_.contains(tag))
        {
            return false;
        }
        if(!fuzzyCompare(value(id,tag), result.value(id,tag)))
        {
            return false;
        }
    }
    return true;
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

void TAnalyzer::TestVariation_data()
{
    QTest::addColumn<TimeSeries>("timeSeries");
    QTest::addColumn<double>("expected");

    QTest::newRow("empty_data") << TimeSeries() << 0.0;
    QTest::newRow("data_with_1_double")<< (TimeSeries() << 1.0) << 0.0;
    QTest::newRow("data_with_2_double")<< (TimeSeries() << 1.0 << 2.0) << 0.4714;
    QTest::newRow("data_with_3_double")<< (TimeSeries() << 1.0 << 2.0 << 5.0) << 0.780625;
    QTest::newRow("data_with_negative_and_positive_double")<< (TimeSeries() << 1.0 << 2.0 << -5.0) << -5.67891;
}

void TAnalyzer::TestVariation()
{
    QFETCH(TimeSeries, timeSeries);
    QFETCH(double, expected);

    const double actual = Analyzer::var(timeSeries);

    QVERIFY2(fuzzyCompare(actual, expected), QString("actual:%1 expected:%2").arg(actual).arg(expected).toLatin1());
}

void TAnalyzer::TestAnalyze_data()
{
    QTest::addColumn<Analyzer*>("analyzer");
    QTest::addColumn<TimeSeries>("timeSeries");
    QTest::addColumn<AnalysisResult>("expectedResult");

    QTest::newRow("avg-analyzer1") << (static_cast<Analyzer*>(new AvgAnalyzer()))
                                   << (TimeSeries() << 1.0)
                                   << AnalysisResult().insert("A","Average", 1.0);

    QTest::newRow("avg-analyzer1") << (static_cast<Analyzer*>(new AvgAnalyzer()))
                                   << (TimeSeries() << 1.0 << 2.0)
                                   << AnalysisResult().insert("A","Average", 1.5);

    QTest::newRow("dev-analyzer") << (static_cast<Analyzer*>(new DevAnalyzer()))
                                  << (TimeSeries()<< 1.0 << 2.0 << 5.0)
                                  << AnalysisResult().insert("A","Deviation", 2.08167);
    QTest::newRow("var-analyzer") << (static_cast<Analyzer*>(new VarCoefAnalyzer()))
                                  << (TimeSeries()<< 1.0 << 2.0 << 5.0)
                                  << AnalysisResult().insert("A","Variation", 0.780625);

    QTest::newRow("complex-analyzer") << (static_cast<Analyzer*>(new ComplexAnalyzer(QList<Analyzer*>()
                                                                                     << new AvgAnalyzer()
                                                                                     << new DevAnalyzer()
                                                                                     << new VarCoefAnalyzer())))
                                      <<(TimeSeries()<< 1.0 << 2.0 << 5.0)
                                     <<AnalysisResult()
                                       .insert("A","#Average", (1.0 + 2.0 + 5.0) / 3.0)
                                       .insert("A","#Deviation", 2.08167)
                                       .insert("A","#Variation", 0.780625);
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

void TAnalyzer::TestAnalysisResultProject_data()
{
    QTest::addColumn<AnalysisResult>("result");
    QTest::addColumn<QString>("id");
    QTest::addColumn<AnalysisResult>("expectedProjection");

    QTest::newRow("empty") << AnalysisResult()
                           << "id1"
                           << AnalysisResult();

    QTest::newRow("single-element1") << AnalysisResult()
                                        .insert("id1", "#tag1", 10.0)
                                     << "id1"
                                     << AnalysisResult()
                                        .insert("id1", "#tag1", 10.0);

    QTest::newRow("single-element2") << AnalysisResult()
                                        .insert("id1", "#tag1", 10.0)
                                     << "id2"
                                     << AnalysisResult();

    QTest::newRow("two-elements1") << AnalysisResult()
                                      .insert("id1", "#tag1", 10.0)
                                      .insert("id2", "#tag1", 100.0)
                                   << "id1"
                                   << AnalysisResult()
                                      .insert("id1", "#tag1", 10.0);

    QTest::newRow("two-elements2") << AnalysisResult()
                                      .insert("id1", "#tag1", 10.0)
                                      .insert("id2", "#tag1", 100.0)
                                   << "id2"
                                   <<   AnalysisResult()
                                        .insert("id2", "#tag1", 100.0);


    QTest::newRow("two-elements3") << AnalysisResult()
                                      .insert("id1", "#tag1", 10.0)
                                      .insert("id2", "#tag1", 100.0)
                                      .insert("id2", "#tag2", 999.0)
                                   << "id2"
                                   <<   AnalysisResult()
                                        .insert("id2", "#tag1", 100.0)
                                        .insert("id2", "#tag2", 999.0);

    QTest::newRow("replacement") << AnalysisResult()
                                        .insert("id1", "#tag1", 10.0)
                                        .insert("id1", "#tag1", 100.0)
                                     << "id1"
                                     << AnalysisResult()
                                        .insert("id1", "#tag1", 190.0);
}

void TAnalyzer::TestAnalysisResultProject()
{
    QFETCH(AnalysisResult, result);
    QFETCH(QString, id);
    QFETCH(AnalysisResult, expectedProjection);

    QCOMPARE(result.project(id), expectedProjection);
}

void TAnalyzer::TestAnalyzeForIDs_data()
{

}

void TAnalyzer::TestAnalyzeForIDs()
{
    QFETCH(QList<TimeSeries>, timeSeriesCollection);
    QFETCH(ComplexAnalyzer*, analyzer);
    QFETCH(QList<QString>, ids);
    QFETCH(AnalysisResult, expectedResult);

    const QString databaseName = QString(QTest::currentDataTag()) + "TAnalyzer::TestAnalyzeForIDs.db";

    QVERIFY(TimeSeriesDBI::clear(databaseName));

    {
        TimeSeriesDBI dbi(databaseName);
        foreach(const TimeSeries &ts, timeSeriesCollection)
        {
            dbi.write(ts);
        }
    }

    const AnalysisResult actualResult = analyzer->analyzeForIDs(databaseName, ids);

    foreach(const QString &id, ids)
    {
        QCOMPARE(actualResult.project(id), expectedResult.project(id));
    }

    QCOMPARE(actualResult, expectedResult);

    delete analyzer;
}






















