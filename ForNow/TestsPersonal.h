#ifndef TESTSPERSONAL_H
#define TESTSPERSONAL_H

#include <QtCore>
#include <QtTest/QTest>
#include <TimeSeries.h>
#include <Analyzer.h>
#include "DataInMemmoryMoc.h"
#include "TimeSeriesDBI.h"

class TestsPersonal
{
public:
    TestsPersonal();
};






class TAnalyzer : public QObject
{
    private: int typeDatBase;
public:
    TAnalyzer(int key)
    {
    typeDatBase = key;
    }

void setType (int type)
{
     typeDatBase = type;
}


        Q_OBJECT
private slots:
    void TestAverage_data();
    void TestAverage();

    void TestDeviation_data();
    void TestDeviation();

    void TestVariation_data();
    void TestVariation();

    void TestAnalyze_data();
    void TestAnalyze();

    void TestAnalysisResultProject_data();
    void TestAnalysisResultProject();

    void TestAnalyzeForIDs_data();
    void TestAnalyzeForIDs();

    void TestJsonRecord_data();
    void TestJsonRecord();

    void TestJsonRecordInFile_data();
    void TestJsonRecordInFile();

    void TestWriteReadRewrite_data();
    void TestWriteReadRewrite();
};

class TBenchAnalyzer : public QObject
{
    Q_OBJECT

private slots:
    void TestTimeRecordWrite_data();
    void TestTimeRecordWrite();
};

/*class TTimeSeriesDBI : public QObject
{
    Q_OBJECT
private slots:
    void TestWriteReadRewrite_data();
    void TestWriteReadRewrite();
};*/

#endif // TESTSPERSONAL_H
