#include "view.h"
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
    datBaseSql_ = new TimeSeriesDocumentDBI ("Analise.db");
    resultTableModel_->clear();
    idsTableModel_->clear();
    QFile file;
    globPath_ = QFileDialog::getOpenFileName(NULL,"","C:/.","*.json");
    file.setFileName(globPath_);
    if (file.open(QIODevice::ReadOnly|QFile::Text))
    {
        doc_ = QJsonDocument::fromJson(QByteArray(file.readAll()),&docError_);
        datBaseSql_->loadDataFromJson(globPath_);
    }
    else
    {
        qWarning() << "Can't load json";
    }
    state_.ids.clear();
    QList<QString> namesOfFunction;
    qWarning() << datBaseSql_->fetchAllIDs(namesOfFunction);
    state_.ids = datBaseSql_->fetchAllIDs(namesOfFunction);
    qWarning() << state_.ids;
    emit analyzeDone();
    analizeButton_->setEnabled(true);
}

void  View::saveFile()
{

    QFile file;
    QString saveFileName = QFileDialog::getSaveFileName(this,"Save As","./output.json",tr("files(*.json )"));
    QMessageBox::information(this, tr(" "), "File Saved");
    file.setFileName(saveFileName);
    state_.result.saveJson(saveFileName);
    update();
    //datBaseSql_->clear(datBaseSql_->getPath());
}

void  View::deleteDatBase()
{
    datBaseSql_->clear(datBaseSql_->getPath());
}

void  View::initState()
{

    //namesOfFunction << "A" << "B" << "C" << "D";
    analyzer_ = new ComplexAnalyzer(QList<Analyzer*>()
                                    << new AvgAnalyzer()
                                    << new DevAnalyzer()
                                    << new VarCoefAnalyzer());

}

void View::initModels()
{
    idsTableModel_ = new QStandardItemModel();
    resultTableModel_ = new QStandardItemModel();
    datBaseSql_->insertIntoTableFromOriginalType((TimeSeriesList()
                                                 << (TimeSeries("A") << 1.0 << 0.999 << 5.0)
                                                 << (TimeSeries("B") << 1085.0 << 2.0 << 14.85)));
}

void View::initView()
{
    //initialising visual object
    analizeButton_ = new QPushButton("Анализ");
    analizeButton_->setEnabled(false);

    deleteDB_ = new QPushButton("Удалить базу");

    openButton_ = new QPushButton("Открыть");
    openButton_->setDefault(true);

    saveButton_ = new QPushButton("Сохранить");
    saveButton_->setEnabled(false);

    idsTableView_ = new QTableView();
    resultTableView_ = new QTableView();

    //set icons
    QPixmap pixOpen("open.png");
    openButton_->setIcon(pixOpen);
    openButton_->setIconSize(QSize(46, 46));

    QPixmap pixSave("save.png");
    saveButton_->setIcon(pixSave);
    saveButton_->setIconSize(QSize(46, 46));

    QPixmap pixAnalise("analise.png");
    analizeButton_->setIcon(pixAnalise);
    analizeButton_->setIconSize(QSize(46, 46));

    //objects orientation
    QHBoxLayout *loadSaveDataButtonsLayout = new QHBoxLayout();
    loadSaveDataButtonsLayout->addWidget(openButton_);
    loadSaveDataButtonsLayout->addWidget(saveButton_);

    QVBoxLayout *idsLayout = new QVBoxLayout;
    idsLayout->addLayout(loadSaveDataButtonsLayout);
    idsLayout->addWidget(idsTableView_);

    QHBoxLayout *analiseAndDelete = new QHBoxLayout;
    analiseAndDelete->addWidget(analizeButton_);
    analiseAndDelete->addWidget(deleteDB_);

    QVBoxLayout *resultLayout = new QVBoxLayout;
    resultLayout->addWidget(resultTableView_);
    resultLayout->addLayout(analiseAndDelete);

    QHBoxLayout *mainLyaout = new QHBoxLayout;
    mainLyaout->addLayout(idsLayout);
    mainLyaout->addLayout(resultLayout);

    setLayout(mainLyaout);
    setWindowTitle("F-Now!");

    //set models to objects
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
    connect(deleteDB_, SIGNAL(clicked()), this, SLOT(deleteDatBase()));
    connect(this, SIGNAL(analyzeDone()), this, SLOT(update()));
    connect(resultTableModel_, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(updateItem(QStandardItem*)));
}

void View::analyze()
{
    const QModelIndexList selection = idsTableView_->selectionModel()->selectedRows();
    namesOfSelected_.clear();
    for(int i = 0; i < selection.count(); i ++)
    {
        QModelIndex index = selection.at(i);
        namesOfSelected_.append(idsTableModel_->data(index).toString());
    }
    rowsInFutureTable_.clear();
    state_.result = analyzer_->analyzeForIDs(datBaseSql_,namesOfSelected_);
    QList<QStandardItem*> row;
    foreach(const QString &id, state_.result.tags())
    {
        row.clear();
        row << new QStandardItem(id);
        foreach(const QString &tag, state_.result.tagsInside(id))
        {
            row << new QStandardItem(QString("%1").arg(state_.result.getTable()[id].value(tag)));
        }
        rowsInFutureTable_.append(row);
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
    QList<QString> z;
    z.append("Names");
    foreach(const QString element,state_.result.tagsInside(state_.result.tags().value(0)))
        z.append(element);
    resultTableModel_->setHorizontalHeaderLabels(z);


    foreach(const QList<QStandardItem*> &row, rowsInFutureTable_)
    {
        resultTableModel_->appendRow(row);
    }


    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(resultTableModel_);
    for(int i = 0; i < resultTableView_->model()->rowCount(); i++)
    {
        for (int j = 0; j < resultTableView_->model()->columnCount(); j++)
        {
            if(resultTableModel_->index(i,j).data() < 0 && j != 0)
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
            if(resultTableModel_->index(numOfRow,i).data() < 0 && i != 0)
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

