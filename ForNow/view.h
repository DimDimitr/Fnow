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
#include "TimeSeriesInArray.h"


class State
{
public:
    QList<QString> ids;
    AnalysisResult result;
};

class View : public QDialog
{

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
    QPushButton *deleteDB_;

    QTableView *idsTableView_;

    QTableView *resultTableView_;


    QList <QList<QStandardItem*> > rowsInFutureTable_;

    QStandardItemModel *idsTableModel_;
    QStandardItemModel *resultTableModel_;

    QJsonDocument doc_;
    QJsonArray docArr_;
    QJsonParseError docError_;
    QString globPath_;

    TimeSeriesInArray *datBaseSql_;
    AnalysisResult analiseDat_;

    double elementWithRedSquare_;
    QList<QString> namesOfSelected_;

    //json variable
    State state_;
    //AnalysisResult aResult_;
    ComplexAnalyzer *analyzer_;

    Q_OBJECT
protected slots:

    //open dialog and get path, call write in DB
    void loadFile();

    //call analise for selected names
    void analyze();

    //update the table view
    void update();

    //open dialog and get path, call write results in json
    void saveFile();

    void deleteDatBase();

    //using for updating any changes in table
    void updateItem(QStandardItem* item);
};
#endif // VIEW_H

