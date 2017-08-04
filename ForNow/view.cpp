#include "view.h"
#include "database.h"


View::View(QWidget *parent) : QDialog(parent)
{
    initState();
    initModels();
    initView();
    initLogic();
    update();
}

void View::loadFile()
{
    QFile file;
    ///review20170727 ставить пробел с обоих сторон = или других бинарных операторов, кроме .  и ->
    globPath = QFileDialog::getOpenFileName(NULL,"","C:/fnow-studying/.","*.json");
    file.setFileName(globPath);
    if (file.open(QIODevice::ReadOnly|QFile::Text))
    {
        doc = QJsonDocument::fromJson(QByteArray(file.readAll()),&docError);
        ///review20170727 использовать qWarning() вместо qDebug(), а ещё лучше собственную функцию вывода
    }
    file.close();

    if(docError.errorString().toInt() == QJsonParseError::NoError)
    {
        QStandardItemModel *model = new QStandardItemModel(NULL);
        model -> setHorizontalHeaderLabels(QStringList() << "Point" << "A" << "B" << "C" << "D");

        DbManager z("D:/QTProjects/ForNow/4.db");

        docArr = QJsonValue(doc.object().value("Points")).toArray();
        for (int i = 0; i < docArr.count();i++)
        {

            int P_w = docArr.at(i).toObject().value("Point").toInt();
            double A_w = docArr.at(i).toObject().value("A").toDouble();
            double B_w = docArr.at(i).toObject().value("B").toDouble();
            double C_w = docArr.at(i).toObject().value("C").toDouble();
            double D_w = docArr.at(i).toObject().value("D").toDouble();

            QStandardItem* point = new QStandardItem (QString::number(P_w));
            QStandardItem* A = new QStandardItem (QString::number(A_w));
            QStandardItem* B = new QStandardItem (QString::number(B_w));
            QStandardItem* C = new QStandardItem (QString::number(C_w));
            QStandardItem* D = new QStandardItem (QString::number(D_w));

            model->appendRow(QList<QStandardItem*>() << point << A << B << C << D);
            z.insert_to_db(P_w,A_w,B_w,C_w,D_w);
            if (i%100 == 0)
            {
                qDebug() << i;
            }

        }
        //qDebug() << rez;
        //z.PrintAll();
        idsTableView_->setModel(model);
    }

}

