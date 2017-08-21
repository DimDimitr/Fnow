#include "Analyzer.h"
#include "DataInMemmoryMoc.h"
#include "TimeSeriesDBI.h"

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

/*QString Analyzer::nameOfTS()
{
    return QString();
}*/

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
    double avg_ = avg(timeSeries);
    double dev_ = dev(timeSeries);
    if (avg == 0 || dev_ == 0)
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
    foreach(const QString &id, (table_.keys() + result.table_.keys()).toSet())
        foreach(const QString &tag, (table_[id].keys() + result.table_[id].keys()).toSet())
        {
            if(table_.contains(tag) || result.table_.contains(tag))
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



QString AnalysisResult:: operator << (const AnalysisResult &result) const

{
    QString row;
    foreach(const QString id,  result.table_.keys())
    {
        row.append(id);
        row.append(" : ");
        foreach(const QString tag, result.table_[id].keys())
        {
            row.append(" #");
            row.append(tag);
            row.append(" = ");
            row.append(QString::number(result.table_[id].value(tag)));
        }
    }

    return row;
}



bool AnalysisResult::operator !=(const AnalysisResult &result) const
{
    return !(*this == result);
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

    for(int i = 0; i < 5; i++)
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
                                                                                     )))
                                      << (TimeSeries() << 1.0 << 2.0 << 5.0)
                                      << AnalysisResult()
                                         .insert("A", "#Average", (1.0 + 2.0 + 5.0) / 3.0)
                                         .insert("A", "#Deviation", 2.08167)
                                         .insert("A", "#Variation", 0.780625);
}

void TAnalyzer::TestAnalyze()
{
    QFETCH(Analyzer*, analyzer);
    QFETCH(TimeSeries, timeSeries);
    QFETCH(AnalysisResult, expectedResult);
    AnalysisResult actualResult;
    actualResult.insertRow("A", analyzer->analyze(timeSeries));
    QCOMPARE(actualResult.getTable()["A"].value("dev"), expectedResult.getTable()["A"].value("dev"));
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
                                    .insert("id1", "#tag1", 100.0);
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
    typedef QList<TimeSeries> TimeSeriesList;
    QTest::addColumn<ComplexAnalyzer*>("analyzer");
    QTest::addColumn<QList<QString> >("ids");
    QTest::addColumn<TimeSeriesList>("timeSeriesList");
    QTest::addColumn<AnalysisResult>("expectedResult");

    
    //1-st test
    QTest::newRow("Easy test 1 row 1 analise")
            <<new ComplexAnalyzer(QList<Analyzer*>()
                                  << new AvgAnalyzer()
                                  )

           << (QList<QString>()
               << "A")
           << (TimeSeriesList()
               << (TimeSeries("A")<< 1.0 << 2.0 << 5.0))
           << AnalysisResult()
              .insertRow("A", Hash<QString,double>()
                         .insertInc("Average", (1.0 + 2.0 + 5.0) / 3.0)
                         );

    //2-nd test
    QTest::newRow("Easy test 2 row 1 analise")
            <<new ComplexAnalyzer(QList<Analyzer*>()
                                  << new AvgAnalyzer()
                                  )

           << (QList<QString>()
               << "A"
               <<"B")
           << (TimeSeriesList()
               << (TimeSeries("A")<< 1.0 << 2.0 << 5.0)
               << (TimeSeries("B")<< 1.0 << 2.0 << -5.0))
           << AnalysisResult()
              .insertRow("A", Hash<QString,double>()
                         .insertInc("Average", (1.0 + 2.0 + 5.0) / 3.0))
              .insertRow("B", Hash<QString,double>()
                         .insertInc("Average", (1.0 + 2.0 -5.0) / 3.0)
                         );

    //3-rd test
    QTest::newRow("Easy test 1 row 2 analise")
            <<new ComplexAnalyzer(QList<Analyzer*>()
                                  << new AvgAnalyzer()
                                  << new DevAnalyzer()
                                  )

           << (QList<QString>()
               << "A")
           << (TimeSeriesList()
               << (TimeSeries("A")<< 1.0 << 2.0 << 5.0))
           << AnalysisResult()
              .insertRow("A", Hash<QString,double>()
                         .insertInc("Average", (1.0 + 2.0 + 5.0) / 3.0)
                         .insertInc("Deviation", 2.08167)
                         );

    //4-th test
    QTest::newRow("Easy test 1 row all analise")
            <<new ComplexAnalyzer(QList<Analyzer*>()
                                  << new AvgAnalyzer()
                                  << new DevAnalyzer()
                                  << new VarCoefAnalyzer()
                                  )

           << (QList<QString>()
               << "A")
           << (TimeSeriesList()
               << (TimeSeries("A")<< 1.0 << 2.0 << 5.0))
           << AnalysisResult()
              .insertRow("A", Hash<QString,double>()
                         .insertInc("Average", (1.0 + 2.0 + 5.0) / 3.0)
                         .insertInc("Deviation", 2.08167)
                         .insertInc("Variation", 0.780625)
                         );


    //5-th test
    QTest::newRow("1-st test for AnalyzeForIDs_data")
            <<new ComplexAnalyzer(QList<Analyzer*>()
                                  << new AvgAnalyzer()
                                  << new DevAnalyzer()
                                  << new VarCoefAnalyzer()
                                  )
           << (QList<QString>()
               << "A"
               << "B")
           << (TimeSeriesList()
               << (TimeSeries("A")<< 1.0 << 2.0 << 5.0)
               << (TimeSeries("B")<< 1.0 << 2.0 << 5.0))
           << AnalysisResult()
              .insertRow("A", Hash<QString,double>()
                         .insertInc("Average", (1.0 + 2.0 + 5.0) / 3.0)
                         .insertInc("Deviation", 2.08167)
                         .insertInc("Variation", 0.780625))
              .insertRow("B", Hash<QString,double>()
                         .insertInc("Average", (1.0 + 2.0 + 5.0) / 3.0)
                         .insertInc("Deviation", 2.08167)
                         .insertInc("Variation", 0.780625));
}




