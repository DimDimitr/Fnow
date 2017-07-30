#include "view.h"
#include "mainwindow.h"
#include "database.h"
View::View(QWidget *parent):QDialog(parent)
{


    analize=new QPushButton("Анализ");
    analize->setEnabled(false);

    open=new QPushButton("Открыть");
    open->setDefault(true);


    save=new QPushButton("Сохранить");

    name_open= new QLabel("Имя открываемого файла");
    open_str=new QLineEdit();
    name_save=new QLabel("Имя сохраняемого файла");
    save_str=new QLineEdit();

    QTableView *table=new QTableView;
    QTableView *table_rez=new QTableView;

    QVBoxLayout *layoutopen= new QVBoxLayout;
    layoutopen->addWidget(name_open);
    layoutopen->addWidget(open_str);

    QVBoxLayout *layoutsave= new QVBoxLayout;
    layoutopen->addWidget(name_save);
    layoutopen->addWidget(save_str);

    QVBoxLayout *layout_op_save= new QVBoxLayout;
    layout_op_save->addLayout(layoutopen);
    layout_op_save->addLayout(layoutsave);

    QHBoxLayout *layouttool= new QHBoxLayout;
    layouttool->addWidget(open);
    layouttool->addWidget(save);

    QVBoxLayout *layout0= new QVBoxLayout;
    layout0->addLayout(layout_op_save);
    layout0->addLayout(layouttool);


    QVBoxLayout *layout2= new QVBoxLayout;
    layout2->addLayout(layout0);
    layout2->addWidget(table);

    QVBoxLayout *layout= new QVBoxLayout;
    layout->addWidget(table_rez);
    layout->addWidget(analize);

    QHBoxLayout *layoutmain= new QHBoxLayout;
    layoutmain->addLayout(layout2);
    layoutmain->addLayout(layout);

    connect(open,SIGNAL(clicked()),this,SLOT(OpenClicked()));


    setLayout(layoutmain);
    setWindowTitle("My first app");
}


void View:: OpenClicked()
{

    QFile file;
    ///review20170727 ставить пробел с обоих сторон = или других бинарных операторов, кроме .  и ->
    globPath = QFileDialog::getOpenFileName(NULL,"","C:/fnow-studying/.","*.json");
    file.setFileName(globPath);
    if (file.open(QIODevice::ReadOnly|QFile::Text))
    {
        doc = QJsonDocument::fromJson(QByteArray(file.readAll()),&docError);
        ///review20170727 использовать qWarning() вместо qDebug(), а ещё лучше собственную функцию вывода
        qDebug()<<"A asf";
    }
    file.close();

    if(docError.errorString().toInt() == QJsonParseError::NoError)
    {
        QStandardItemModel *model=new QStandardItemModel(NULL);
        model->setHorizontalHeaderLabels(QStringList()<<"Point"<<"A"<<"B"<<"C"<<"D");

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
            z.Insert_to_DB(P_w,A_w,B_w,C_w,D_w);
            if (i%100 == 0)
            {
                qDebug() << i;
            }

        }
        //qDebug() << rez;
        //z.PrintAll();
        table->setModel(model);
    }

}

