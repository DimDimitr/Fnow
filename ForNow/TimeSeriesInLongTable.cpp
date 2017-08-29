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

void TimeSeriesInLongTable::insertIntoTable(const QHash <QString, QHash <int, double> > &ts)
{
    db_.transaction();

    QSqlQuery query(db_);
    query.prepare("INSERT OR REPLACE INTO timeSeriesByPoints (Key, Num, Value) VALUES (:Key, :Num, :Value)");

    foreach(const TimeSeriesID &id, ts.keys())
    {
        //        qInfo() << "id" << id;
        //const QString value = ts.value(id);
        foreach (const int &point, ts[id].keys())
        {
            //            qWarning() << "row:" << id << point << ts[id].value(point);

            query.bindValue(":Key", id);
            query.bindValue(":Num", point);
            query.bindValue(":Value", ts[id].value(point));
            query.exec();
        }
    }
    db_.commit();
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
    /*QSqlQuery query(db_);
    query.prepare("DELETE FROM" + databaseName + "timeSeriesByPoints");
    query.exec();*
    db_.close();
    db_= QSqlDatabase();
    QSqlDatabase::removeDatabase(databaseName);
    return true;*/

    QSqlQuery query(db_);
    query.prepare("DELETE FROM" + databaseName + "timeSeriesByPoints");
    //query.exec();
    db_.close();
    db_ = QSqlDatabase();
    QSqlDatabase::removeDatabase(databaseName);
    QFile(databaseName).remove();
    return true;
}

void TimeSeriesInLongTable::loadDataFromFile(const QString &path)
{
    Q_UNUSED(path);
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




QList <TimeSeries> TimeSeriesInLongTable::getStringFromDatBase(const  QList<QString> &ids)
{
    QList <TimeSeries> result;
    if (!db_.open())
    {
        qWarning() << "Error: connection with database fail from getStringFromDatBase";
    }
    else
    {
        QElapsedTimer timer;
        timer.restart();
        QSet<QString> idSet = ids.toSet();
        QSqlQuery query(db_);
        timer.restart();
                QHash <QString, QMap<int, double> > rawPointsTable;
        query.setForwardOnly(true);
        query.exec("SELECT Key, Num, Value FROM timeSeriesByPoints");
        //query.lastError();
        //qWarning() << ids;

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
        foreach(const QString &id, rawPointsTable.keys())
        {
            result << (TimeSeries(id) << rawPointsTable.value(id).values());
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
    }

}
