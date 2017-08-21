//#include "mainwindow.h"
#include"view.h"
#include <QApplication>

#include "Analyzer.h"
#include "DataInMemmoryMoc.h"
#include "TimeSeriesDBI.h"


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    QCommandLineParser parser;

    QCommandLineOption runTestsOption(QStringList() << "run" << "tests",
                                      QCoreApplication::translate("main", "Overwrite existing files."));
    parser.addOption(runTestsOption);
    QCommandLineOption runBenchOption(QStringList() << "run" << "bench",
                                      QCoreApplication::translate("main", "Overwrite existing files."));
    parser.addOption(runBenchOption);
    parser.process(a);
    bool tests = parser.isSet(runTestsOption);
    bool bench = parser.isSet(runBenchOption);

    if (tests)
    {
    TAnalyzer tAnalyzer;
    QTest::qExec(&tAnalyzer, argc, argv);

    TDataInMemmoryMoc tTimeSeriesDatabase;
    QTest::qExec(&tTimeSeriesDatabase, argc, argv);

    TTimeSeriesDBI TestWriteReadDatBase;
    QTest::qExec(&TestWriteReadDatBase, argc, argv);
    }
    else if (bench)
    {

    }
    else
    {
    View *view = new View();
    view->show();
    }

    return a.exec();
}
