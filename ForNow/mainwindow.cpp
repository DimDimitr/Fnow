#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QFile file;
    globPath=QFileDialog::getOpenFileName(nullptr,"","D:/QTProjects/FNOW/","*.json");
    file.setFileName(globPath);
    if (file.open(QIODevice::ReadOnly|QFile::Text))
    {
        doc=QJsonDocument::fromJson(QByteArray(file.readAll()),&docError);
    }
    file.close();

    if(docError.errorString().toInt()==QJsonParseError::NoError)
    {
    QStandardItemModel *model=new QStandardItemModel(nullptr);
    model->setHorizontalHeaderLabels(QStringList()<<"Point"<<"A"<<"B"<<"C"<<"D");

    docArr=QJsonValue(doc.object().value("Points")).toArray();
    for (int i=0;i<docArr.count();i++)
    {
        QStandardItem* point=new QStandardItem (QString::number( docArr.at(i).toObject().value("Point").toInt()));
        QStandardItem* A=new QStandardItem (( docArr.at(i).toObject().value("A").toString()));
        QStandardItem* B=new QStandardItem (( docArr.at(i).toObject().value("B").toString()));
        QStandardItem* C=new QStandardItem (( docArr.at(i).toObject().value("C").toString()));
        QStandardItem* D=new QStandardItem (( docArr.at(i).toObject().value("D").toString()));
        model->appendRow(QList<QStandardItem*>()<<point<<A<<B<<C<<D);
    }
    ui->tableView->setModel(model);
    }
}

void MainWindow::on_pushButton_2_clicked()
{

}

void MainWindow::on_pushButton_3_clicked()
{

}
