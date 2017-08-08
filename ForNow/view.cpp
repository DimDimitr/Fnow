#include "view.h"
#include "Database.h"
#include "TimeSeriesDatabase.h"
#include "QIcon"


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
    datBaseVirtual =new TimeSeriesDBI ("Analise");

    QFile file;
    globPath = QFileDialog::getOpenFileName(NULL,"","C:/QtStud/Fnow/ForNow/.","*.json");
    file.setFileName(globPath);
    if (file.open(QIODevice::ReadOnly|QFile::Text))
    {
        doc = QJsonDocument::fromJson(QByteArray(file.readAll()),&docError);
    }
    file.close();
    if(docError.errorString().toInt() == QJsonParseError::NoError)
    {
        QStandardItemModel *model = new QStandardItemModel(NULL);
        model -> setHorizontalHeaderLabels(QStringList() << "Function Names");

        activeDatBase.setDb("C:/QtStud/Fnow/ForNow/1.db");

        docArr = QJsonValue(doc.object().value("Points")).toArray();

        TimeSeries funktionA("A");
        TimeSeries funktionB("B");
        TimeSeries funktionC("C");
        TimeSeries funktionD("D");

        for (int i = 0; i < docArr.count();i++)

        {
            int P_w = docArr.at(i).toObject().value("Point").toInt();
            funktionA<<(docArr.at(i).toObject().value("A").toDouble());
            funktionB<<( docArr.at(i).toObject().value("B").toDouble());
            funktionC<<(docArr.at(i).toObject().value("C").toDouble());
            funktionD<<(docArr.at(i).toObject().value("D").toDouble());

            //activeDatBase.insert_to_db(P_w,A_w,B_w,C_w,D_w);
            if (i%100 == 0)
            {
                qWarning() << i;
            }
        }

        datBaseVirtual->write(funktionA);
        datBaseVirtual->write(funktionB);
        datBaseVirtual->write(funktionC);
        datBaseVirtual->write(funktionD);
        activeDatBase.rownumbers("A");
        qWarning()<< datBaseVirtual->read("A").length();
    }
    analizeButton_->setEnabled(true);
}


void  View::initState()
{
    qWarning()<<"-------------A'm alive!!--------------";
    //TimeSeriesDBI timeSeriesDBI("test.db");
    QList<QString> namesOfFunction;
    namesOfFunction<< "A" << "B" << "C" << "D";
    state_.ids = datBaseVirtual->fetchAllIDs(namesOfFunction);

    analyzer_ = new ComplexAnalyzer(QList<Analyzer*>()
                                    << new AvgAnalyzer()
                                    << new DevAnalyzer()
                                    << new VarCoefAnalyzer());
}


void View::initModels()
{
    idsTableModel_ = new QStandardItemModel();
    resultTableModel_ = new QStandardItemModel();
}


void View::initView()
{
    analizeButton_ = new QPushButton("Анализ");
    analizeButton_->setEnabled(false);

    openButton_ = new QPushButton("Открыть");
    openButton_->setDefault(true);

    saveButton_ = new QPushButton("Сохранить");
    saveButton_->setEnabled(false);

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
    idsTableView_->setEditTriggers(QAbstractItemView::NoEditTriggers);

}


void View::initLogic()
{
    connect(openButton_, SIGNAL(clicked()), this, SLOT(loadFile()));
    connect(analizeButton_, SIGNAL(clicked()), this, SLOT(analyze()));
    connect(saveButton_, SIGNAL(clicked()), this, SLOT(saveButtonPressed()));
    connect(this, SIGNAL(analyzeDone()), this, SLOT(update()));
}


void View::analyze()
{
    const QModelIndex current = idsTableView_->currentIndex();
    qWarning() << "current:" << current;

    const QModelIndexList selection = idsTableView_->selectionModel()->selectedRows();

    namesOfSelected.clear();
    for(int i=0; i< selection.count(); i++)
    {
        QModelIndex index = selection.at(i);
        namesOfSelected.append(idsTableModel_->data(index).toString());
    }
    emit analyzeDone();
    saveButton_->setEnabled(true);
}


void View::update()
{
    idsTableModel_->clear();
    foreach(const QString &id, state_.ids)
    {
        QStandardItem* idItem = new QStandardItem(id);
        idsTableModel_->appendRow(QList<QStandardItem*>() << idItem);
    }

    resultTableModel_->clear();
    resultTableModel_->setHorizontalHeaderLabels(state_.result.tags());

    foreach(const QString &selected, namesOfSelected)
    {
        QList<QStandardItem*> row;
        QList<double> list = datBaseVirtual->read(selected);
        state_.result = analyzer_->analyzeForID(selected,list);
        row.clear();
        foreach(const QString &tag, state_.result.tags())
        {
            qWarning() << "tag:" << tag << "state_.result.value(tag):" << state_.result.value(tag);
            row << new QStandardItem(QString::number(state_.result.value(tag)));
        }
        resultTableModel_->appendRow(row);
    }
    resultTableModel_->setSortRole(1);
}


void View::saveButtonPressed()
{
    activeDatBase.close_db();
}

