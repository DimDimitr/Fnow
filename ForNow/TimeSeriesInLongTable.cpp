#include "TimeSeriesInLongTable.h"

TimeSeriesInLongTable::TimeSeriesInLongTable()
{

}
QSqlDatabase TimeSeriesInLongTable::db_;

//                  NEW TYPE OF DB

TimeSeriesInLongTable::TimeSeriesInLongTable(const QString path)
{
    db_ = QSqlDatabase();
    QSqlQuery query(db_);
    QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);

    db_ = QSqlDatabase::addDatabase("QSQLITE");
    db_.setDatabaseName(path);
    db_.open();

    query = QSqlQuery(db_);

    if (!db_.isOpen())
    {
        //qWarning() << "Error: connection with database fail from TimeSeriesDBI";
    }
    else
    {

        QSqlQuery query("CREATE TABLE timeSeriesByPoints ("
                        "Key TEXT NOT NULL,"
                        "Num INT NOT NULL,"
                        "Value DOUBLE NOT NULL,"
                        "PRIMARY KEY (Key, Num)"
                        ")");
        query.exec();



        //qWarning() << "Database: connection ok" << query.lastError();
    }

}


void TimeSeriesInLongTable::inhectionIn(const QHash <QString, QMap<int, double> > &init, const QHash <QString, QHash<int, double> > &additional)
{

    TimeSeriesList mainResult;
    foreach (QString id, init.keys())
    {

        QMap <int,double> initMap;
        QMap <int,double> additionalMap;
        foreach (int i, additional[id].keys())
        {
            additionalMap.insert(i,additional[id].value(i));
        }
        foreach (int i, init[id].keys())
        {
            initMap.insert(i,init[id].value(i));
        }
        int min;
        int max;
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
    }
    deleteFromOriginalTypes(mainResult);
    //qWarning() << "deleteFromOriginalTypes" << timer.restart();
    insertIntoTableFromOriginalTypes(mainResult);
}

void TimeSeriesInLongTable::deleteFromOriginalTypes(const TimeSeriesList &ts)
{
    db_.transaction();
    QSqlQuery query(db_);
    query.prepare("DELETE FROM timeSeriesByPoints WHERE Key = :id");
    foreach (TimeSeries id, ts)
    {
        query.bindValue(":id", id.id());
        query.exec();
    }
    db_.commit();
}

void TimeSeriesInLongTable::insertIntoTable(const QHash <QString, QHash <int, double> > &ts)
{
    QHash <QString, QMap<int, double> > tSLRecord = hashFromDatBase(ts.keys());
    if (!tSLRecord.isEmpty())
    {
        inhectionIn(tSLRecord, ts);
    }
    else
    {
        db_.transaction();

        QSqlQuery query(db_);
        query.prepare("INSERT OR REPLACE INTO timeSeriesByPoints (Key, Num, Value) VALUES (:Key, :Num, :Value)");

        foreach(const TimeSeriesID &id, ts.keys())
        {
            foreach (const int &point, ts[id].keys())
            {
                query.bindValue(":Key", id);
                query.bindValue(":Num", point);
                query.bindValue(":Value", ts[id].value(point));
                query.exec();
            }
        }
        db_.commit();
    }
}

void TimeSeriesInLongTable::insertIntoTableFromOriginalTypes(const TimeSeriesList &ts)
{
    QHash <QString, QHash <int, double> > result;
    foreach (const TimeSeries &object, ts)
    {
        QHash <int, double> actualHash;
        for (int i = 0; i < object.length(); i ++)
        {
            actualHash.insert(i, object.at(i));
        }
        result.insert(object.id(),actualHash);
    }
    //qWarning() << "I get" << result;
    this->insertIntoTable(result);
}

QList<QString> TimeSeriesInLongTable::fetchAllIDs(const QList<QString> names)
{
    return names;
}

