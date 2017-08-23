#ifndef TESTSPERSONAL_H
#define TESTSPERSONAL_H

#include <QtCore>
#include <QtTest/QTest>
#include <TimeSeries.h>
#include <Analyzer.h>
#include "DataInMemmoryMoc.h"
#include "TimeSeriesDBI.h"

//main class for analise tests
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

    //tests for Average function
    void TestAverage_data();
    void TestAverage();

    //tests for Deviation function
    void TestDeviation_data();
    void TestDeviation();

    //tests for Variation function
    void TestVariation_data();
    void TestVariation();

    //tests for Analyze function
    void TestAnalyze_data();
    void TestAnalyze();

    //tests for Project function
    void TestAnalysisResultProject_data();
    void TestAnalysisResultProject();

    //tests for AnalyzeForIDs function
    void TestAnalyzeForIDs_data();
    void TestAnalyzeForIDs();

    //tests for read-write json results
    void TestJsonRecord_data();
    void TestJsonRecord();

    //tests for read-write json results
    void TestJsonRecordInFile_data();
    void TestJsonRecordInFile();

    //tests for read-write json file
    void TestWriteReadRewrite_data();
    void TestWriteReadRewrite();
};

class TBenchAnalyzer : public QObject
{
    Q_OBJECT

private slots:

    //tests to determine the time
    void TestTimeRecordWrite_data();
    void TestTimeRecordWrite();
};

#endif // TESTSPERSONAL_H
