#include "view.h"
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
    qWarning()<<"Hey, i am loadFile";
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

            if (i%100 == 0)
            {
                qWarning() << i;
            }
        }

        datBaseVirtual->write(funktionA);
        datBaseVirtual->write(funktionB);
        datBaseVirtual->write(funktionC);
        datBaseVirtual->write(funktionD);
        //activeDatBase.rownumbers("A");
        qWarning()<< datBaseVirtual->read("A").length();
    }
    analizeButton_->setEnabled(true);
}


void  View::saveFile()
{
    qWarning()<<"Hey, i am saveFile";

    QFile file;
    QString saveFileName = QFileDialog::getSaveFileName(this,"Save As","./untitled.json",tr("files(*.json )"));
    //saveFile(saveFileName);
    QMessageBox::StandardButton reply;
    reply = QMessageBox::information(this, tr(" "), "File Saved");
    file.setFileName(saveFileName);
    qWarning()<<saveFileName;
    qWarning()<<"qwertyuio";


    /*
    {
    "имя_ряда1" :
    {
    "имя_анализа1" : 123.0,
    "имя_анализа2" : 123.0,
    "имя_анализа3" : 123.0
    },
    "имя_ряда2" :
    {
    "имя_анализа1" : 123.0,
    "имя_анализа3" : 123.0,
    "имя_анализа2" : 123.0
    }
    }
    */



    QVariantList rJson;



        for(int i=0;i<resultTableModel_->rowCount();i++)
        {
            QJsonObject json;
        QList<QStandardItem *> element=resultTableModel_->takeRow(i);
        qWarning()<<"element"<<element;
        qWarning()<<i<<element.value(0)->data().toString();
        QVariant name(element.value(0)->data().toString()), averege(element.value(1)->data().toDouble()),deviation(element.value(2)->data().toDouble()),variation(element.value(3)->data().toDouble());
        json["Name"] = name.toString();
        json.insert("Averege", averege.toDouble());
        json.insert("Deviation", deviation.toDouble());
        json.insert("Variation", variation.toDouble());
        rJson.append(json);
    }



        /*QVariant id(1), name("John Doe");
        QJsonObject json;

        json["Name"] = name.toString();
        json.insert("id", id.toInt());




    /*foreach(QList<QStandardItem *> element,resultTableModel_)
        {
            foreach(const QString &tag, state_.result.tags())
            {
            QVariantMap nowWrite;
            //nowWrite.insert(tag,QString(element[1]));
            }
        }*/

    qWarning()<<rJson;


}



void  View::initState()
{
    qWarning()<<"Hey, i am initState";
    qWarning()<<"-------------A'm alive!!--------------";
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
    qWarning()<<"Hey, i am initModels";
    idsTableModel_ = new QStandardItemModel();
    resultTableModel_ = new QStandardItemModel();
}


void View::initView()
{
    qWarning()<<"Hey, i am initView";
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
    resultTableView_->setSortingEnabled(true);

    idsTableView_->setEditTriggers(QAbstractItemView::NoEditTriggers);

}

void View::initLogic()
{
    qWarning()<<"Hey, i am initLogic";

    connect(openButton_, SIGNAL(clicked()), this, SLOT(loadFile()));
    connect(analizeButton_, SIGNAL(clicked()), this, SLOT(analyze()));
    connect(saveButton_, SIGNAL(clicked()), this, SLOT(saveFile()));
    connect(this, SIGNAL(analyzeDone()), this, SLOT(update()));
    connect(resultTableModel_, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(updateItem(QStandardItem*)));
}



void View::analyze()
{
    qWarning()<<"Hey, i am analyze";
    const QModelIndexList selection = idsTableView_->selectionModel()->selectedRows();
    namesOfSelected.clear();
    for(int i=0; i< selection.count(); i++)
    {
        QModelIndex index = selection.at(i);
        namesOfSelected.append(idsTableModel_->data(index).toString());
    }
    rowsInFutureTable.clear();
    foreach(const QString &selected, namesOfSelected)
    {
        QList<double> list = datBaseVirtual->read(selected);
        state_.result = analyzer_->analyzeForID(selected,list);
        QList<QStandardItem*> row;
        row.clear();
        row << new QStandardItem(selected);
        foreach(const QString &tag, state_.result.tags())
        {
            row << new QStandardItem(QString::number(state_.result.value(tag)));
            qWarning() << "tag:" << tag << "state_.result.value(tag):" << state_.result.value(tag);
        }
        rowsInFutureTable.append(row);
    }
    emit analyzeDone();
    saveButton_->setEnabled(true);
}


void View::update()
{
    qWarning()<<"Hey, i am update";
    idsTableModel_->clear();
    foreach(const QString &id, state_.ids)
    {
        QStandardItem* idItem = new QStandardItem(id);
        idsTableModel_->appendRow(QList<QStandardItem*>() << idItem);
    }
    resultTableModel_->clear();
    QList<QString> z;
    z.append("Names");
    foreach( QString element,state_.result.tags())
        z.append(element);
    resultTableModel_->setHorizontalHeaderLabels(z);


    foreach(const QList<QStandardItem*> &row, rowsInFutureTable)
    {
        resultTableModel_->appendRow(row);
    }
    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(resultTableModel_);
    for(int i=0;i<resultTableView_->model()->rowCount();i++)
    {
        for (int j=0;j<resultTableView_->model()->columnCount();j++)
        {
            if(resultTableModel_->index(i,j).data()>2 &&j!=0)
            {
                elementWithRedSquare_=resultTableModel_->index(i,j).data().toDouble();
                resultTableModel_->setData(resultTableModel_->index(i,j), QVariant(QBrush(Qt::red)), Qt::BackgroundRole);
            }
        }
    }
}




void View::updateItem(QStandardItem *item)
{
    int numOfRow=item->index().row();
    qWarning() << "Is Digit?"<<resultTableModel_->index(numOfRow,item->index().column()).data().toDouble();
    qWarning() << "item:" << elementWithRedSquare_<<"item:" <<resultTableModel_->index(numOfRow,item->index().column()).data().toDouble();
    if (elementWithRedSquare_!=resultTableModel_-> index(numOfRow,item->index().column()).data().toDouble())
    {
        qWarning() << "Here!";
        for(int i = 0; i < resultTableView_->model()->columnCount(); i++)
        {
            if(resultTableModel_->index(numOfRow,i).data() > 2 && i!=0)
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

