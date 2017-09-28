#ifndef TTIMESERIESDBI_H

#define TTIMESERIESDBI_H
#include <QObject>

#include <TimeSeriesDBI.h>

inline char *toString(const TimeSeries &ts);

class TTimeSeriesDBI : public QObject
{
public:
    TTimeSeriesDBI(int choose);

    Q_OBJECT
private slots:
    void TestWriteReadRewireRead_data();
    void TestWriteReadRewireRead();

    void TestReadComparisonJsons_data();
    void TestReadComparisonJsons();

    void TestMissingPoints_data();
    void TestMissingPoints();

    void TestFromBench_data();
    void TestFromBench();

    void lackOfZeros_data();
    void lackOfZeros();
private:
    QHash<QString, QSharedPointer<TimeSeriesDBI> > dbiTable_;
};



class TBenchAnalyzer : public QObject
{

public:
    TBenchAnalyzer(bool choose);

    Q_OBJECT
private slots:
    //tests to determine the time
    void BenchmarkImportAnalizeExport_data();
    void BenchmarkImportAnalizeExport();

private:
    QHash<QString, TimeSeriesDBI*> dbiTableBench_;
};



#endif // TTIMESERIESDBI_H
