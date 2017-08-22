#include "Analyzer.h"
#include "DataInMemmoryMoc.h"
#include "TimeSeriesDBI.h"

double Analyzer::avg(const TimeSeries &timeSeries)
{
    if(timeSeries.isEmpty())
    {
        return 0.0;
    }

    double sum = 0.0;
    foreach(const double e, timeSeries)
    {
        sum += e;
    }

    return sum / timeSeries.length();
}

double Analyzer::dev(const TimeSeries &timeSeries)
{

    if (timeSeries.isEmpty())
    {
        return 0;
    }
    else
    {
        const double aver = avg(timeSeries);
        double summ = 0;
        foreach(const double  element, timeSeries)
        {
            summ +=  pow((element - aver),2);
        }
        const double divider = (timeSeries.length() - 1);
        if(divider == 0)
        {
            return 0;
        }
        return sqrt(summ / divider);

    }
}

double Analyzer::var(const TimeSeries &timeSeries)
{
    double avg_ = avg(timeSeries);
    double dev_ = dev(timeSeries);
    if (avg == 0 || dev_ == 0)
    {
        return 0;
    }
    return dev_/avg_;
}

bool TimeSeries :: operator ==(const TimeSeries &series) const
{
    if(size() != series.size())
    {
        return false;
    }

    for(int i = 0; i < series.size(); i++)
    {
        if(!fuzzyCompare(at(i), series.at(i)))
        {
            return false;
        }
    }

    return id_ == series.id_;
}
double AnalysisResult::value(const QString &id,const QString &tag) const
{
    return table_.value(id).value(tag, 0.0);
}

bool AnalysisResult:: operator == (const AnalysisResult &result) const
{
    foreach(const QString &id, (table_.keys() + result.table_.keys()).toSet())
        foreach(const QString &tag, (table_[id].keys() + result.table_[id].keys()).toSet())
        {
            if(table_.contains(tag) || result.table_.contains(tag))
            {
                return false;
            }
            if(!fuzzyCompare(value(id,tag), result.value(id,tag)))
            {
                return false;
            }
        }
    return true;
}



QString AnalysisResult:: operator << (const AnalysisResult &result) const

{
    QString row;
    foreach(const QString id,  result.table_.keys())
    {
        row.append(id);
        row.append(" : ");
        foreach(const QString tag, result.table_[id].keys())
        {
            row.append(" #");
            row.append(tag);
            row.append(" = ");
            row.append(QString::number(result.table_[id].value(tag)));
        }
    }

    return row;
}



bool AnalysisResult::operator !=(const AnalysisResult &result) const
{
    return !(*this == result);
}


AnalysisResult ComplexAnalyzer::analyzeForIDs(TimeSeriesDBI *database, const QList<QString> &ids)
{
    AnalysisResult results;
    TimeSeriesList listTmSrs = database->timeSeriesFromString(ids);
    foreach (const TimeSeries &tsString, listTmSrs)
    {
        results.insertRow(tsString.id(), analyzeForID(tsString.id(), tsString));
    }
    return results;
}

AnalysisResult ComplexAnalyzer::analyzeForIDsTestMoc(DataInMemmoryMoc *database, const QList<QString> &ids)
{
    AnalysisResult results;
    TimeSeriesList listTmSrs = database->timeSeriesFromString(ids);
    foreach (const TimeSeries &tsString, listTmSrs)
    {
        results.insertRow(tsString.id(), analyzeForID(tsString.id(), tsString));
    }
    return results;
}
