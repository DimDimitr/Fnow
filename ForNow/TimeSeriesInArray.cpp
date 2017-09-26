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

        QSqlQuery query("CREATE TABLE timeSeriesByPoints ("
                        "Key TEXT UNIQUE NOT NULL,"
                        "Value TEXT NOT NULL"
                        ")");
        query.exec();
    }

}

void TimeSeriesInArray::insertIntoTable(const QHash <QString,QString> &ts)
{
    QHash <QString,QString> tSLRecord = getStringFromDatBase(ts.keys());
    if (!tSLRecord.isEmpty())
    {
        injectionIn(tSLRecord, ts);
    }
    else
    {
        m_db_.transaction();

        QSqlQuery query(m_db_);
        query.prepare("INSERT OR REPLACE INTO timeSeriesByPoints (Key, Value) VALUES (:Key, :Value)");

        foreach(const TimeSeriesID &id, ts.keys())
        {
            const QString value = ts.value(id);
            query.bindValue(":Key", id);
            query.bindValue(":Value", value);
            query.exec();
        }
        m_db_.commit();
    }
}


void TimeSeriesInArray::injectionIn(const QHash<TimeSeriesID, QString> &init,
                                    const QHash<TimeSeriesID, QString> &additional)
{
    TimeSeriesList mainResult;
    QElapsedTimer timer;
    timer.start();
    QElapsedTimer jsonTimer;
    jsonTimer.restart();
    jsonTimer.restart();
    foreach (const QString &id, init.keys())
    {
        QMap<int, double> initMap = getMapFromStr(init.value(id));
        QMap<int, double> additionalMap = getMapFromStr(additional.value(id));
        const int min = additionalMap.firstKey();
        const int max = additionalMap.lastKey();

        foreach(const int num, initMap.keys())
        {
            if(num >= min && num <= max)
            {
                initMap.remove(num);
            }
        }
        initMap.unite(additionalMap);
        mainResult.append(timeSeriesFromQMap(id, initMap));
    }
    deleteFromOriginalTypes(mainResult);
    insertIntoTableFromOriginalTypes(mainResult);
}

void TimeSeriesInArray::deleteFromOriginalTypes(const TimeSeriesList &ts)
{
    m_db_.transaction();
    QSqlQuery query(m_db_);
    query.prepare("DELETE FROM timeSeriesByPoints WHERE Key = :id");
    foreach (TimeSeries id, ts)
    {
        query.bindValue(":id", id.id());
        query.exec();
    }
    m_db_.commit();
}


void TimeSeriesInArray::insertIntoTableFromOriginalTypes(const TimeSeriesList &ts)
{
    QHash <QString,QString> result;
    foreach (const TimeSeries object, ts)
    {
        if ( ! object.isEmpty())
        {
            QString actualStr;
            int memberZero = 0;
            for (int i = 0; i < object.size(); i ++)
            {
                if(object.value(i + 1) == 0)
                {
                    memberZero++;
                }
                if (memberZero < 2)
                {
                    actualStr.append(QString::number(i));
                    actualStr.append("*");
                    actualStr.append(QString::number(object.value(i + 1)));
                    actualStr.append("_");
                    memberZero = 0;
                }
            }
            actualStr.chop(1);
            result.insert(object.id(),actualStr);
        }
    }
    this->insertIntoTable(result);
}

QSqlQuery* TimeSeriesInArray::getQueryForIndependQuery(const  QList<TimeSeriesID> &ids)
{
    QSqlQuery *query;
    query = new  QSqlQuery (m_db_);
    query->prepare("CREATE TEMPORARY TABLE tempTimeSeriesByPoints (Id TEXT NOT NULL)");
    if (!query->exec())
    {
        qWarning() << "query.lastError 1 " << query->lastError();
    }
    {
        m_db_.transaction();
        QSqlQuery query1(m_db_);
        query1.prepare("INSERT OR REPLACE INTO tempTimeSeriesByPoints (Id) VALUES (:Id)");
        foreach(const TimeSeriesID &id, ids)
        {
            query1.bindValue(":Id", id);
            if (!query1.exec())
            {
                qWarning() << "query.lastError 2 " << query1.lastError();
            }
        }
        m_db_.commit();
    }
    query->prepare("SELECT Id FROM tempTimeSeriesByPoints");
    if (!query->exec())
    {
        qWarning() << "query.lastError 3 " << query->lastError();
    }
    query->setForwardOnly(true);
    query->exec("SELECT Key, Value FROM timeSeriesByPoints INNER JOIN tempTimeSeriesByPoints ON"
                " timeSeriesByPoints.Key = tempTimeSeriesByPoints.Id");
    return query;
}