bool TimeSeriesInLongTable::clear(const QString &databaseName)
{
    db_.close();
    db_ = QSqlDatabase();
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

void TimeSeriesInLongTable::loadDataFromFile(const QString &path)
{
    loadDataFromJson(path);
}

void TimeSeriesInLongTable::write(const TimeSeriesList &ts)
{
    insertIntoTableFromOriginalTypes(ts);
}

TimeSeriesList TimeSeriesInLongTable::read(const QList<TimeSeriesID> &ids)
{
    return getStringFromDatBase(ids);
}

bool TimeSeriesInLongTable::remove(const QString &databaseName)
{
    return clear(databaseName);
}

TimeSeriesInLongTable *TimeSeriesInLongTable::open(const QString &databaseName)
{
    return new TimeSeriesInLongTable(databaseName);
}

QHash <QString, QMap<int, double> > TimeSeriesInLongTable::hashFromDatBase(const  QList<QString> &ids)
{
    QSet<QString> idSet = ids.toSet();
    QSqlQuery query(db_);
    QHash <QString, QMap<int, double> > rawPointsTable;
    query.setForwardOnly(true);
    query.exec("SELECT Key, Num, Value FROM timeSeriesByPoints");
    while(query.next())
    {
        const QString id = query.value(0).toString();
        const int num = query.value(1).toInt();
        const double value = query.value(2).toDouble();
        if(idSet.contains(id))
        {
            rawPointsTable[id][num] = value;
        }
    }
    return rawPointsTable;
}

QSqlQuery* TimeSeriesInLongTable::getQueryForIndependQuery(const  QList<TimeSeriesID> &ids)
{
    QSqlQuery *query;
    query = new  QSqlQuery (db_);
    query->prepare("CREATE TEMPORARY TABLE tempTimeSeriesByPoints (Id TEXT UNIQUE NOT NULL)");
    if (!query->exec())
    {
        qWarning() << "query.lastError 1 " << query->lastError();
    }
    {
        db_.transaction();
        QSqlQuery query(db_);
        query.prepare("INSERT INTO tempTimeSeriesByPoints (Id) VALUES (:Id)");
        foreach(const TimeSeriesID &id, ids)
        {
            query.bindValue(":Id", id);
            if (!query.exec())
            {
                qWarning() << "query.lastError 2 " << query.lastError();
            }
        }
        db_.commit();
    }
    query->prepare("SELECT Id FROM tempTimeSeriesByPoints");
    if (!query->exec())
    {
        qWarning() << "query.lastError 3 " << query->lastError();
    }
    query->setForwardOnly(true);
    query->exec("SELECT Key, Num, Value FROM timeSeriesByPoints INNER JOIN tempTimeSeriesByPoints ON"
               " timeSeriesByPoints.Key = tempTimeSeriesByPoints.Id ORDER BY Key");
    return query;
}


QList <TimeSeries> TimeSeriesInLongTable::getStringFromDatBase(const  QList<QString> &ids)
{
    QList <TimeSeries> result;
    if (!db_.open())
    {
        qWarning() << "Error: connection with database fail from getStringFromDatBase";
    }
    else
    {
        QHash <QString, QMap<int, double> > rawPointsTable = hashFromDatBase(ids);
        foreach(const QString &id, ids)
        {

            result <<  timeSeriesFromQMap(id, rawPointsTable[id]);
        }

    }
    std::sort(result.begin(), result.end());
    return result;
}



void TimeSeriesInLongTable::loadDataFromJson(const QString path)
{
    QJsonDocument docjson;
    QFile jsonFile(path);
    if(jsonFile.open(QIODevice::Text|QIODevice::ReadOnly))
    {
        QTextStream stream(&jsonFile);
        docjson = QJsonDocument::fromJson(stream.readAll().toUtf8());
        QHash <QString, QHash<int, double> > forwrite;
        QJsonObject jsonObject = docjson.object();
        foreach (const QString &key, jsonObject.keys())
        {
            QJsonArray jsonArray = jsonObject[key].toArray();
            QHash<int, double> res;
            foreach(const QJsonValue ob, jsonArray)
            {
                QJsonObject object;
                object = ob.toObject();
                int numb;
                foreach (const QString tag, object.keys())
                {
                    if ( tag == "num")
                    {
                        numb = object[tag].toInt();
                    }
                    else if( tag == "value")
                    {
                        res.insert(numb, object[tag].toDouble());
                    }

                }
            }
            forwrite.insert(key,res);
        }
        this->insertIntoTable(forwrite);
    }
    else
    {
        qWarning () << "";
    }

}

TimeSeries TimeSeriesInLongTable::timeSeriesFromQMap(const QString &strJsonValue, QMap <int, double> mapTS)
{

    qWarning() << "timeSeriesFromQMap" << mapTS;
    if(!mapTS.isEmpty())
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
        qWarning() << "results" << results.id() << results;
    return results;
    }
    else
    {
    return TimeSeries();
    }
}



TimeSeries TimeSeriesInLongTable::fetchTimeSeriesFromQuery(QSqlQuery *query)
{
    qWarning() << "TimeSeriesInLongTable::fetchTimeSeriesFromQuery";
    QMap <int, double> mapTS;
    int key = 1;
    TimeSeriesID previousID;
    Q_ASSERT(query != Q_NULLPTR);
    while(query->next())
    {
        TimeSeriesID currentTimeSeriesID = query->value(0).toString();

        qWarning() << "currentTimeSeriesID:" << currentTimeSeriesID;

        if(previousID.isNull())
        {
            const int num = query->value(1).toInt();
            const double value = query->value(2).toDouble();
            mapTS.insert(num, value);

            previousID = currentTimeSeriesID;
        }
        else if(previousID == currentTimeSeriesID)
        {
            const int num = query->value(1).toInt();
            const double value = query->value(2).toDouble();
            mapTS.insert(num, value);
        }
        else
        {
            break;
        }

        //        if (key == 1)
        //        {
        //            idR = query->value(0).toString();
        //            key = 2;
        //        }
        //        if(query->value(0).toString() == idR)
        //        {
        //            mapTS.insert(query->value(1).toInt(), query->value(1).toDouble());
        //            query->next();
        //        }
        //        else {
        //            key = 0;
        //        }
    }
    return timeSeriesFromQMap(previousID, mapTS);
}
