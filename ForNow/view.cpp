#include "view.h"
//#include "DataInMemmoryMoc.h"
#include "QIcon"
#include "TimeSeriesDBI.h"


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
    datBaseSql = new TimeSeriesDBI ("Analise.db",1);

    QFile file;
    globPath = QFileDialog::getOpenFileName(NULL,"","C:/QtStud/Fnow/ForNow/.","*.json");
    file.setFileName(globPath);
    if (file.open(QIODevice::ReadOnly|QFile::Text))
    {
        doc = QJsonDocument::fromJson(QByteArray(file.readAll()),&docError);
        datBaseSql->loadDataFromJson(globPath);
    }
    else
    {
        qWarning() << "Can't load json";
    }
    analizeButton_->setEnabled(true);
}

void  View::saveFile()
{

    QFile file;
    QString saveFileName = QFileDialog::getSaveFileName(this,"Save As","./output.json",tr("files(*.json )"));
    QMessageBox::information(this, tr(" "), "File Saved");
    file.setFileName(saveFileName);
    state_.result.saveJson(saveFileName);

}



void  View::initState()
{
    QList<QString> namesOfFunction;
    namesOfFunction << "A" << "B" << "C" << "D";
    state_.ids = datBaseSql->fetchAllIDs(namesOfFunction);

    analyzer_ = new ComplexAnalyzer(QList<Analyzer*>()
                                    << new AvgAnalyzer()
                                    << new DevAnalyzer()
                                    << new VarCoefAnalyzer());

}


void View::initModels()
{
    idsTableModel_ = new QStandardItemModel();
    resultTableModel_ = new QStandardItemModel();
    datBaseSql->insertIntoTableFromOriginalType((TimeSeriesList()
                                                 << (TimeSeries("A") << 1.0 << 0.999 << 5.0)
                                                 << (TimeSeries("B") << 1085.0 << 2.0 << 14.85)));
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



    QPixmap pixOpen("analise.png");
    openButton_->setIcon(pixOpen);
    openButton_->setIconSize(QSize(46, 46));

    QPixmap pixSave("save.png");
    saveButton_->setIcon(pixSave);
    saveButton_->setIconSize(QSize(46, 46));

    QPixmap pixAnalise("analise.png");
    analizeButton_->setIcon(pixAnalise);
    analizeButton_->setIconSize(QSize(46, 46));



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
    resultTableView_->setSortingEnabled(true);

    idsTableView_->setEditTriggers(QAbstractItemView::NoEditTriggers);

}

void View::initLogic()
{
    connect(openButton_, SIGNAL(clicked()), this, SLOT(loadFile()));
    connect(analizeButton_, SIGNAL(clicked()), this, SLOT(analyze()));
    connect(saveButton_, SIGNAL(clicked()), this, SLOT(saveFile()));
    connect(this, SIGNAL(analyzeDone()), this, SLOT(update()));
    connect(resultTableModel_, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(updateItem(QStandardItem*)));
}




void View::analyze()
{
    const QModelIndexList selection = idsTableView_->selectionModel()->selectedRows();
    namesOfSelected.clear();
    for(int i = 0; i < selection.count(); i ++)
    {
        QModelIndex index = selection.at(i);
        namesOfSelected.append(idsTableModel_->data(index).toString());
    }
    rowsInFutureTable.clear();

    state_.result = analyzer_->analyzeForIDs(datBaseSql,namesOfSelected);
    QList<QStandardItem*> row;
    foreach(const QString &id, state_.result.tags())
    {
        row.clear();
        row << new QStandardItem(id);
        foreach(const QString &tag, state_.result.tagsInside(id))
        {
            row << new QStandardItem(QString("%1").arg(state_.result.getTable()[id].value(tag)));
        }
        rowsInFutureTable.append(row);
    }
    emit analyzeDone();
    saveButton_->setEnabled(true);
}


void View::update()
{
    //qWarning() << "Hey, i am update";
    idsTableModel_->clear();
    foreach(const QString &id, state_.ids)
    {
        QStandardItem* idItem = new QStandardItem(id);
        idsTableModel_->appendRow(QList<QStandardItem*>() << idItem);
    }
    resultTableModel_->clear();
    QList<QString> z;
    z.append("Names");
    foreach(const QString element,state_.result.tagsInside(state_.result.tags().value(0)))
        z.append(element);
    resultTableModel_->setHorizontalHeaderLabels(z);


    foreach(const QList<QStandardItem*> &row, rowsInFutureTable)
    {
        resultTableModel_->appendRow(row);
    }


    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(resultTableModel_);
    for(int i = 0; i < resultTableView_->model()->rowCount(); i++)
    {
        for (int j = 0; j < resultTableView_->model()->columnCount(); j++)
        {
            if(resultTableModel_->index(i,j).data() > 2 && j != 0)
            {
                elementWithRedSquare_ = resultTableModel_->index(i,j).data().toDouble();
                resultTableModel_->setData(resultTableModel_->index(i,j), QVariant(QBrush(Qt::red)), Qt::BackgroundRole);
            }
        }
    }
}

void View::updateItem(QStandardItem *item)
{
    int numOfRow = item->index().row();
    if (elementWithRedSquare_ != resultTableModel_-> index(numOfRow,item->index().column()).data().toDouble())
    {
        for(int i = 0; i < resultTableView_->model()->columnCount(); i++)
        {
            if(resultTableModel_->index(numOfRow,i).data() > 2 && i != 0)
            {
                resultTableModel_->setData(resultTableModel_->index(numOfRow,i), QVariant(QBrush(Qt::red)), Qt::BackgroundRole);
            }
            else
            {
                resultTableModel_->setData(resultTableModel_->index(numOfRow,i), QVariant(QBrush(Qt::green)), Qt::BackgroundRole);
            }
        }
    }
}