void TAnalyzer::TestAnalyzeForIDs()
{
    typedef QList<TimeSeries> TimeSeriesList;
    QFETCH(TimeSeriesList, timeSeriesList);
    QFETCH(ComplexAnalyzer*, analyzer);
    QFETCH(QList<QString>, ids);
    QFETCH(AnalysisResult, expectedResult);
    const QString databaseName = QString(QTest::currentDataTag()) + "IDs.db";

    QVERIFY(TimeSeriesDBI::clear(databaseName));

    TimeSeriesDBI dbi(databaseName,1);
    {

            dbi.insertIntoTableFromOriginalType(timeSeriesList);
    }

    const AnalysisResult actualResult = analyzer->analyzeForIDs(&dbi, ids);
    dbi.clear(databaseName);
    //qWarning()<<"I get actualResult"<<actualResult.table_;
    //qWarning()<<"I get expectedResult"<<expectedResult.table_;
    foreach(const QString &id, ids)
    {
        QCOMPARE(actualResult.project(id), expectedResult.project(id));
    }

    QCOMPARE(actualResult, expectedResult);
    delete analyzer;
}






void TAnalyzer::TestJsonRecord_data()
{
    QTest::addColumn<AnalysisResult>("result");
    QTest::newRow("Easy test 1 row 1 analise")
            << AnalysisResult()
              .insertRow("A", Hash<QString,double>()
                         .insertInc("Average", (1.0 + 2.0 + 5.0) / 3.0)
                         );
}



void TAnalyzer::TestJsonRecord()
{
    QFETCH(AnalysisResult, result);
    AnalysisResult temp;
    //qWarning() << result.toJSONString();
    temp = result.fromJSONString(result.toJSONString());
    //qWarning() << temp.table_;
    {
        const AnalysisResult actual = temp.fromJSONString(result.toJSONString());
        QCOMPARE(actual, result);
    }

}