QList<QString> TimeSeriesInArray::fetchAllIDs(const QList<QString> names)
{
    if (!names.isEmpty())
    {
        return names;
    }
    else
    {

        QList<QString> result;
        QSqlQuery query(m_db_);
        query.prepare("SELECT Key FROM timeSeriesByPoints");
        if (!query.exec())
        {
            qWarning() << query.lastError();
        }

        while (query.next())
        {
            result.append(query.value(0).toString());
        }
        return result;
    }
}

bool TimeSeriesInArray::clear(const QString &databaseName)
{
    m_db_.close();
    m_db_ = QSqlDatabase();
    QSqlDatabase::removeDatabase(databaseName);
    if(QFile::exists(databaseName))
    {
        return QFile::remove(databaseName);
    }
    else
    {
        return true;
    }
}

void TimeSeriesInArray::loadDataFromFile(const QString &path)
{
    loadDataFromJson(path);
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

QMap<int,double> TimeSeriesInArray::getMapFromStr(const QString &strOfValue)
{
    QMap <int, double> mapTS;
    QStringList list = strOfValue.split('_');
    foreach (QString element, list)
    {
        QStringList elementsIn = element.split('*');
        int point = elementsIn.at(0).toInt();
        double value = elementsIn.at(1).toDouble();
        mapTS.insert(point, value);
    }
    return mapTS;
}

TimeSeries TimeSeriesInArray::timeSeriesFromQMap(const QString &strJsonValue, QMap <int, double> mapTS)
{
    TimeSeries results(strJsonValue);
    int memberPoint = mapTS.firstKey();
    int memberValue = mapTS.first();
    foreach (int key, mapTS.keys())
    {
        if (key > memberPoint + 1)
        {
            for (int i = 0; i < (key - memberPoint - 1); i++)
            {
                results.append(memberValue);
            }
        }
        results.append(mapTS.value(key));
        memberPoint = key;
        memberValue = results.last();
    }
    return results;
}

QList<TimeSeries> TimeSeriesInArray::timeSeriesFromString(const QList<QString> &ids)
{
    QHash<QString, QString> strMy = getStringFromDatBase(ids);
    QList<TimeSeries> mainResult;
    foreach(const QString &strMyValue, ids)
    {
        QMap <int, double> mapTS = getMapFromStr(strMy[strMyValue]);
        TimeSeries results = timeSeriesFromQMap(strMyValue, mapTS);
        mainResult.append(results);
    }
    //std::sort(mainResult.begin(), mainResult.end());
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
        QSet<QString> idSet = ids.toSet();
        QSqlQuery query(m_db_);
        query.setForwardOnly(true);
        query.exec("SELECT Key, Value FROM timeSeriesByPoints");
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
        QHash <QString, QString> forwrite;
        QJsonObject jsonObject = docjson.object();
        foreach (const QString &key, jsonObject.keys())
        {
            QJsonArray jsonArray = jsonObject[key].toArray();
            QString res;
            int zeroInTS = 0;
            foreach(const QJsonValue ob, jsonArray)
            {
                QJsonObject object;
                object = ob.toObject();
                if (object["value"].toDouble() == 0.0)
                {
                    zeroInTS++;
                }
                else
                {
                    zeroInTS = 0;
                }
                if (zeroInTS < 2)
                {
                    res.append(QString::number(object["num"].toInt()));
                    res.append("*");
                    res.append(QString::number(object["value"].toDouble()));
                    res.append("_");
                }
            }
            res.chop(1);
            forwrite.insert(key,res);
        }
        this->insertIntoTable(forwrite);
    }
}

TimeSeries TimeSeriesInArray::fetchTimeSeriesFromQuery(QSqlQuery *query)
{
    return fetchTimeSeriesFromQueryARR(query);
}

TimeSeries TimeSeriesInArray::fetchTimeSeriesFromQueryARR(QSqlQuery *query)
{
    const QMap <int, double> mapTS = getMapFromStr(query->value(1).toString());
    return timeSeriesFromQMap(query->value(0).toString(), mapTS);
}

