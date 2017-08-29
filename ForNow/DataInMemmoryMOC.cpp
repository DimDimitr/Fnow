#include "DataInMemmoryMoc.h"

QHash<QString, TimeSeries> DataInMemmoryMoc::storage_;



DataInMemmoryMoc::DataInMemmoryMoc(const QString)
{
}

DataInMemmoryMoc::DataInMemmoryMoc()
{

}

void DataInMemmoryMoc::insertIntoTableFromOriginalType(const QList<TimeSeries> &tsl)
{
    foreach (const TimeSeries ts,tsl)
    {
        storage_.insert(ts.id(), ts);
    }
}

QList<TimeSeries> DataInMemmoryMoc::timeSeriesFromString(const QList<QString> &id)
{
    QList<TimeSeries> result;
    foreach (QString oneId, id)
    {
        result.append(storage_.value(oneId, TimeSeries(oneId)));
    }
    return result;
}

bool DataInMemmoryMoc::clear(const QString &databaseName)
{
    Q_UNUSED(databaseName);
    storage_.clear();
    return true;
}

QList<QString> DataInMemmoryMoc::fetchAllIDs(QList<QString> names)
{
    return names;
}
