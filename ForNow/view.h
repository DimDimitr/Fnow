#ifndef VIEW_H
#define VIEW_H

#include <QDialog>
#include<QPushButton>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QTableView>
#include<QMessageBox>
#include<Qsql>

#include <QDebug>

#include <QByteArray>
#include <QString>

#include<QLabel>
#include <QLineEdit>


#include <QJsonObject>
#include <QJsonParseError>
#include <QFile>
#include <QFileDialog>
#include <QStandardItem>
#include <QJsonArray>

#include "Analyzer.h"
#include "TimeSeriesDatabase.h"
#include "Database.h"

class State
{
public:
    QList<QString> ids;
    AnalysisResult result;
};

class View : public QDialog
{
    Q_OBJECT
signals:
    void analyzeDone();

public:
    View(QWidget *parent = 0);

protected:

    void initState();
    void initModels();
    void initView();
    void initLogic();
private:
    QPushButton *openButton_;
    QPushButton *saveButton_;
    QPushButton *analizeButton_;

    QTableView *idsTableView_;

    QTableView *resultTableView_;





    QStandardItemModel *idsTableModel_;
    QStandardItemModel *resultTableModel_;

    QJsonDocument doc;
    QJsonArray docArr;
    QJsonParseError docError;
    QString globPath;
    DbManager activeDatBase;


    QList<QString> namesOfSelected;

    //Все для Json
    State state_;
    ComplexAnalyzer *analyzer_;

protected slots:
    void loadFile();
    void analyze();
    void update();
    void saveButtonPressed();
};



class Make_string : public QDialog
{
    Q_OBJECT
private:
    int i;
public slots:
    void FilePathw(){
    }
    void FilePathr()
    {
        QMessageBox msg;
        QString z="Файл успешно сохранен!";
        msg.setText(z);
        msg.exec();
    }




};
#endif // VIEW_H
