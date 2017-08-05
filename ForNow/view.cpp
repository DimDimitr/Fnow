#include "view.h"
#include "Database.h"
#include "TimeSeriesDatabase.h"


View::View(QWidget *parent) : QDialog(parent)
{
    initState(5);
    initModels();
    initView();
    initLogic();
    update();
}

void View::loadFile()
{
    QFile file;
    ///review20170727 ставить пробел с обоих сторон = или других бинарных операторов, кроме .  и ->
    globPath = QFileDialog::getOpenFileName(NULL,"","C:/fnow-studying/.","*.json");
    file.setFileName(globPath);
    if (file.open(QIODevice::ReadOnly|QFile::Text))
    {
        doc = QJsonDocument::fromJson(QByteArray(file.readAll()),&docError);
        ///review20170727 использовать qWarning() вместо qDebug(), а ещё лучше собственную функцию вывода
    }
    file.close();

    if(docError.errorString().toInt() == QJsonParseError::NoError)
    {
        QStandardItemModel *model = new QStandardItemModel(NULL);
        model -> setHorizontalHeaderLabels(QStringList() << "Point" << "A" << "B" << "C" << "D");

        DbManager z("D:/QTProjects/ForNow/4.db");

        docArr = QJsonValue(doc.object().value("Points")).toArray();
        for (int i = 0; i < docArr.count();i++)
        {

            int P_w = docArr.at(i).toObject().value("Point").toInt();
            double A_w = docArr.at(i).toObject().value("A").toDouble();
            double B_w = docArr.at(i).toObject().value("B").toDouble();
            double C_w = docArr.at(i).toObject().value("C").toDouble();
            double D_w = docArr.at(i).toObject().value("D").toDouble();

            QStandardItem* point = new QStandardItem (QString::number(P_w));
            QStandardItem* A = new QStandardItem (QString::number(A_w));
            QStandardItem* B = new QStandardItem (QString::number(B_w));
            QStandardItem* C = new QStandardItem (QString::number(C_w));
            QStandardItem* D = new QStandardItem (QString::number(D_w));

            model->appendRow(QList<QStandardItem*>() << point << A << B << C << D);
            z.insert_to_db(P_w,A_w,B_w,C_w,D_w);
            if (i%100 == 0)
            {
                qDebug() << i;
            }

        }
        idsTableView_->setModel(model);
    }

}


void  View::initState(int e)
    {
    qWarning()<<"-------------A'm alive!!--------------"<<e;
        TimeSeriesDBI timeSeriesDBI("test.db");
        state_.ids = timeSeriesDBI.fetchAllIDs();

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

void View::initLogic()
{
    connect(openButton_, SIGNAL(clicked()), this, SLOT(loadFile()));
    connect(analizeButton_, SIGNAL(clicked()), this, SLOT(analyze()));
    connect(this, SIGNAL(analyzeDone()), this, SLOT(update()));
}

void View::analyze()
{
    const QModelIndex current = idsTableView_->currentIndex();
    qWarning() << "current:" << current;
    const QString id = idsTableModel_->data(current).toString();
    qWarning() << "id:" << id;
    state_.result = analyzer_->analyzeForID(id);
    emit analyzeDone();
}

void View::update()
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
