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
        query.exec();
        //qWarning() << "Database: connection ok" << query.lastError();
    }

}


void TimeSeriesDocumentDBI::inhectionIn(const QHash <QString, QString> &tSLRecord, const QHash <QString, QString> &ts)
{
    TimeSeriesList mainResult;
    foreach (QString tsR, tSLRecord.keys())
    {
        QMap <int,double> mTimeSrsResul;
        QMap <int,double> mapTimeSrsReadDB = getMapFromJson(tSLRecord.value(tsR));
        QMap <int,double> mapTimeSrsWriteJS = getMapFromJson(ts.value(tsR));
        //qWarning() << "i get mapTimeSrsWriteJS" << mapTimeSrsWriteJS;
        int min = mapTimeSrsWriteJS.firstKey();
        int max = mapTimeSrsWriteJS.lastKey();

        if (min <= mapTimeSrsReadDB.firstKey())
        {
            foreach(int key, mapTimeSrsWriteJS.keys())
            {
                mTimeSrsResul.insert(key, mapTimeSrsWriteJS[key]);
            }
            if (max < mapTimeSrsReadDB.lastKey())
            {
                for (int i = max; i < mapTimeSrsReadDB.lastKey() + 1; i++)
                {
                    if (mapTimeSrsReadDB[i])
                    {
                        mTimeSrsResul.insert(i, mapTimeSrsReadDB[i]);
                    }
                }
            }
        }
        else
        {
            foreach (int key, mapTimeSrsReadDB.keys())
            {
                if (key < min)
                {
                    mTimeSrsResul.insert(key, mapTimeSrsReadDB[key]);
                }
            }
            foreach (int key, mapTimeSrsWriteJS.keys())
            {
                mTimeSrsResul.insert(key, mapTimeSrsWriteJS[key]);
            }
            for ( int key = max; key < mapTimeSrsReadDB.lastKey(); key++)
            {
                if (mapTimeSrsReadDB[key])
                {
                    mTimeSrsResul.insert(key, mapTimeSrsReadDB[key]);
                }
            }
        }
        mainResult.append(timeSeriesFromQMap(tsR, mTimeSrsResul));
        //qWarning() << "I get TS result = " << tsR << mainResult;
    }
    insertIntoTableFromOriginalType(mainResult);
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

void TimeSeriesDocumentDBI::insertIntoTableFromOriginalType(const TimeSeriesList &ts)
{
    QSqlQuery query(m_db_);
    //qWarning() << "i have " << ts;
    foreach (TimeSeries id, ts)
    {
        query.prepare("DELETE FROM timeSeriesByPoints WHERE Key = ?");
        query.addBindValue(id.id());
    }
    query.exec();
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

    QMap <int, double> mapTS;
    foreach(const QJsonValue ob, jsonArray)
    {
        QJsonObject object;
        object = ob.toObject();
        double val;
        int point;
        foreach (const QString tag, object.keys())
        {
            if( tag == "value")
            {
                val=(object[tag].toDouble());
            }
            else
            {
                point=(object[tag].toInt());
            }
        }
        mapTS.insert(point,val);
    }
    return mapTS;
}

TimeSeries TimeSeriesDocumentDBI::timeSeriesFromQMap(const QString &strJsonValue, QMap <int, double> mapTS)
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


QList<TimeSeries> TimeSeriesDocumentDBI::timeSeriesFromString(const QList<QString> &ids)
{
    QHash<QString, QString> strJson = this->getStringFromDatBase(ids);
    QList<TimeSeries> mainResult;
    foreach(const QString &strJsonValue, strJson.keys())
    {
        QMap <int, double> mapTS = getMapFromJson(strJson[strJsonValue]);
        TimeSeries results = timeSeriesFromQMap(strJsonValue, mapTS);
        mainResult.append(results);
    }
    std::sort(mainResult.begin(), mainResult.end());
    return mainResult;
}

QHash <QString,QString> TimeSeriesDocumentDBI::getStringFromDatBase(const  QList<QString> &ids)
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
