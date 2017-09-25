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
//       dbiTable_.insert("strArray", new TimeSeriesInArray());
      dbiTable_.insert("longTable", new TimeSeriesInLongTable());
//       dbiTable_.insert("json-doc", new TimeSeriesDocumentDBI());
//       dbiTable_.insert("inmemmory", new DataInMemmoryMoc());

}


void TTimeSeriesDBI::TestWriteReadRewireRead_data()
{
    QTest::addColumn<TimeSeriesDBI*>("dbi");
    QTest::addColumn<TimeSeriesList>("initTimeSeries");
    QTest::addColumn< QList<TimeSeriesID> >("initIDs");
    QTest::addColumn<TimeSeriesList>("additioanlTimeSeries");
    QTest::addColumn< QList<TimeSeriesID> >("additioanlIDs");
    QTest::addColumn<TimeSeriesList>("expectedAdditioanlTimeSeries");


    foreach(const QString &dbiName, dbiTable_.keys())
    {
        TimeSeriesDBI *dbi = dbiTable_.value(dbiName);
        QTest::newRow(QString("single_series " + dbiName).toLatin1())
                << dbi
                << (TimeSeriesList() << (TimeSeries("ts1") << 110.0))
                << (QList<TimeSeriesID>() << "ts1")
                << (TimeSeriesList())
                << (QList<TimeSeriesID>())
                << (TimeSeriesList());

        // 1
        QTest::newRow(QString("single_series_rewrite " + dbiName).toLatin1())
                << dbi
                << (TimeSeriesList() << (TimeSeries("ts1") << 110.0))
                << (QList<TimeSeriesID>() << "ts1")
                << (TimeSeriesList() << (TimeSeries("ts1") << 118.0))
                << (QList<TimeSeriesID>() << "ts1")
                << (TimeSeriesList() << (TimeSeries("ts1") << 118.0));
        // 2
        QTest::newRow(QString("2 series + 1 " + dbiName).toLatin1())
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
                << (QList<TimeSeriesID>() << "ts1" << "ts2")
                << (TimeSeriesList () << (TimeSeries("ts3")
                                          << 41110.0 << 11112 << 10004 << 14001
                                          << 4104 << 145 << 17615
                                          << 4140 << 452 << 17645
                                          << 4114 << 4235 << 17765
                                          << 4124 << 4541 << 17965
                                          << 4134 << 45571 << 18765))
                << (QList<TimeSeriesID>() << "ts1" << "ts2" << "ts3")
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
                        << 134 << 457 << 18765)
                    << (TimeSeries("ts3")
                        << 41110.0 << 11112 << 10004 << 14001
                        << 4104 << 145 << 17615
                        << 4140 << 452 << 17645
                        << 4114 << 4235 << 17765
                        << 4124 << 4541 << 17965
                        << 4134 << 45571 << 18765));



        QTest::newRow(QString("2 series simple " + dbiName).toLatin1())
                << dbi
                << (TimeSeriesList () << (TimeSeries("ts1")
                                          << 110.0)
                    << (TimeSeries("ts2")
                        << 112.0 ))
                << (QList<TimeSeriesID>() << "ts1" << "ts2")
                << (TimeSeriesList () << (TimeSeries("ts3")
                                          << 41110.0 ))
                << (QList<TimeSeriesID>() << "ts1" << "ts2" << "ts3")
                << (TimeSeriesList () << (TimeSeries("ts1")
                                          << 110.0)
                    << (TimeSeries("ts2")
                        << 112.0)
                    << (TimeSeries("ts3")
                        << 41110.0));

        // 3
        QTest::newRow(QString("2 series + rewrite + add " + dbiName).toLatin1())
                << dbi
                << (TimeSeriesList () << (TimeSeries("first")
                                          << 110.0 << 12 << 14 << 14
                                          )
                    << (TimeSeries("second")
                        << 110.0 << 12 << 114 << 714
                        << 104 << 45 << 17615
                        << 140 << 45 << 17645
                        << 114 << 425 << 17765
                        << 124 << 451 << 17965
                        << 134 << 457 << 18765))
                << (QList<TimeSeriesID>() << "first" << "second")
                << (TimeSeriesList () << (TimeSeries("first")
                                          << 41110.0 << 11112 << 10004 << 14001
                                          << 4104 << 145 << 17615
                                          << 4140 << 452 << 17645
                                          << 4114 << 4235 << 17765
                                          << 4124 << 4541 << 17965
                                          << 4134 << 45571 << 18765))
                << (QList<TimeSeriesID>() << "first" << "second")
                << (TimeSeriesList () << (TimeSeries("first")
                                          << 41110.0 << 11112 << 10004 << 14001
                                          << 4104 << 145 << 17615
                                          << 4140 << 452 << 17645
                                          << 4114 << 4235 << 17765
                                          << 4124 << 4541 << 17965
                                          << 4134 << 45571 << 18765)
                    << (TimeSeries("second")
                        << 110.0 << 12 << 114 << 714
                        << 104 << 45 << 17615
                        << 140 << 45 << 17645
                        << 114 << 425 << 17765
                        << 124 << 451 << 17965
                        << 134 << 457 << 18765));
        // 4
        QTest::newRow(QString("2 series + short TS + rewrite_" + dbiName).toLatin1())
                << dbi
                << (TimeSeriesList () << (TimeSeries("first")
                                          << 41110.0 << 11112 << 10004 << 14002
                                          )
                    << (TimeSeries("second")
                        << 110.0 << 12 << 114 << 714
                        << 104 << 45 << 17615
                        << 140 << 45 << 17645
                        << 114 << 425 << 17765
                        << 124 << 451 << 17965
                        << 134 << 457 << 18765))
                << (QList<TimeSeriesID>() << "first" << "second")
                << (TimeSeriesList () << (TimeSeries("first") << 425004))
                << (QList<TimeSeriesID>() << "first" << "second")
                << (TimeSeriesList () << (TimeSeries("first")
                                          << 425004.0 << 11112 << 10004
                                          << 14002 )
                    << (TimeSeries("second")
                        << 110.0 << 12 << 114 << 714
                        << 104 << 45 << 17615
                        << 140 << 45 << 17645
                        << 114 << 425 << 17765
                        << 124 << 451 << 17965
                        << 134 << 457 << 18765));
        // 5
        QTest::newRow(QString("2 series + empty TS + rewrite_" + dbiName).toLatin1())
                << dbi
                << (TimeSeriesList () << (TimeSeries("first")
                                          << 41110.0 << 11112 << 10004 << 14001
                                          << 4104 << 145 << 17615
                                          << 4140 << 452 << 17645
                                          << 4114 << 4235 << 17765
                                          << 4124 << 4541 << 17965
                                          << 4134 << 45571 << 18765
                                          )
                    << (TimeSeries("second")
                        << 110.0 << 12 << 114 << 714
                        << 104 << 45 << 17615
                        << 140 << 45 << 17645
                        << 114 << 425 << 17765
                        << 124 << 451 << 17965
                        << 134 << 457 << 18765))
                << (QList<TimeSeriesID>() << "first" << "second")
                << (TimeSeriesList () << (TimeSeries("first")))
                << (QList<TimeSeriesID>() << "first" << "second")
                << (TimeSeriesList ()
                    << (TimeSeries("first")
                        << 41110.0 << 11112 << 10004 << 14001
                        << 4104 << 145 << 17615
                        << 4140 << 452 << 17645
                        << 4114 << 4235 << 17765
                        << 4124 << 4541 << 17965
                        << 4134 << 45571 << 18765
                        )
                    << (TimeSeries("second")
                        << 110.0 << 12 << 114 << 714
                        << 104 << 45 << 17615
                        << 140 << 45 << 17645
                        << 114 << 425 << 17765
                        << 124 << 451 << 17965
                        << 134 << 457 << 18765));

        QTest::newRow(QString("partial_read_1_" + dbiName).toLatin1())
                << dbi
                << (TimeSeriesList() << (TimeSeries("id1") << 1) << (TimeSeries("id2") << 2))
                << (QList<TimeSeriesID>() << "id1" << "id2")
                << (TimeSeriesList())
                << (QList<TimeSeriesID>() << "id2")
                << (TimeSeriesList () << (TimeSeries("id2") << 2));

        QTest::newRow(QString("partial_read_2_" + dbiName).toLatin1())
                << dbi
                << (TimeSeriesList() << (TimeSeries("id1") << 1) << (TimeSeries("id2") << 2) << (TimeSeries("id3") << 3))
                << (QList<TimeSeriesID>() << "id1" << "id2" << "id3")
                << (TimeSeriesList())
                << (QList<TimeSeriesID>() << "id2" << "id3")
                << (TimeSeriesList () << (TimeSeries("id2") << 2)  << (TimeSeries("id3") << 3));
    }
}

