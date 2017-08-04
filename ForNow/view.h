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
    void initState()
    {
        TimeSeriesDBI timeSeriesDBI("test.db");
        state_.ids = timeSeriesDBI.fetchAllIDs();

        analyzer_ = new ComplexAnalyzer(QList<Analyzer*>()
                                        << new AvgAnalyzer()
                                        << new DevAnalyzer()
                                        << new VarCoefAnalyzer());
    }

    void initModels()
    {
        idsTableModel_ = new QStandardItemModel();
        resultTableModel_ = new QStandardItemModel();
    }

    void initView()
    {
        analizeButton_ = new QPushButton("Анализ");
       // analizeButton_->setEnabled(false);

        openButton_ = new QPushButton("Открыть");
       // openButton_->setDefault(true);

        saveButton_ = new QPushButton("Сохранить");
       // saveButton_->setEnabled(false);

        idsTableView_ = new QTableView();
        resultTableView_ = new QTableView();

        QHBoxLayout *loadSaveDataButtonsLayout = new QHBoxLayout();
        loadSaveDataButtonsLayout->addWidget(openButton_);
        loadSaveDataButtonsLayout->addWidget(saveButton_);

        QVBoxLayout *idsLayout = new QVBoxLayout;
        idsLayout->addLayout(loadSaveDataButtonsLayout);
        idsLayout->addWidget(idsTableView_);

        QVBoxLayout *resultLayout = new QVBoxLayout;
        resultLayout->addWidget(resultTableView_);
        resultLayout->addWidget(analizeButton_);

        QHBoxLayout *mainLyaout = new QHBoxLayout;
        mainLyaout->addLayout(idsLayout);
        mainLyaout->addLayout(resultLayout);

        setLayout(mainLyaout);
        setWindowTitle("F-Now!");

        idsTableView_->setModel(idsTableModel_);
        resultTableView_->setModel(resultTableModel_);
    }

    void initLogic()
    {
        connect(openButton_, SIGNAL(clicked()), this, SLOT(loadFile()));
        connect(analizeButton_, SIGNAL(clicked()), this, SLOT(analyze()));
        connect(this, SIGNAL(analyzeDone()), this, SLOT(update()));
    }
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
    //Все для Json

    State state_;
    ComplexAnalyzer *analyzer_;

protected slots:
    void loadFile();
    void analyze()
    {
        const QModelIndex current = idsTableView_->currentIndex();
        qWarning() << "current:" << current;
        const QString id = idsTableModel_->data(current).toString();
        qWarning() << "id:" << id;
        state_.result = analyzer_->analyzeForID(id);
        emit analyzeDone();
    }

    void update()
    {
        qWarning() << "!!!udpate!!!";

        idsTableModel_->clear();
        foreach(const QString &id, state_.ids)
        {
            QStandardItem* idItem = new QStandardItem(id);
            idsTableModel_->appendRow(QList<QStandardItem*>() << idItem);
        }

        resultTableModel_->clear();

        QList<QStandardItem*> row;
        resultTableModel_->setHorizontalHeaderLabels(state_.result.tags());
        foreach(const QString &tag, state_.result.tags())
        {
            qWarning() << "tag:" << tag << "state_.result.value(tag):" << state_.result.value(tag);
            row << new QStandardItem(QString::number(state_.result.value(tag)));
        }
        resultTableModel_->appendRow(row);
    }

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
