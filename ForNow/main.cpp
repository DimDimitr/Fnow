#include"view.h"
#include <QApplication>

#include "Analyzer.h"
#include "DataInMemmoryMoc.h"
#include "TimeSeriesDBI.h"
#include <QCommandLineParser>
#include <TestsPersonal.h>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCommandLineParser parser;

    //Option for using moc DB in tests
    QCommandLineOption runMoc(QStringList() << "moc_base",
                            QCoreApplication::translate("main", "Overwrite existing files."));
    parser.addOption(runMoc);

    //Option for runing only tests
    QCommandLineOption runTestsOption(QStringList() << "run_tests",
                                      QCoreApplication::translate("main", "Overwrite existing files."));
    parser.addOption(runTestsOption);

    //Option for runing only bench
    QCommandLineOption runBenchOption(QStringList() << "run_bench",
                                      QCoreApplication::translate("main", "Overwrite existing files."));
    parser.addOption(runBenchOption);

    parser.process(a);
    bool tests = parser.isSet(runTestsOption);
    bool bench = parser.isSet(runBenchOption);
    int keyType=0;
    if (parser.isSet(runMoc))
        keyType = 1;

    if (tests)
    {
        TAnalyzer tAnalyzer(keyType);
        qWarning() << "Start Analizer";
        QTest::qExec(&tAnalyzer);
    }
    if (bench)
    {
        TBenchAnalyzer benchAnaliser;
        QTest::qExec(&benchAnaliser);
        return a.exec();
    }
    if(!tests && !bench)
    {
        View *view = new View();
        view->show();
    }

    return a.exec();
}