void TTimeSeriesDBI::TestWriteReadRewireRead()
{
    QFETCH(TimeSeriesDBI*, dbi);
    QFETCH(TimeSeriesList, initTimeSeries);
    QFETCH(QList<TimeSeriesID>, initIDs);
    QFETCH(TimeSeriesList, additioanlTimeSeries);
    QFETCH(QList<TimeSeriesID>,  additioanlIDs);
    QFETCH(TimeSeriesList, expectedAdditioanlTimeSeries);

    const QString databaseName = QString(QTest::currentDataTag()) + "TestWriteReadRewireRead.db";
    //qWarning() << "I get init =" << initTimeSeries;
    QVERIFY2(dbi->remove(databaseName), QString("can't remove testing database %1").arg(databaseName).toLatin1());
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
    {
        TimeSeriesDBI *writeDBI = dbi->open(databaseName);
        writeDBI->write(additioanlTimeSeries);
        delete writeDBI;
    }
    {
        TimeSeriesDBI *readDBI = dbi->open(databaseName);
        const TimeSeriesList additioanlInitTimeSeries = readDBI->read(additioanlIDs);
        QCOMPARE(additioanlInitTimeSeries, expectedAdditioanlTimeSeries);
    }
    {
        TimeSeriesDBI *streamReader = dbi->open(databaseName);
        TimeSeriesDBI::TimeSeriesStream *stream = streamReader->stream(additioanlIDs);
        TimeSeriesList actual;
        while(stream->next())
        {
            actual.append(stream->current());
        }
        qSort(actual.begin(), actual.end());
        //qWarning() << "actual " << actual << "expectedAdditioanlTimeSeries" << expectedAdditioanlTimeSeries;
        QCOMPARE(actual, expectedAdditioanlTimeSeries);
        delete stream;
    }
    {
        TimeSeriesDBI *remover = dbi->open(databaseName);
        remover->remove(databaseName);
        delete remover;
    }
}


