//#include "mainwindow.h"
#include"view.h"
#include <QApplication>

#include "Analyzer.h"
#include "TimeSeriesDBI.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //    MainWindow w;
    //    w.show();
    View *view = new View();

    TAnalyzer tAnalyzer;
    //QTest::qExec(&tAnalyzer, argc, argv);

    TTimeSeriesDBI tTimeSeriesDatabase;
    //QTest::qExec(&tTimeSeriesDatabase, argc, argv);
    view->show();
    return a.exec();
}
