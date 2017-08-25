#include "TimeSeriesInArray.h"

TimeSeriesInArray::TimeSeriesInArray()
{

}
QSqlDatabase TimeSeriesInArray::m_db_;

//                  NEW TYPE OF DB

TimeSeriesInArray::TimeSeriesInArray(const QString path)
{
    m_db_ = QSqlDatabase();
    QSqlQuery query(m_db_);
    QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);

    m_db_ = QSqlDatabase::addDatabase("QSQLITE");
    m_db_.setDatabaseName(path);
    m_db_.open();

    query = QSqlQuery(m_db_);

    if (!m_db_.isOpen())
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

void TimeSeriesInArray::insertIntoTable(const QHash <QString,QString> &ts)
{
    m_db_.transaction();

    QSqlQuery query(m_db_);
    query.prepare("INSERT OR REPLACE INTO Function (Key, Value) VALUES (:Key, :Value)");

    foreach(const TimeSeriesID &id, ts.keys())
    {
        const QString value = ts.value(id);
        query.bindValue(":Key", id);
        query.bindValue(":Value", value);
        query.exec();
    }
    m_db_.commit();
}

void TimeSeriesInArray::insertIntoTableFromOriginalTypes(const TimeSeriesList &ts)
{
    QHash <QString,QString> result;
    foreach (const TimeSeries object, ts)
    {
        QString actualStr;
        for (int i = 0; i < object.length(); i ++)
        {
            actualStr.append(QString::number(object.value(i)));
            actualStr.append(" ");
        }
        actualStr.chop(1);
        result.insert(object.id(),actualStr);
    }
    this->insertIntoTable(result);
}

QList<QString> TimeSeriesInArray::fetchAllIDs(const QList<QString> names)
{
    return names;
}

bool TimeSeriesInArray::clear(const QString &databaseName)
{
    m_db_.close();
    QSqlQuery query(m_db_);
    m_db_ = QSqlDatabase();
    QSqlDatabase::removeDatabase(databaseName);
    return true;
}

void TimeSeriesInArray::loadDataFromFile(const QString &path)
{

}

void TimeSeriesInArray::write(const TimeSeriesList &ts)
{
    insertIntoTableFromOriginalTypes(ts);
}

TimeSeriesList TimeSeriesInArray::read(const QList<TimeSeriesID> &ids)
{
    return timeSeriesFromString(ids);
}

bool TimeSeriesInArray::remove(const QString &databaseName)
{
    return clear(databaseName);
}

TimeSeriesInArray *TimeSeriesInArray::open(const QString &databaseName)
{
    return new TimeSeriesInArray(databaseName);
}



QList<TimeSeries> TimeSeriesInArray::timeSeriesFromString(const QList<QString> &ids)
{
    QHash<QString, QString> strWithDat = this->getStringFromDatBase(ids);
    QList<TimeSeries> mainResult;
    foreach(const QString &strWithDatValue, strWithDat.keys())
    {
        TimeSeries results(strWithDatValue);
        QStringList list = strWithDat.value(strWithDatValue).split(' ');
        foreach (QString element, list)
        {
            results.append((element).toDouble());
        }
        mainResult.append(results);
    }
    return mainResult;

}

QHash <QString,QString> TimeSeriesInArray::getStringFromDatBase(const  QList<QString> &ids)
{
    QHash <QString,QString> result;
    if (!m_db_.open())
    {
        qWarning() << "Error: connection with database fail from getStringFromDatBase";
    }
    else
    {
        QElapsedTimer timer;
        timer.restart();
        QSet<QString> idSet = ids.toSet();
        QSqlQuery query(m_db_);
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

void TimeSeriesInArray::loadDataFromJson(const QString path)
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
