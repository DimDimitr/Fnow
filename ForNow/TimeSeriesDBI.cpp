#include "TimeSeriesDBI.h"


QSqlDatabase TimeSeriesDBI::m_db;

TimeSeriesDBI::TimeSeriesDBI(const QString path)
{
    m_db = QSqlDatabase();
    QSqlQuery query(m_db);
    QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);
    m_db.open();
    
    query = QSqlQuery(m_db);
    
    if (!m_db.open())
    {
        qWarning() << "Error: connection with database fail from create";
    }
    else
    {
        
        QSqlQuery query("CREATE TABLE point ("
                        "point integer PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL,"
                        "A double NOT NULL,"
                        "B double NOT NULL,"
                        "C double NOT NULL,"
                        "D double NOT NULL"
                        ")");
        query.exec();
        qWarning() << "Database: connection ok"<<query.lastError();
    }
    
}

void TimeSeriesDBI::write(const QList<TimeSeries> &ts)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO point (point, A, B,C,D) "
                  "VALUES (:point, :A, :B, :C, :D)");
    
    
    for (int i=0; i<ts.at(0).size();i++)
    {
        query.bindValue(":point", i);
        query.bindValue(":A", ts.value(0).value(i));
        query.bindValue(":B", ts.value(1).value(i));
        query.bindValue(":C", ts.value(2).value(i));
        query.bindValue(":D", ts.value(3).value(i));
        query.exec();
        query.next();
    }
    
}

TimeSeries TimeSeriesDBI::read(const QString &id)
{
    TimeSeries list(id);
    if (!m_db.open())
    {
        qWarning() << "Error: connection with database fail from read";
    }
    else
    {
        QSqlQuery query(m_db);
        query.setForwardOnly(true);

        qWarning() << query.lastError().text()<<"I am 1 bug";
        if (id == "A")
        {
            query.prepare("SELECT A FROM point");
        }
        else  if (id == "B")
        {
            query.prepare("SELECT B FROM point");
        }
        else  if (id == "C")
        {
            query.prepare("SELECT C FROM point");
        }
        else
        {
            query.prepare("SELECT D FROM point");
        }
        if(!query.exec())
        {
            qWarning() << query.lastError().text()<<"I am 2 bug";
        }

        while (query.next())
        {
            list << query.value(0).toDouble();
        }
    }
    return list;
}



//                  NEW TYPE OF DB



TimeSeriesDBI::TimeSeriesDBI(const QString path, const int key)
{
    m_db = QSqlDatabase();
    QSqlQuery query(m_db);
    QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);
    m_db.open();

    qInfo() << m_db.lastError();

    query = QSqlQuery(m_db);

    if (!m_db.isOpen())
    {
        // qWarning() << "Error: connection with database fail from TimeSeriesDBI";
    }
    else
    {

        QSqlQuery query("CREATE TABLE Function ("
                        "Key TEXT UNIQUE NOT NULL,"
                        "Value TEXT NOT NULL"
                        ")");
        query.exec();
        //qWarning() << "Database: connection ok"<<query.lastError();
    }

}


void TimeSeriesDBI::insertIntoTable(const QHash <QString,QString> &ts)
{
    //qWarning() << "writefromjson, i get " << ts;
    m_db.transaction();

    QSqlQuery query(m_db);
    query.prepare("INSERT INTO Function (Key, Value) VALUES (:Key, :Value)");

    foreach(const TimeSeriesID &id, ts.keys())
    {
        const QString value = ts.value(id);
        query.bindValue(":Key", id);
        query.bindValue(":Value", value);
        query.exec();
    }
    m_db.commit();
    //qWarning() << "exit";
}




void TimeSeriesDBI::insertIntoTableFromOriginalType(const TimeSeriesList &ts)
{
    //qWarning() << "I am insertIntoTableFromOriginalType, i get "<<ts;
    QHash <QString,QString> result;
    foreach (TimeSeries object, ts)
    {
        QJsonArray array;
        for (int i=0; i < object.length(); i++)
        {
            QJsonObject z;
            //qWarning () << "Enter at " << i;
            z.insert("num", QJsonValue::fromVariant(i+1));
            z.insert("value", QJsonValue::fromVariant(object.value(i)));
            array.append(z);
        }
        QJsonDocument doc;
        doc.setArray(array);
        result.insert(object.id(),doc.toJson(QJsonDocument::Compact));
        //qWarning() << "I got array = " << result;
    }
    //qWarning() << "And give you "<<result;
    this->insertIntoTable(result);
}




TimeSeries TimeSeriesDBI::timeSeriesFromString(const QString &id)
{
    //qWarning() << "Hey, i am strFromTimeSeries!";

    QString strJson = this->getStringFromDatBase(id);
    QJsonDocument docjson;
    docjson = QJsonDocument::fromJson(strJson.toUtf8());
    QJsonArray jsonArray = docjson.array();
    //qWarning() << jsonArray;

    TimeSeries results(id);
    foreach(QJsonValue ob, jsonArray)
    {
        QJsonObject object;
        object = ob.toObject();
        foreach (QString tag, object.keys())
        {

            //qWarning() << tag << object[tag];
            if( tag == "value")
            {
                results.append(object[tag].toDouble());
            }
        }
    }
    //qWarning() << results;
    return results;

}


