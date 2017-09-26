#include "TimeSeriesDBI.h"
#include <QPair>


QSqlDatabase TimeSeriesDocumentDBI::m_db_;

//                  NEW TYPE OF DB
TimeSeriesDocumentDBI::TimeSeriesDocumentDBI()
{

}


TimeSeriesDocumentDBI::TimeSeriesDocumentDBI(const QString path)
{
    m_db_ = QSqlDatabase();
    QSqlQuery query(m_db_);
    QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);

    m_db_ = QSqlDatabase::addDatabase("QSQLITE");
    m_db_.setDatabaseName(path);
    m_db_.open();
    //this->setPath(path);

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
        query.prepare("CREATE INDEX indx_Key "
                      "ON timeSeriesByPoints(Key)");
        query.exec();
        //qWarning() << "Database: connection ok" << query.lastError();
    }
}


void TimeSeriesDocumentDBI::injectionIn(const QHash<TimeSeriesID, QString> &init,
                                        const QHash<TimeSeriesID, QString> &additional)
{
    TimeSeriesList mainResult;
    foreach (const QString &id, init.keys())
    {
        QMap<int, double> initMap = getMapFromJson(init.value(id));
        QMap<int, double> additionalMap = getMapFromJson(additional.value(id));
        int min = 0;
        int max = 0;
        //qWarning() << "i get mapTimeSrsWriteJS" << mapTimeSrsWriteJS;
        if(!additionalMap.isEmpty())
        {
            min = additionalMap.firstKey();
            max = additionalMap.lastKey();
        }
        foreach(const int num, initMap.keys())
        {
            if(num >= min && num <= max)
            {
                initMap.remove(num);
            }
        }
        initMap.unite(additionalMap);
        mainResult.append(timeSeriesFromQMap(id, initMap));
        //qWarning() << "I get TS result = " << tsR << mainResult;
    }
    //qWarning() << "jsonTime:" << jsonTime << "jsonCounter:" << jsonCounter;
    //qWarning() << "insertF" << timer.restart();
    deleteFromOriginalTypes(mainResult);
    //qWarning() << "deleteFromOriginalTypes" << timer.restart();
    insertIntoTableFromOriginalType(mainResult);
    //qWarning() << "insertIntoTableFromOriginalType" << timer.elapsed();
}

void TimeSeriesDocumentDBI::deleteFromOriginalTypes(const TimeSeriesList &ts)
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


void TimeSeriesDocumentDBI::insertIntoTable(const QHash <QString,QString> &ts)
{
    //qWarning() << "insertIntoTable get " << ts;
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
        //qWarning() << 1;
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

TimeSeries TimeSeriesDocumentDBI::fetchTimeSeriesFromQuery(QSqlQuery *query)
{
    return fetchTimeSeriesFromQueryDBI(query);
}

void TimeSeriesDocumentDBI::insertIntoTableFromOriginalType(const TimeSeriesList &timeSeriesList)
{
    QHash <QString,QString> result;
    foreach (const TimeSeries &ts, timeSeriesList)
    {
        if (!ts.isEmpty())
        {
            QJsonArray array;
            for (int i = 0; i < ts.size();i ++)
            {
                QJsonObject point;
                point.insert("num", QJsonValue::fromVariant(i + 1));
                //qWarning() << "ts.value(i)" << ts << ts.value(i+1);
                point.insert("value", QJsonValue::fromVariant(ts.value(i+1)));
                //qWarning() << point;
                array.append(point);
            }
            QJsonDocument doc;
            doc.setArray(array);
            result.insert(ts.id(),doc.toJson(QJsonDocument::Compact));
            //qWarning() << "result = " << result;
        }
    }
    this->insertIntoTable(result);
}

QList<QString> TimeSeriesDocumentDBI::fetchAllIDs(QList<QString> list)
{
    if (!list.isEmpty())
    {
        return list;
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
            //qWarning() << query.value(0).toString();
        }
        return result;
    }
}

bool TimeSeriesDocumentDBI::clear(const QString &databaseName)
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

void TimeSeriesDocumentDBI::loadDataFromFile(const QString &path)
{
    loadDataFromJson(path);
}

void TimeSeriesDocumentDBI::write(const TimeSeriesList &ts)
{
    insertIntoTableFromOriginalType(ts);
}

TimeSeriesList TimeSeriesDocumentDBI::read(const QList<TimeSeriesID> &ids)
{
    return timeSeriesFromString(ids);
}

bool TimeSeriesDocumentDBI::remove(const QString &databaseName)
{
    return clear(databaseName);
}

TimeSeriesDBI *TimeSeriesDocumentDBI::open(const QString &databaseName)
{
    return new TimeSeriesDocumentDBI(databaseName);
}



