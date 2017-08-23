#ifndef VIEW_H
#define VIEW_H

#include <QDialog>
#include<QPushButton>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QTableView>
#include<QMessageBox>

#include <QString>


#include <QJsonObject>
#include <QJsonParseError>
#include <QFile>
#include <QFileDialog>
#include <QStandardItem>

#include "Analyzer.h"
#include "TimeSeries.h"
#include "TimeSeriesDBI.h"


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
    void changeDone();

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


    QList <QList<QStandardItem*> > rowsInFutureTable;

    QStandardItemModel *idsTableModel_;
    QStandardItemModel *resultTableModel_;

    QJsonDocument doc;
    QJsonArray docArr;
    QJsonParseError docError;
    QString globPath;

    TimeSeriesDBI *datBaseSql;
    AnalysisResult analiseDat;

    double elementWithRedSquare_;
    QList<QString> namesOfSelected;

    //json variable
    State state_;
    AnalysisResult aResult;
    ComplexAnalyzer *analyzer_;

protected slots:

    //open dialog and get path, call write in DB
    void loadFile();

    //call analise for selected names
    void analyze();

    //update the table view
    void update();

    //open dialog and get path, call write results in json
    void saveFile();

    //using for updating any changes in table
    void updateItem(QStandardItem* item);
};
#endif // VIEW_H

