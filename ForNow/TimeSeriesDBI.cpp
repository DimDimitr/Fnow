#include "TimeSeriesDBI.h"


QSqlDatabase TimeSeriesDBI::m_db;

//                  NEW TYPE OF DB
TimeSeriesDBI::TimeSeriesDBI(const QString path)
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
        //qWarning() << "Error: connection with database fail from TimeSeriesDBI";
    }
    else
    {

        QSqlQuery query("CREATE TABLE Function ("
                        "Key TEXT UNIQUE NOT NULL,"
                        "Value TEXT NOT NULL"
                        ")");
        query.exec();
        //qWarning() << "Database: connection ok" << query.lastError();
    }

}

void TimeSeriesDBI::insertIntoTable(const QHash <QString,QString> &ts)
{
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
}

void TimeSeriesDBI::insertIntoTableFromOriginalType(const TimeSeriesList &ts)
{
    QHash <QString,QString> result;
    foreach (const TimeSeries object, ts)
    {
        QJsonArray array;
        for (int i = 0; i < object.length(); i ++)
        {
            QJsonObject z;
            z.insert("num", QJsonValue::fromVariant(i+1));
            z.insert("value", QJsonValue::fromVariant(object.value(i)));
            array.append(z);
        }
        QJsonDocument doc;
        doc.setArray(array);
        result.insert(object.id(),doc.toJson(QJsonDocument::Compact));
    }
    this->insertIntoTable(result);
}

QList<QString> TimeSeriesDBI::fetchAllIDs(const QList<QString> names)
{
    return names;
}

bool TimeSeriesDBI::clear(const QString &databaseName)
{
    m_db.close();
    QSqlQuery query(m_db);
    m_db = QSqlDatabase();
    QSqlDatabase::removeDatabase(databaseName);
    return true;
}

QList<TimeSeries> TimeSeriesDBI::timeSeriesFromString(const QList<QString> &ids)
{
    QHash<QString, QString> strJson = this->getStringFromDatBase(ids);
    QJsonDocument docJson;
    QList<TimeSeries> mainResult;
    foreach(const QString &strJsonValue, strJson.keys())
    {
        docJson = QJsonDocument::fromJson(strJson[strJsonValue].toUtf8());
        QJsonArray jsonArray = docJson.array();
        TimeSeries results(strJsonValue);


        foreach(const QJsonValue ob, jsonArray)
        {
            QJsonObject object;
            object = ob.toObject();
            foreach (const QString tag, object.keys())
            {
                if( tag == "value")
                {
                    results.append(object[tag].toDouble());
                }
            }
        }

        mainResult.append(results);
    }
    return mainResult;

}

QHash <QString,QString> TimeSeriesDBI::getStringFromDatBase(const  QList<QString> &ids)
{
    QHash <QString,QString> result;
    if (!m_db.open())
    {
        qWarning() << "Error: connection with database fail from getStringFromDatBase";
    }
    else
    {
        QElapsedTimer timer;
        timer.restart();
        QSet<QString> idSet = ids.toSet();
        QSqlQuery query(m_db);
        query.setForwardOnly(true);
        query.exec("SELECT Key, Value FROM Function");
        while(query.next())
        {
            const QString id = query.value(0).toString();
            if(idSet.contains(id))
            {
                result[id] = query.value(1).toString();
            }
        }
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
        this->insertIntoTable(forwrite);
    }
    else
    {
    }

}
