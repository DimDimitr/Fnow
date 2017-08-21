//#include "mainwindow.h"
#include"view.h"
#include <QApplication>

#include "Analyzer.h"
#include "DataInMemmoryMoc.h"
#include "TimeSeriesDBI.h"


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    TAnalyzer tAnalyzer;
    QTest::qExec(&tAnalyzer, argc, argv);

    TDataInMemmoryMoc tTimeSeriesDatabase;
    QTest::qExec(&tTimeSeriesDatabase, argc, argv);

    TTimeSeriesDBI TestWriteReadDatBase;
    QTest::qExec(&TestWriteReadDatBase, argc, argv);

    View *view = new View();
    view->show();
    //qWarning() << "End of f";
    return a.exec();
}
