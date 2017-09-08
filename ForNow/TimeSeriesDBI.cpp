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


void TimeSeriesDocumentDBI::inhectionIn(const QHash<TimeSeriesID, QString> &init,
                                        const QHash<TimeSeriesID, QString> &additional)
{
    TimeSeriesList mainResult;
    QElapsedTimer timer;
    timer.start();
    QElapsedTimer jsonTimer;
    jsonTimer.restart();
    int jsonTime = 0;
    int jsonCounter = 0;

    jsonTimer.restart();
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
    qWarning() << "jsonTime:" << jsonTime << "jsonCounter:" << jsonCounter;

    qWarning() << "insertF" << timer.restart();
    deleteFromOriginalTypes(mainResult);
    qWarning() << "deleteFromOriginalTypes" << timer.restart();
    insertIntoTableFromOriginalType(mainResult);
    qWarning() << "insertIntoTableFromOriginalType" << timer.elapsed();
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
        inhectionIn(tSLRecord, ts);
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

void TimeSeriesDocumentDBI::insertIntoTableFromOriginalType(const TimeSeriesList &ts)
{
    QHash <QString,QString> result;
    //qWarning() << "ts = " << ts;
    foreach (const TimeSeries object, ts)
    {
        if (!object.isEmpty())
        {
            QJsonArray array;
            for (int i = 0; i < object.length(); i ++)
            {
                QJsonObject point;
                point.insert("num", QJsonValue::fromVariant(i+1));
                point.insert("value", QJsonValue::fromVariant(object.value(i)));
                array.append(point);
            }
            QJsonDocument doc;
            doc.setArray(array);
            result.insert(object.id(),doc.toJson(QJsonDocument::Compact));
            //qWarning() << "result = " << result;
        }
    }
    this->insertIntoTable(result);
}

QList<QString> TimeSeriesDocumentDBI::fetchAllIDs(const QList<QString> names)
{
    return names;
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

QSqlQuery TimeSeriesDocumentDBI::getQueryForIndependQuery(const  QList<TimeSeriesID> &ids)
{
    QSqlQuery query(m_db_);
    query.prepare("CREATE TEMPORARY TABLE tempTimeSeriesByPoints (Id TEXT UNIQUE NOT NULL)");
    query.exec();
    {
        m_db_.transaction();
        QSqlQuery query(m_db_);
        query.prepare("INSERT INTO timeSeriesByPoints (Id) VALUES (:Id)");
        foreach(const TimeSeriesID &id, ids)
        {
            query.bindValue(":Id", id);
            query.exec();
        }
        m_db_.commit();
    }
    query.setForwardOnly(true);
    query.exec("SELECT Key, Value FROM timeSeriesByPoints INNER JOIN tempTimeSeriesByPoints ON"
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
            //qWarning () << "I get " << jsonArray;
            QJsonDocument doc;
            doc.setArray(jsonArray);
            forwrite.insert(key,doc.toJson(QJsonDocument::Compact));
        }
        this->insertIntoTable(forwrite);
    }
    else
    {
        qWarning() << "";
    }

}


TimeSeriesDBI::~TimeSeriesDBI()
{

}

TimeSeries TimeSeriesDBI::fetchTimeSeriesFromQuery(QSqlQuery *query)
{

}

bool TimeSeriesDBI::TimeSeriesStream::next()
{
    return query_->next();
//    if (query_.next())
//    {
//        QHash <QString,QString> result;
//        actualTS_.clear();
//        const QString id = query_.value(0).toString();
//        if(listOfIds_.contains(id))
//        {

//            result[id] = query_.value(1).toString();
//            QMap <int, double> mapTS = TimeSeriesDocumentDBI::getMapFromJson(result.value(0));
//            actualTS_ = TimeSeriesDocumentDBI::timeSeriesFromQMap(result.values(), mapTS);
//        }
//        return true;
//    }
//    return false;
}

TimeSeries TimeSeriesDBI::TimeSeriesStream::current()
{
    return dbi_->fetchTimeSeriesFromQuery(query_);
}
