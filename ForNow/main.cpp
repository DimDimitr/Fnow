#include"view.h"
#include <QApplication>

#include "Analyzer.h"
#include "DataInMemmoryMoc.h"
#include "TimeSeriesDBI.h"
#include <QCommandLineParser>
#include <TestsPersonal.h>
#include <TTimeSeriesDBI.h>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCommandLineParser parser;

    //Option for using moc DB in tests
    QCommandLineOption runMocOption(QStringList() << "moc_base",
                            QCoreApplication::translate("main", "Run tests on moc_base"));
    parser.addOption(runMocOption);

    //Option for runing only tests
    QCommandLineOption runTestsOption(QStringList() << "run_tests",
                                      QCoreApplication::translate("main", "Run tests"));
    parser.addOption(runTestsOption);

    //Option for runing only bench
    QCommandLineOption runBenchOption(QStringList() << "run_bench",
                                      QCoreApplication::translate("main", "Run bench"));
    parser.addOption(runBenchOption);

    parser.process(a);

    const bool isRunTestsFlag = parser.isSet(runTestsOption);
    const bool isRunBenchFlag = parser.isSet(runBenchOption);
    const bool isRunMocBase = parser.isSet(runMocOption);

    int variantOfTypeDB = 0;

    if (isRunMocBase)
    {
        variantOfTypeDB = 1;
    }
    if (isRunTestsFlag)
    {
        TAnalyzer tAnalyzer(variantOfTypeDB);
        QTest::qExec(&tAnalyzer);

        TTimeSeriesDBI tTimeSeriesDBI(variantOfTypeDB);
        QTest::qExec(&tTimeSeriesDBI);

    }
    if (isRunBenchFlag)
    {
        TBenchAnalyzer benchAnaliser(isRunMocBase);
        QTest::qExec(&benchAnaliser);
    }

    if(!isRunTestsFlag && !isRunBenchFlag)
    {
        View *view = new View();
        view->show();
    }

    return a.exec();
}
