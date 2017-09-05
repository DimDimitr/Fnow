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
        //qWarning() << "Database: connection ok" << query.lastError();
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


void TimeSeriesInArray::injectionIn(const QHash <QString, QString> &tSLRecord, const QHash <QString, QString> &ts)
{
    //qWarning() << "tSLRecordts";
    TimeSeriesList mainResult;
    foreach (QString tsR, tSLRecord.keys())
    {
        QMap <int,double> mTimeSrsResul;
        QMap <int,double> mapTimeSrsReadDB = getMapFromStr(tSLRecord.value(tsR));
        QMap <int,double> mapTimeSrsWriteJS = getMapFromStr(ts.value(tsR));
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
            for (int i = 0; i < object.length(); i ++)
            {
                actualStr.append(QString::number(i));
                actualStr.append("*");
                actualStr.append(QString::number(object.value(i)));
                actualStr.append("_");
            }
            actualStr.chop(1);
            result.insert(object.id(),actualStr);
        }
    }
    //qWarning() << "result:" << result;
    this->insertIntoTable(result);

}




QList<QString> TimeSeriesInArray::fetchAllIDs(const QList<QString> names)
{
    return names;
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


/*QHash <QString, QString> TimeSeriesInArray::hashFromDatBase (const  QList<QString> &ids)
{

}*/

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
    std::sort(mainResult.begin(), mainResult.end());
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
        //qWarning() << result.keys();
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
            foreach(const QJsonValue ob, jsonArray)
            {
                QJsonObject object;
                object = ob.toObject();
                foreach (const QString tag, object.keys())
                {
                    if ( tag == "num")
                    {
                        res.append(QString::number(object[tag].toInt()));
                        res.append("*");
                    }
                    else if( tag == "value")
                    {
                        res.append(QString::number(object[tag].toInt()));
                        res.append("_");
                    }


                }
            }
            res.chop(1);
            forwrite.insert(key,res);
        }
        this->insertIntoTable(forwrite);
    }
    else
    {
        qWarning () << "";
    }


}
