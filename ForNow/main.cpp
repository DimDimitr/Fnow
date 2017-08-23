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

    ///todo разобраться с классом QCommandLineOption
    //Option for using moc DB in tests
    QCommandLineOption runMoc(QStringList() << "moc_base",
                            QCoreApplication::translate("main", "Run tests on moc_base"));
    parser.addOption(runMoc);

    //Option for runing only tests
    QCommandLineOption runTestsOption(QStringList() << "run_tests",
                                      QCoreApplication::translate("main", "Run tests"));
    parser.addOption(runTestsOption);

    //Option for runing only bench
    QCommandLineOption runBenchOption(QStringList() << "run_bench",
                                      QCoreApplication::translate("main", "Run bench"));
    parser.addOption(runBenchOption);

    parser.process(a);
 /**/   ///todo всё что может быть const должно быть const
 /**/   /// даваь переменным более осмысленные имена, особенно булевым флагам, например тут может быть isRunTestsFlag
    const bool isRunTestsFlag = parser.isSet(runTestsOption);
    const bool isRunBenchFlag = parser.isSet(runBenchOption);
 /**/   ///todo пробелы между бинарными операторами
    int variantOfTypeDB = 0;
 /**/  ///todo всегда выделять тела условий и циклов в {}
    if (parser.isSet(runMoc))
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
        TBenchAnalyzer benchAnaliser;
        QTest::qExec(&benchAnaliser);
    }

    if(!isRunTestsFlag && !isRunBenchFlag)
    {
        View *view = new View();
        view->show();
    }

    return a.exec();
}