void TAnalyzer::TestJsonRecordInFile_data()
{

    QTest::addColumn<AnalysisResult>("result");
    QTest::newRow("TestJsonRecordInFile_data")
            << AnalysisResult()
              .insertRow("A", Hash<QString,double>()
                         .insertInc("Average", (1.0 + 2.0 + 5.0) / 3.0)
                         );


    QTest::newRow("TestJsonRecordInFile_data")
            <<AnalysisResult()
                             .insertRow("A", Hash<QString,double>()
                                        .insertInc("Average", (1.0 + 2.0 + 5.0) / 3.0)
                                        .insertInc("Deviation", 2.08167)
                                        .insertInc("Variation", 0.780625))
                             .insertRow("B", Hash<QString,double>()
                                        .insertInc("Average", (1.0 + 2.0 + 5.0) / 3.0)
                                        .insertInc("Deviation", 2.08167)
                                        .insertInc("Variation", 0.780625));

    QTest::newRow("TestJsonRecordInFile_data")
            <<AnalysisResult()
                             .insertRow("A", Hash<QString,double>()
                                        .insertInc("Average", (1.0 + 2.0 + 5.0) / 3.0)
                                        .insertInc("Deviation", 2.08167)
                                        .insertInc("Variation", 0.780625))
                             .insertRow("B", Hash<QString,double>()
                                        .insertInc("Average", (1.0 + 2.0 + 5.0) / 3.0)
                                        );


    QTest::newRow("TestJsonRecordInFile_data")
            <<AnalysisResult()
                             .insertRow("A", Hash<QString,double>()
                                        .insertInc("Variation", 0.780625))
                             .insertRow("B", Hash<QString,double>()
                                        .insertInc("Average", (1.0 + 2.0 + 5.0) / 3.0)
                                        .insertInc("Deviation", 2.08167)
                                        .insertInc("Variation", 0.780625));
}



void TAnalyzer::TestJsonRecordInFile()
{
    QFETCH(AnalysisResult, result);
    {
        result.saveJson("test.json");
        AnalysisResult actual = actual.loadJson("test.json");
        //qWarning() << "I get actual " << actual.table_ << "I get result "<< result.table_;
        QCOMPARE(actual, result);

    }
}

void TAnalyzer::TestTimeRecordWrite_data()
{
    //typedef QList<TimeSeries> TimeSeriesList;
    QTest::addColumn<ComplexAnalyzer*>("analyzer");
    QTest::addColumn<int>("expectedResult");


    //1-st test
    QTest::newRow("TimeTests")
            << new ComplexAnalyzer(QList<Analyzer*>()
                                  << new AvgAnalyzer()
                                  << new DevAnalyzer()
                                  << new VarCoefAnalyzer()
                                  )
            << 1;
}

void TAnalyzer::TestTimeRecordWrite()
{
    typedef QList<TimeSeries> TimeSeriesList;
    QFETCH(int, expectedResult);
    QFETCH(ComplexAnalyzer*, analyzer);

    const QString databaseName = "TestTimeIDs.db";

    QVERIFY(TimeSeriesDBI::clear(databaseName));

    TimeSeriesList generate;
    for (int i = 0; i < 4000; i ++)
    {
        int tag = qrand();
        TimeSeries ts(QString::number(tag));
        for(int j = 0; j < 1000; j++)
        {
           ts.append( ((double)qrand()/(double)RAND_MAX));
        }
        generate.append(ts);
    }

    TimeSeriesDBI dbi(databaseName,1);
    QElapsedTimer timer;

//1-st Import
    timer.start();
    dbi.insertIntoTableFromOriginalType(generate);
    qWarning() << "Import operation took" << timer.elapsed() << "milliseconds";

    QList <QString> tags;
    for (int i = 0; i < generate.size(); i++)
    {
        tags.append(generate.value(i).id());
    }

//2-nd Analise
    timer.start();
    AnalysisResult anResult = analyzer->analyzeForIDs(&dbi, tags);
    qWarning() << "Analise operation took" << timer.elapsed() << "milliseconds";

//3-rd Export
    QString path = "Test100x1000.json";
    timer.start();
    anResult.saveJson(path);
    qWarning() << "Export operation took" << timer.elapsed() << "milliseconds";

    int actualResult = 1;
    QCOMPARE(actualResult, expectedResult);
    delete analyzer;
}



AnalysisResult ComplexAnalyzer::analyzeForIDs(TimeSeriesDBI *database, const QList<QString> &ids)
{
    AnalysisResult results;
    TimeSeriesList listTmSrs = database->timeSeriesFromString(ids);
    foreach (const TimeSeries &tsString, listTmSrs)
    {
        results.insertRow(tsString.id(), analyzeForID(tsString.id(), tsString));
        //qWarning() << "analyzeForIDs get " <<tsString.id();
    }
    return results;
}

AnalysisResult ComplexAnalyzer::analyzeForIDsTestMoc(DataInMemmoryMoc *database, const QList<QString> &ids)
{
    AnalysisResult results;
    foreach (const QString &id, ids)
    {
        results.insertRow(id, analyzeForID(id, database->read(id)));
    }
    return results;
}
