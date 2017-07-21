#include "mainwindow.h"
#include <QApplication>
#include <database.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DbManager z("D:/QTProjects/ForNow/1.db");
    int p=5;
    double aa=0.25,b=0.35,c=0.85,d=99.9;
    int rez=z.Insert_to_DB(p,aa,b,c,d);
    rez=z.Insert_to_DB(p*2,aa,b,c,d);
    qDebug() << rez;
    z.PrintAll();
    MainWindow w;
    w.show();

    return a.exec();
}