QMap<int,double> TimeSeriesDocumentDBI::getMapFromJson(const QString &strJsonValue)
{
    QJsonDocument docJson = QJsonDocument::fromJson(strJsonValue.toUtf8());
    QJsonArray jsonArray = docJson.array();

    QMap<int, double> mapTS;
    foreach(const QJsonValue ob, jsonArray)
    {
        QJsonObject object;
        object = ob.toObject();
        const int num = object["num"].toInt();
        const double value = object["value"].toDouble();
        mapTS.insert(num, value);
    }
    return mapTS;
}

TimeSeries TimeSeriesDocumentDBI::timeSeriesFromQMap(const QString &strJsonValue, QMap <int, double> mapTS)
{
    TimeSeries results(strJsonValue);
    //results.setData(mapTS);
    int memberPoint = 0;
    int memberValue = 0;
    if (!mapTS.isEmpty())
    {
        memberPoint = mapTS.firstKey();
        memberValue = mapTS.first();
    }

    foreach (const int key, mapTS.keys())
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


QList<TimeSeries> TimeSeriesDocumentDBI::timeSeriesFromString(const QList<QString> &ids)
{
    QHash<QString, QString> strJson = this->getStringFromDatBase(ids);
    QList<TimeSeries> mainResult;
    foreach(const QString &strJsonValue, ids)
    {
        QMap <int, double> mapTS = getMapFromJson(strJson[strJsonValue]);
        TimeSeries results = timeSeriesFromQMap(strJsonValue, mapTS);
        mainResult.append(results);
    }
    //std::sort(mainResult.begin(), mainResult.end());
    return mainResult;
}

TimeSeries TimeSeriesDocumentDBI::fetchTimeSeriesFromQueryDBI(QSqlQuery *query)
{
    const QMap <int, double> mapTS = getMapFromJson(query->value(1).toString());
    return timeSeriesFromQMap(query->value(0).toString(), mapTS);
}

QSqlQuery* TimeSeriesDocumentDBI::getQueryForIndependQuery(const  QList<TimeSeriesID> &ids)
{
    QSqlQuery *query;
    query = new QSqlQuery (m_db_);

    query->prepare("CREATE TEMPORARY TABLE tempTimeSeriesByPoints (Id TEXT UNIQUE NOT NULL)");
    if (!query->exec())
    {
        qWarning() << "query.lastError 1 " << query->lastError();
    }
    {
        m_db_.transaction();
        QSqlQuery query(m_db_);
        query.prepare("INSERT INTO tempTimeSeriesByPoints (Id) VALUES (:Id)");
        //qWarning() << "query.lastError 2 " << query.lastError();
        foreach(const TimeSeriesID &id, ids)
        {
            query.bindValue(":Id", id);
            query.exec();
            //qWarning() << "query.lastError 3 " << query.lastError();
        }
        //qWarning() << "query.lastError 4 " << query.lastError();
        m_db_.commit();
    }

    query->prepare("SELECT Id FROM tempTimeSeriesByPoints");
    //qWarning() << "query.lastError 5 " << query->lastError();
    if (!query->exec())
    {
        qWarning() << "query.lastError 6 " << query->lastError();
    }
    while (query->next())
    {
        //qWarning() << "I get query" << query->value(0).toString();
    }
    query->setForwardOnly(true);
    query->exec("SELECT Key, Value FROM timeSeriesByPoints INNER JOIN tempTimeSeriesByPoints ON"
                " timeSeriesByPoints.Key = tempTimeSeriesByPoints.Id");
    return query;
}


QHash <QString,QString> TimeSeriesDocumentDBI::getStringFromDatBase(const  QList<TimeSeriesID> &ids)
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

void TimeSeriesDocumentDBI::loadDataFromJson(const QString path)
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
            doc.setArray(this->deleteZeros(jsonArray));
            forwrite.insert(key,doc.toJson(QJsonDocument::Compact));
        }
        this->insertIntoTable(forwrite);
    }
    else
    {
        qWarning() << "";
    }

}


QJsonArray TimeSeriesDocumentDBI::deleteZeros(QJsonArray &actual)
{
    QMap<int, double> map;
    QJsonArray outJson;
    QJsonDocument doc;
    doc.setArray(actual);
    map = getMapFromJson(doc.toJson(QJsonDocument::Compact));
    int zeroInTS = 0;
    foreach (int point, map.keys())
    {
        if (map[point] == 0)
        {
            zeroInTS++;
        }
        else
        {
            zeroInTS = 0;
        }
        if (zeroInTS < 2)
        {
            QJsonObject obj;
            obj.insert("num",point);
            obj.insert("value",map[point]);
            outJson.append(obj);
        }
    }
    return outJson;
}


TimeSeriesDBI::~TimeSeriesDBI()
{

}



bool TimeSeriesDBI::TimeSeriesStream::next()
{
    if (query_->next())
    {
        return true;
    }
    else
    {
        delete query_;
        return false;
    }
}

TimeSeries TimeSeriesDBI::TimeSeriesStream::current()
{
    return dbi_->fetchTimeSeriesFromQuery(query_);
}