void TTimeSeriesDBI::TestReadComparisonJsons_data()
{
    QTest::addColumn<TimeSeriesDBI*>("dbi");
    QTest::addColumn<TimeSeriesList>("initTimeSeries");
    QTest::addColumn< QList<TimeSeriesID> >("initIDs");

    foreach(const QString &dbiName, dbiTable_.keys())
    {
        TimeSeriesDBI *dbi = dbiTable_.value(dbiName);
        QTest::newRow(QString("TestJson" + dbiName).toLatin1())
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

void TTimeSeriesDBI::TestReadComparisonJsons()
{
    QFETCH(TimeSeriesDBI*, dbi);
    QFETCH(TimeSeriesList, initTimeSeries);
    QFETCH(QList<TimeSeriesID>, initIDs);
    //qWarning() << "I'm Enter";
    const QString databaseName = QString(QTest::currentDataTag()) + "TestReadComparisonJsons.db";
    QVERIFY2(dbi->remove(databaseName), QString("can't remove testing database %1").arg(databaseName).toLatin1());
    {
        TimeSeriesDBI *writeDBI = dbi->open(databaseName);
        writeDBI->write(initTimeSeries);
        ComplexAnalyzer* anLse = new ComplexAnalyzer(QList<Analyzer*>()
                                                     << new AvgAnalyzer()
                                                     << new DevAnalyzer()
                                                     << new VarCoefAnalyzer()
                                                     );
        AnalysisResult result;
        result = anLse->analyzeForIDs(writeDBI, initIDs);
        QString path = QString(QTest::currentDataTag()) + "_Test.json";
        result.saveJson(path);
        writeDBI->remove(databaseName);
        delete anLse;
    }
    dbi->remove(databaseName);
}




void TTimeSeriesDBI::TestMissingPoints_data()
{
    QTest::addColumn<TimeSeriesDBI*>("dbi");
    QTest::addColumn<QJsonObject>("jsonObj");
    QTest::addColumn<QList<TimeSeriesID> >("initIDs");
    QTest::addColumn<TimeSeriesList>("expectedTimeSeries");
    QTest::addColumn<QJsonObject>("addJsonObj");
    QTest::addColumn<QList<TimeSeriesID> >("addInitIDs");
    QTest::addColumn<TimeSeriesList>("addExpectedTimeSeries");
    foreach(const QString &dbiName, dbiTable_.keys())
    {
        TimeSeriesDBI *dbi = dbiTable_.value(dbiName);
        //  1
        {
            QJsonArray array;
            QJsonObject obj1;
            QJsonObject obj2;
            obj1.insert("num",1);
            obj1.insert("value",1);
            array.append(obj1);
            obj2.insert("num",3);
            obj2.insert("value",3);
            array.append(obj2);
            QJsonObject object;
            object.insert("A",array);
            //qWarning() << object;
            QTest::newRow(QString("TestJson A-add-empty_" + dbiName).toLatin1())
                    << dbi
                    <<  object
                     << (QList<TimeSeriesID>() << "A")
                     << (TimeSeriesList () << (TimeSeries("A")
                                               << 1 << 1 << 3))
                     << QJsonObject()
                     << (QList<TimeSeriesID>() << "A")
                     << (TimeSeriesList () << (TimeSeries("A")
                                               << 1 << 1 << 3));
        }
        //  2
        {
            QJsonObject object1;
            QJsonObject object2;
            {
                QJsonArray array;
                QJsonObject obj1;
                QJsonObject obj2;
                obj1.insert("num",1);
                obj1.insert("value",1);
                array.append(obj1);
                obj2.insert("num",3);
                obj2.insert("value",3);
                array.append(obj2);
                object1.insert("A",array);
            }
            {
                QJsonArray array;
                QJsonObject obj1;
                QJsonObject obj2;
                obj1.insert("num",1);
                obj1.insert("value",2);
                array.append(obj1);
                obj2.insert("num",4);
                obj2.insert("value",9);
                array.append(obj2);
                object2.insert("B",array);
            }

            QTest::newRow(QString("TestJson A-add-B_" + dbiName).toLatin1())
                    << dbi
                    <<  object1
                     << (QList<TimeSeriesID>() << "A")
                     << (TimeSeriesList () << (TimeSeries("A")
                                               << 1 << 1 << 3))
                     << object2
                     << (QList<TimeSeriesID>() << "A" << "B")
                     << (TimeSeriesList () << (TimeSeries("A")
                                               << 1 << 1 << 3)
                         << (TimeSeries("B")
                             << 2 << 2 << 2 << 9));
        }
        //  3
        {
            QJsonObject object1;
            QJsonObject object2;
            {
                QJsonArray array;
                QJsonObject obj1;
                QJsonObject obj2;
                obj1.insert("num",1);
                obj1.insert("value",1);
                array.append(obj1);
                obj2.insert("num",3);
                obj2.insert("value",3);
                array.append(obj2);
                object1.insert("A",array);
            }
            {
                QJsonArray array;
                QJsonObject obj1;
                QJsonObject obj2;
                obj1.insert("num",1);
                obj1.insert("value",2);
                array.append(obj1);
                obj2.insert("num",4);
                obj2.insert("value",9);
                array.append(obj2);
                object2.insert("A",array);
            }
            QTest::newRow(QString("TestJson A-rewrite_" + dbiName).toLatin1())
                    << dbi
                    <<  object1
                     << (QList<TimeSeriesID>() << "A")
                     << (TimeSeriesList () << (TimeSeries("A")
                                               << 1 << 1 << 3))
                     << object2
                     << (QList<TimeSeriesID>() << "A")
                     << (TimeSeriesList () << (TimeSeries("A")
                                               << 2 << 2 << 2 << 9));
        }
        //  3.1
        {
            QJsonObject object1;
            QJsonObject object2;
            {
                QJsonArray array;
                QJsonObject obj1;
                QJsonObject obj2;
                obj1.insert("num",1);
                obj1.insert("value",1);
                array.append(obj1);
                obj2.insert("num",3);
                obj2.insert("value",3);
                array.append(obj2);
                object1.insert("A",array);
            }
            {
                QJsonArray array;
                QJsonObject obj1;
                QJsonObject obj2;
                obj1.insert("num",2);
                obj1.insert("value",2);
                array.append(obj1);
                obj2.insert("num",4);
                obj2.insert("value",9);
                array.append(obj2);
                object2.insert("A",array);
            }
            QTest::newRow(QString("TestJson A-rewriteInside_" + dbiName).toLatin1())
                    << dbi
                    <<  object1
                     << (QList<TimeSeriesID>() << "A")
                     << (TimeSeriesList () << (TimeSeries("A")
                                               << 1 << 1 << 3))
                     << object2
                     << (QList<TimeSeriesID>() << "A")
                     << (TimeSeriesList () << (TimeSeries("A")
                                               << 1 << 2 << 2 << 9));
        }
        //  3.2
        {
            QJsonObject object1;
            QJsonObject object2;
            {
                QJsonArray array;
                QJsonObject obj1;
                QJsonObject obj2;
                obj1.insert("num",3);
                obj1.insert("value",1);
                array.append(obj1);
                obj2.insert("num",5);
                obj2.insert("value",3);
                array.append(obj2);
                object1.insert("A",array);
            }
            {
                QJsonArray array;
                QJsonObject obj1;
                QJsonObject obj2;
                obj1.insert("num",1);
                obj1.insert("value",2);
                array.append(obj1);
                obj2.insert("num",4);
                obj2.insert("value",9);
                array.append(obj2);
                object2.insert("A",array);
            }
            QTest::newRow(QString("TestJson A-rewritePre_" + dbiName).toLatin1())
                    << dbi
                    <<  object1
                     << (QList<TimeSeriesID>() << "A")
                     << (TimeSeriesList () << (TimeSeries("A")
                                               << 1 << 1 << 3))
                     << object2
                     << (QList<TimeSeriesID>() << "A")
                     << (TimeSeriesList () << (TimeSeries("A")
                                               << 2 << 2 << 2 << 9 << 3));
        }
        //  3.3
        {
            QJsonObject object1;
            QJsonObject object2;
            {
                QJsonArray array;
                QJsonObject obj1;
                QJsonObject obj2;
                obj1.insert("num",1);
                obj1.insert("value",1);
                array.append(obj1);
                obj2.insert("num",3);
                obj2.insert("value",3);
                array.append(obj2);
                object1.insert("A",array);
            }
            {
                QJsonArray array;
                QJsonObject obj1;
                QJsonObject obj2;
                obj1.insert("num",5);
                obj1.insert("value",2);
                array.append(obj1);
                obj2.insert("num",7);
                obj2.insert("value",9);
                array.append(obj2);
                object2.insert("A",array);
            }
            QTest::newRow(QString("TestJson A-rewritePost_" + dbiName).toLatin1())
                    << dbi
                    <<  object1
                     << (QList<TimeSeriesID>() << "A")
                     << (TimeSeriesList () << (TimeSeries("A")
                                               << 1 << 1 << 3))
                     << object2
                     << (QList<TimeSeriesID>() << "A")
                     << (TimeSeriesList () << (TimeSeries("A")
                                               << 1 << 1 << 3 << 3 << 2 << 2 << 9));
        }
        //  4
        {
            QJsonObject object1;
            {
                QJsonArray array;
                QJsonObject obj1;
                QJsonObject obj2;
                obj1.insert("num",4);
                obj1.insert("value",1);
                array.append(obj1);
                obj2.insert("num",1);
                obj2.insert("value",3);
                array.append(obj2);
                object1.insert("A",array);
            }

            QTest::newRow(QString("TestJson A-reverse_" + dbiName).toLatin1())
                    << dbi
                    <<  object1
                     << (QList<TimeSeriesID>() << "A")
                     << (TimeSeriesList () << (TimeSeries("A")
                                               << 3 << 3 << 3 << 1))
                     << QJsonObject()
                     << (QList<TimeSeriesID>() << "A")
                     << (TimeSeriesList () << (TimeSeries("A")
                                               << 3 << 3 << 3 << 1));
        }
        //  5
        {
            QJsonObject object1;
            {
                QJsonArray array;
                QJsonObject obj1;
                QJsonObject obj2;
                QJsonObject obj3;
                obj1.insert("num",4);
                obj1.insert("value",1);
                array.append(obj1);
                obj2.insert("num",2);
                obj2.insert("value",3);
                array.append(obj2);
                obj3.insert("num",6);
                obj3.insert("value",4);
                array.append(obj3);
                object1.insert("A",array);
            }

            QTest::newRow(QString("TestJson A-passes_" + dbiName).toLatin1())
                    << dbi
                    <<  object1
                     << (QList<TimeSeriesID>() << "A")
                     << (TimeSeriesList () << (TimeSeries("A")
                                               << 3 << 3 << 1 << 1 << 4))
                     << QJsonObject()
                     << (QList<TimeSeriesID>() << "A")
                     << (TimeSeriesList () << (TimeSeries("A")
                                               << 3 << 3 << 1 << 1 << 4));
        }
    }
}

void TTimeSeriesDBI::TestMissingPoints(){
    QFETCH(TimeSeriesDBI*, dbi);
    QFETCH(QJsonObject, jsonObj);
    QFETCH(QList<TimeSeriesID>, initIDs);
    QFETCH(TimeSeriesList, expectedTimeSeries);
    QFETCH(QJsonObject, addJsonObj);
    QFETCH(QList<TimeSeriesID>, addInitIDs);
    QFETCH(TimeSeriesList, addExpectedTimeSeries);

    const QString databaseName = QString(QTest::currentDataTag()) + "TestJson.db";
    const QString jsonFileName = QString(QTest::currentDataTag()) + "TestMissingPointsJson.json";
    if(QFile::exists(jsonFileName))
    {
        QVERIFY2(QFile::remove(jsonFileName), QString("can't remove testing json %1").arg(jsonFileName).toLatin1());
    }
    QVERIFY2(dbi->remove(databaseName), QString("can't remove testing database %1").arg(databaseName).toLatin1());
    {
        QFile jsonFile(jsonFileName);
        QJsonDocument doc(jsonObj);
        if(jsonFile.open(QIODevice::Text|QIODevice::WriteOnly))
        {
            QTextStream stream(&jsonFile);
            stream << doc.toJson();
        }
        {
            TimeSeriesDBI *writeDBI = dbi->open(databaseName);
            writeDBI->loadDataFromFile(jsonFileName);
        }
        {
            TimeSeriesDBI *readDBI = dbi->open(databaseName);
            const TimeSeriesList actualTSList = readDBI->read(initIDs);
            if(QFile::exists(databaseName))
            {
                QFile::remove(jsonFileName);
            }
            QCOMPARE(actualTSList, expectedTimeSeries);
        }

        doc.setObject(addJsonObj);
        jsonFile.setFileName(jsonFileName);
        if(jsonFile.open(QIODevice::Text|QIODevice::WriteOnly))
        {
            QTextStream stream(&jsonFile);
            stream << doc.toJson();
        }
        {
            TimeSeriesDBI *writeDBI = dbi->open(databaseName);
            writeDBI->loadDataFromFile(jsonFileName);
        }
        {
            TimeSeriesDBI *readDBI = dbi->open(databaseName);
            const TimeSeriesList actualTSList = readDBI->read(addInitIDs);
            if(QFile::exists(databaseName))
            {
                QFile::remove(jsonFileName);
            }
            dbi->remove(databaseName);
            QCOMPARE(actualTSList, addExpectedTimeSeries);
        }
    }
    if(QFile::exists(jsonFileName))
    {
        QFile::remove(jsonFileName);
    }
}

void TTimeSeriesDBI::TestFromBench_data()
{
    QTest::addColumn<TimeSeriesDBI*>("dbi");
    QTest::addColumn<TimeSeriesList>("inputTimeSeries1");
    QTest::addColumn<TimeSeriesList>("inputTimeSeries2");
    QTest::addColumn<QList<TimeSeriesID> >("addInitIDs");
    QTest::addColumn<TimeSeriesList>("addExpectedTimeSeries");
    foreach(const QString &dbiName, dbiTable_.keys())
    {
        TimeSeriesDBI *dbi = dbiTable_.value(dbiName);
        //  1

        QTest::newRow(QString("TestJson BenchSimulator_" + dbiName).toLatin1())
                << dbi
                <<  (TimeSeriesList() << (TimeSeries("A") << 1 << 5 << 7 << 9)
                     << (TimeSeries("B") << 5 << 45 << 98 << 11))
                 << (TimeSeriesList() << (TimeSeries("A") << 1 << 2 << 3 << 4)
                     << (TimeSeries("B") << 7 << 8 << 9 << 6))
                 << (QList<TimeSeriesID>() << "A" << "B")
                 << (TimeSeriesList() << (TimeSeries("A") << 1 << 2 << 3 << 4)
                     << (TimeSeries("B") << 7 << 8 << 9 << 6));
    }


}

void TTimeSeriesDBI::TestFromBench()
{
    typedef QList<TimeSeries> TimeSeriesList;
    QFETCH(TimeSeriesDBI*, dbi);
    QFETCH(TimeSeriesList, inputTimeSeries1);
    QFETCH(TimeSeriesList, inputTimeSeries2);
    QFETCH(TimeSeriesList, addExpectedTimeSeries);
    QFETCH(QList<TimeSeriesID>, addInitIDs);

    const QString databaseName = "TestFromBench";

    QVERIFY(dbi->remove(databaseName));

    {
        TimeSeriesDBI *writer = dbi->open(databaseName);
        writer->write(inputTimeSeries1);
        writer->write(inputTimeSeries2);
    }

    {
        TimeSeriesDBI *reader = dbi->open(databaseName);
        TimeSeriesList actual = reader->read(addInitIDs);
        QCOMPARE(actual, addExpectedTimeSeries);
    }

    dbi->remove(databaseName);
}


void TTimeSeriesDBI::lackOfZeros_data()
{
    QTest::addColumn<TimeSeriesDBI*>("dbi");
    QTest::addColumn<QJsonObject>("jsonObj");
    QTest::addColumn<QList<TimeSeriesID> >("initIDs");
    QTest::addColumn<TimeSeriesList>("expectedTimeSeries");

    foreach(const QString &dbiName, dbiTable_.keys())
    {
        TimeSeriesDBI *dbi = dbiTable_.value(dbiName);
        //  1
        {
            QJsonArray array;
            QJsonObject obj1;
            QJsonObject obj2;
            QJsonObject obj3;
            QJsonObject obj4;
            obj1.insert("num",1);
            obj1.insert("value",1);
            array.append(obj1);
            obj2.insert("num",3);
            obj2.insert("value",0);
            array.append(obj2);
            obj3.insert("num",4);
            obj3.insert("value",0);
            array.append(obj3);
            obj4.insert("num",5);
            obj4.insert("value",2);
            array.append(obj4);
            QJsonObject object;
            object.insert("A",array);
            //qWarning() << object;


            QTest::newRow(QString("TestJson 2-unzero+2-zeros " + dbiName).toLatin1())
                    << dbi
                    <<  object
                     << (QList<TimeSeriesID>() << "A")
                     << (TimeSeriesList () << (TimeSeries("A")
                                               << 1 << 1 << 0 << 0 << 2));
        }
    }
}

void TTimeSeriesDBI::lackOfZeros()
{
    QFETCH(TimeSeriesDBI*, dbi);
    QFETCH(QJsonObject, jsonObj);
    QFETCH(QList<TimeSeriesID>, initIDs);
    QFETCH(TimeSeriesList, expectedTimeSeries);
    const QString databaseName = QString(QTest::currentDataTag()) + "Test_lackOfZeros.db";
    const QString jsonFileName = QString(QTest::currentDataTag()) + "Test_lackOfZeros.json";
    {
        QFile jsonFile(jsonFileName);
        QJsonDocument doc(jsonObj);
        if(jsonFile.open(QIODevice::Text|QIODevice::WriteOnly))
        {
            QTextStream stream(&jsonFile);
            stream << doc.toJson();
        }
        {
            TimeSeriesDBI *writeDBI = dbi->open(databaseName);
            writeDBI->loadDataFromFile(jsonFileName);
            QTest::qSleep(10000);
        }
        {
            TimeSeriesDBI *readDBI = dbi->open(databaseName);
            const TimeSeriesList actualTSList = readDBI->read(initIDs);
            readDBI->remove(databaseName);
            if(QFile::exists(databaseName))
            {
                QFile::remove(jsonFileName);
            }
            QCOMPARE(actualTSList, expectedTimeSeries);
        }
    }
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
    //dbiTableBench_.insert("string_doc_", new TimeSeriesInArray());
    //dbiTableBench_.insert("json_", new TimeSeriesDocumentDBI());

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
    qsrand(42);
    typedef QList<TimeSeries> TimeSeriesList;
    QFETCH(int, expectedResult);
    QFETCH(ComplexAnalyzer*, analyzer);
    QFETCH(TimeSeriesDBI*, dbiT);
    QFETCH(QString, databaseName);
    QVERIFY(dbiT->remove(databaseName));
    TimeSeriesList generate;
    const int sizeRand = 10000;

    for(int i = 0; i < sizeRand; i++)
    {
        TimeSeries ts(QString::number(i));
        const int size = 1000 /*(qrand() % 999) + 1*/;
        for (int j = 0; j < size; j ++)
        {
            if (j % 10 == 0)
            {
                ts.insert(j, qrand() % 1000);
            }
            else if(j % 100 == 0)
            {
                ts.insert(j, 0);
            }
//            else
//            {
//                ts.append(0);
//            }
        }
        generate.append(ts);
    }


    //    for(int i = 0; i < sizeRand; i++)
    //    {
    //        TimeSeries ts(QString::number(i));
    //        const int size = 110 /*(qrand() % 999) + 1*/;
    //        for (int j = 0; j < size; j ++)
    //        {
    //            if (j % 10 == 0)
    //            {
    //                ts.append(0);
    //            }
    //            else
    //            {
    //                ts.append(qrand() % 1000);
    //            }
    //        }
    //        generate.append(ts);
    //    }


    //    for(int i = 0; i < sizeRand; i++)
    //    {
    //        TimeSeries ts(QString::number(i));
    //        if (i % 10 == 0)
    //        {
    //            const int size = 1000 /*(qrand() % 999) + 1*/;
    //            const int sizeNull = qrand() % 30;
    //            for(int j = 0; j < sizeNull; j++)
    //            {
    //                ts.append(qrand() % 1000);
    //            }
    //            for(int j = 0; j < size - sizeNull; j++)
    //            {
    //                ts.append(0);
    //            }
    //        }
    //        else
    //        {
    //            const int size = 1000 /*(qrand() % 999) + 1*/;
    //            const int sizeNull = (qrand() % 969) + 30;
    //            for(int j = 0; j < sizeNull; j++)
    //            {
    //                ts.append(qrand() % 1000);
    //            }
    //            for(int j = 0; j < size - sizeNull; j++)
    //            {
    //                ts.append(0);
    //            }
    //        }
    //        generate.append(ts);
    //    }

    //    for(int i = 0; i < 1000; i++)
    //    {
    //        const int size = 1000 /*(qrand() % 999) + 1*/;
    //        TimeSeries ts(QString::number(i));
    //        const int sizeNull = (qrand() % 969) + 30;
    //        for(int j = 0; j < sizeNull; j++)
    //        {
    //            ts.append(qrand() % 1000);
    //        }
    //        for(int j = 0; j < size - sizeNull; j++)
    //        {
    //            ts.append(0);
    //        }
    //        generate.append(ts);
    //    }


    //////

    qWarning() << generate.size();
    TimeSeriesDBI *dbi = dbiT->open(databaseName);
    QElapsedTimer timer;
    {//1-st Import
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

        /*timer.start();
        for(int timeLapse = 0; timeLapse < 10; timeLapse ++)
        {
            TimeSeriesDBI *streamReader = dbi->open(databaseName);
            TimeSeriesDBI::TimeSeriesStream *stream = streamReader->stream(tags);
            TimeSeriesList actual;
            while(stream->next())
            {
                actual.append(stream->current());
            }
        }
        qWarning() << "streamReader operation took" << timer.elapsed() << "milliseconds";*/

        //2-nd Analise
        qWarning() << 1;
        QTest::qSleep(5000);
        qWarning() << -1;
        timer.start();
        AnalysisResult result;
        result = analyzer->analyzeForIDsStream(dbi, tags);
        qWarning() << "Analise operation took" << timer.elapsed() << "milliseconds";

        qWarning() << 1;
        QTest::qSleep(5000);
        qWarning() << -1;
        //3-rd Expor
        QString path = databaseName + "Test100x4000.json";
        timer.start();
        result.saveJson(path);
        qWarning() << "Export operation took" << timer.elapsed() << "milliseconds";
        int actualResult = 1;
        QVERIFY(actualResult == expectedResult);
        if(QFile::exists(path))
        {
            QFile::remove(path);
        }
    }

    generate.clear();
    for(int i = 0; i < sizeRand; i++)
    {
        const int size = 1000 /*(qrand() % 999) + 1*/;
        TimeSeries ts(QString::number(i));
        for(int j = 0; j < size; j++)
        {
            if(qrand() % 3 == 0)
            {
                ts.append(0);
            }
            else
            {
                ts.append(qrand() % 1000);
            }
        }
        generate.append(ts);
    }
    qWarning() << generate.size();

    //1-st Add Import
    timer.start();
    dbi->write(generate);


    qWarning() << databaseName;
    qWarning() << "Add Import operation took" << timer.elapsed() << "milliseconds";
    QList<QString> tags;
    for (int i = 0; i < generate.size(); i++)
    {
        tags.append(generate.value(i).id());
    }

    //2-nd Add Analise
    timer.start();
    AnalysisResult results;
    QCOMPARE(generate.size(), tags.size());
    results = analyzer->analyzeForIDsStream(dbi, tags);

    QVERIFY2(results.getTable().size() == generate.size(), QString("value results is %1 generate - %2").arg(results.getTable().size()).arg(generate.size()).toLatin1());
    qWarning() << "Add Analise operation took" << timer.elapsed() << "milliseconds";

    //3-rd Add Export
    QString path = databaseName + "AddTest100x4000.json";
    timer.start();
    results.saveJson(path);
    qWarning() << "Add Export operation took" << timer.elapsed() << "milliseconds";
    int actualResult = 1;
    QCOMPARE(actualResult, expectedResult);

    AnalysisResult actual = actual.loadJson(path);
    QCOMPARE(actual, results);

    if(QFile::exists(path))
    {
        QFile::remove(path);
    }
    dbiT->remove(databaseName);
    delete analyzer;
}
