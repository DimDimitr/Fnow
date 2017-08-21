//#include "mainwindow.h"
#include"view.h"
#include <QApplication>

#include "Analyzer.h"
#include "DataInMemmoryMoc.h"
#include "TimeSeriesDBI.h"
#include <QCommandLineParser>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCommandLineParser parser;
    QCommandLineOption runMoc(QStringList() << "moc_base",
                            QCoreApplication::translate("main", "Overwrite existing files."));
    parser.addOption(runMoc);

    QCommandLineOption runReal(QStringList() << "real_base",
                                      QCoreApplication::translate("main", "Overwrite existing files."));
    parser.addOption(runReal);

    QCommandLineOption runTestsOption(QStringList() << "run_tests",
                                      QCoreApplication::translate("main", "Overwrite existing files."));

    parser.addOption(runTestsOption);
    QCommandLineOption runBenchOption(QStringList() << "run_bench",
                                      QCoreApplication::translate("main", "Overwrite existing files."));
    parser.addOption(runBenchOption);
    parser.process(a);
    bool tests = parser.isSet(runTestsOption);
    bool bench = parser.isSet(runBenchOption);

    if (tests)
    {
        TAnalyzer tAnalyzer;
        qWarning() << "Start Analizer";
        QTest::qExec(&tAnalyzer);
        TDataInMemmoryMoc tTimeSeriesDatabase;
        QTest::qExec(&tTimeSeriesDatabase);
        TTimeSeriesDBI TestWriteReadDatBase;
        QTest::qExec(&TestWriteReadDatBase);
    }
    if (bench)
    {
        TBenchAnalyzer benchAnaliser;
        QTest::qExec(&benchAnaliser);
    }
    else
    {

        TTimeSeriesDBI testDbi;
        QTest::qExec(&testDbi);



        View *view = new View();
        view->show();
    }

    return a.exec();
}
