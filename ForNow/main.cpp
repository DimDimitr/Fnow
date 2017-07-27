#include "mainwindow.h"

#include <QApplication>

#include "database.h"
#include "Analyzer.h"
#include "TimeSeriesDatabase.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //    MainWindow w;
    //    w.show();

    TAnalyzer tAnalyzer;
    QTest::qExec(&tAnalyzer, argc, argv);

    TTimeSeriesDatabase tTimeSeriesDatabase;
    QTest::qExec(&tTimeSeriesDatabase, argc, argv);

    return 0;
    // return a.exec();
}
