#ifndef VIEW_H
#define VIEW_H

#include <QDialog>
#include<QPushButton>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QTableView>
#include<QMessageBox>
#include<Qsql>

#include <QByteArray>
#include <QString>


#include <QJsonObject>
#include <QJsonParseError>
#include <QFile>
#include <QFileDialog>
#include <QStandardItem>
#include <QJsonArray>

#include "Analyzer.h"
#include "DataInMemmoryMoc.h"
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

    //Все для Json
    State state_;
    AnalysisResult aResult;
    ComplexAnalyzer *analyzer_;

protected slots:
    void loadFile();
    void analyze();
    void update();
    void saveFile();
    void updateItem(QStandardItem* item);
};
#endif // VIEW_H

