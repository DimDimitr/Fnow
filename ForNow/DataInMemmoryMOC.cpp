#include "DataInMemmoryMoc.h"

QHash<QString, QMap<int, double> > DataInMemmoryMoc::storage_;

DataInMemmoryMoc::DataInMemmoryMoc(const QString)
{
}

DataInMemmoryMoc::DataInMemmoryMoc()
{

}

//Done
void DataInMemmoryMoc::insertIntoTableFromOriginalType(const QList<TimeSeries> &tsl)
{
    foreach (const TimeSeries ts,tsl)
    {
        int i = 0;
        QMap <int, double> hashTemp;
        foreach (double value, ts.values())
        {
            hashTemp.insert(i, value);
        }
        storage_.insert(ts.id(), hashTemp);
    }
}

//Done
QList<TimeSeries> DataInMemmoryMoc::timeSeriesFromString(const QList<QString> &ids)
{
    QList<TimeSeries> result;
    foreach (QString oneId, ids)
    {
        result.append(timeSeriesFromQMap(oneId, storage_.value(oneId)));
    }
    return result;
}

//Done
TimeSeries DataInMemmoryMoc::timeSeriesFromQMap(const TimeSeriesID &strJsonValue, QMap <int, double> mapTS)
{
    TimeSeries results(strJsonValue);
    if (!mapTS.isEmpty())
    {
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
    }
    return results;
}

TimeSeries DataInMemmoryMoc::fetchTimeSeriesFromQuery(QSqlQuery *query)
{
   Q_UNUSED(query);
   const TimeSeries ts =  timeSeriesFromQMap(currentID_, storage_.value(currentID_));
   nextID();
   qWarning() << "listOfId_" << listOfId_ << "currentID_" << currentID_;
   return ts;
}

//Done
bool DataInMemmoryMoc::clear(const QString &databaseName)
{
    Q_UNUSED(databaseName);
    storage_.clear();
    return true;
}

//Done
QList<QString> DataInMemmoryMoc::fetchAllIDs(QList<QString> names)
{
    return names;
}

//done
void DataInMemmoryMoc::loadDataFromFile(const QString &path)
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
            QMap<int, double> mapTemp;
            foreach(const QJsonValue ob, jsonArray)
            {
                QJsonObject object;
                object = ob.toObject();
                mapTemp.insert(object.value("num").toInt(), object.value("value").toDouble());
            }
            storage_.insert(key, mapTemp);
        }
    }
    else
    {
        qWarning () << "";
    }
}

//done
void DataInMemmoryMoc::write(const TimeSeriesList &ts)
{
    insertIntoTableFromOriginalType(ts);
}

//Done
TimeSeriesList DataInMemmoryMoc::read(const QList<TimeSeriesID> &ids)
{
    return timeSeriesFromString(ids);
}
//Done
bool DataInMemmoryMoc::remove(const QString &databaseName)
{
    return clear(databaseName);
}
//Done
TimeSeriesDBI *DataInMemmoryMoc::open(const QString &databaseName)
{
    return new DataInMemmoryMoc(databaseName);
}