QString TimeSeriesDBI::getStringFromDatBase(const QString &id)
{
    QString result;
    if (!m_db.open())
    {
        //qWarning() << "Error: connection with database fail from getStringFromDatBase";
    }
    else
    {
        QSqlQuery query(m_db);
        query.setForwardOnly(true);
        query.exec(QString("SELECT Value FROM Function WHERE Key = '"+id+"'"));\
        while (query.next())
        {
            result.append(query.value(0).toString());
        }
        //qWarning() << "I get this row" << result;
    }
    return result;
}

void TimeSeriesDBI::loadDataFromJson(const QString path)
{
    QJsonDocument docjson;
    QFile jsonFile(path);
    if(jsonFile.open(QIODevice::Text|QIODevice::ReadOnly))
    {
        QTextStream stream(&jsonFile);
        docjson = QJsonDocument::fromJson(stream.readAll().toUtf8());
        QHash <QString,QString> forwrite;
        QJsonObject jsonObject = docjson.object();
        foreach (const QString &key, jsonObject.keys())
        {
            QJsonArray jsonArray = jsonObject[key].toArray();
            QJsonDocument doc;
            doc.setArray(jsonArray);
            forwrite.insert(key,doc.toJson(QJsonDocument::Compact));
        }
        //qWarning () << "I find/open json file :)";
        this->insertIntoTable(forwrite);
    }
    else
    {
        //qWarning () << "I can't find/open json file :(";
    }

}






void TTimeSeriesDBI::TestWriteReadRewrite_data()
{
    QTest::addColumn<TimeSeriesList>("initSeries");
    QTest::addColumn<QString>("id");
    QTest::addColumn<TimeSeries>("expected");
    
    QTest::newRow("Test 1") <<(TimeSeriesList()
                               << (TimeSeries("A")<< 1.0 << 2.0 << 5.0
                                   << 1.0 << 2.0 << 5.0)
                               << (TimeSeries("B")<< 1.0 << 2.0 << 5.0
                                   << 10.0 << 2.0 << 5.0))
                           <<"A"
                          <<(TimeSeries("A")<< 1.0 << 2.0 << 5.0
                             << 1.0 << 2.0 << 5.0);
    
    
    QTest::newRow("Test 2") <<(TimeSeriesList()
                               << (TimeSeries("A")<< 1.0 << 2.0 << 5.0)
                               << (TimeSeries("B")<< 1.0 << 2.0 << 5.0))
                           <<"A"
                          <<(TimeSeries("A")<< 1.0 << 2.0 << 5.0);
    
    
    QTest::newRow("Test 3") <<(TimeSeriesList()
                               << (TimeSeries("A")<< 1.0 << 2.0 << 5.0
                                   << 1.0 << 2.0 << 5.0
                                   << 3.0 << 2.0 << 5.0
                                   << 2.0 << 2.0 << 5.0
                                   << 1.0 << 2.0 << 5.0)
                               << (TimeSeries("B")<< 1.0 << 2.0 << 5.0
                                   << 10.0 << 2.0 << 5.0
                                   << 1.0 << 0.0 << 7.0
                                   << 8.0 << 2.0 << 5.0
                                   << 1.0 << 1.0 << 50.0))
                           <<"A"
                          <<(TimeSeries("A")<< 1.0 << 2.0 << 5.0
                             << 1.0 << 2.0 << 5.0
                             << 3.0 << 2.0 << 5.0
                             << 2.0 << 2.0 << 5.0
                             << 1.0 << 2.0 << 5.0);
    QTest::newRow("Test ") <<(TimeSeriesList()
                              << (TimeSeries("A")<< 1.0 << 2.0 << 5.0
                                  << 1.0 << 2.0 << 5.0
                                  << 3.0 << 2.0 << 5.0
                                  << 2.0 << 2.0 << 5.0
                                  << 1.0 << 2.0 << 5.0)
                              << (TimeSeries("B")<< 1.0 << 2.0 << 5.0
                                  << 10.0 << 2.0 << 5.0
                                  << 1.0 << 0.0 << 7.0
                                  << 8.0 << 2.0 << 5.0
                                  << 1.0 << 1.0 << 50.0)
                              << (TimeSeries("C")<< 1.0 << 2.0 << 5.0
                                  << 1.001 << 7.0 << 5.0
                                  << 3.01 << 27.0 << 5.0
                                  << 2.20 << 27.0 << 5.0
                                  << 13.0 << 28.0 << 5.0)
                              << (TimeSeries("D")<< 1.08 << 2.0 << 5.0
                                  << 1.70 << 2.0 << 5.90
                                  << 93.0 << 2.0 << 56.0
                                  << 28.0 << 2.0 << 5.0
                                  << 1.04 << 2.0 << 5.0))
                          <<"D"
                         <<(TimeSeries("D")<< 1.08 << 2.0 << 5.0
                            << 1.70 << 2.0 << 5.90
                            << 93.0 << 2.0 << 56.0
                            << 28.0 << 2.0 << 5.0
                            << 1.04 << 2.0 << 5.0);
    
}

void TTimeSeriesDBI::TestWriteReadRewrite()
{
    QFETCH(TimeSeriesList, initSeries);
    QFETCH(QString, id);
    QFETCH(TimeSeries,expected);
    
    const QString path=QString(QTest::currentDataTag()) + "test_dat_base.db";
    TimeSeriesDBI testDatB(path);
    testDatB.write(initSeries);
    TimeSeries actual = testDatB.read(id);
    
    
    testDatB.clear(path);
    qWarning() << actual << expected;
    QCOMPARE(actual, expected);
}
