//#include "mainwindow.h"
#include"view.h"
#include <QApplication>

#include "Analyzer.h"
#include "DataInMemmoryMoc.h"
#include "TimeSeriesDBI.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    View *view = new View();

    TAnalyzer tAnalyzer;
    QTest::qExec(&tAnalyzer, argc, argv);

    TDataInMemmoryMoc tTimeSeriesDatabase;
    QTest::qExec(&tTimeSeriesDatabase, argc, argv);

    TAnalyzer tTestAnalyzeForIDs;
    QTest::qExec(&tTestAnalyzeForIDs, argc, argv);

    TTimeSeriesDBI TestWriteReadDatBase;
    QTest::qExec(&TestWriteReadDatBase, argc, argv);

    view->show();
    return a.exec();
}
