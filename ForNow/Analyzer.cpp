#include "Analyzer.h"
#include "DataInMemmoryMoc.h"
#include "TimeSeriesDBI.h"

Analyzer::Analyzer(const AnalysisTag &tag) :
    tag_(tag)
{
}

Analyzer::~Analyzer()
{
}

AnalysisTag Analyzer::tag() const
{
    return tag_;
}

double Analyzer::avg(const TimeSeries &timeSeries)
{
    if(timeSeries.isEmpty())
    {
        return 0.0;
    }

    double sum = 0.0;
    foreach(const double e, timeSeries.values())
    {
        sum += e;
    }

    return sum / timeSeries.size();
}


QString AnalysisResult::toJSONString()
{
    QJsonObject jsonObject;
    foreach(const TimeSeriesID &id, table_.keys())
    {
        QJsonObject rowObject;
        foreach(const AnalysisTag &tag, table_[id].keys())
        {
            rowObject[tag] = table_[id].value(tag);
        }
        jsonObject[id] = rowObject;
    }
    QJsonDocument doc(jsonObject);
    return doc.toJson() ;
}


AnalysisResult AnalysisResult::loadJson(const QString fileName)
{
    QFile jsonFile(fileName);
    if(jsonFile.open(QIODevice::Text|QIODevice::ReadOnly))
    {
        QTextStream stream(&jsonFile);
        return fromJSONString(stream.readAll());
    }
    else
    {
        qWarning() << "saveJson does not work. File not found ";
    }
    return AnalysisResult();
}


void AnalysisResult::saveJson(const QString fileName)
{
    QFile jsonFile(fileName);
    if(jsonFile.open(QIODevice::Text|QIODevice::WriteOnly))
    {
        QTextStream stream(&jsonFile);
        stream << toJSONString();
    }
    else
    {
        qWarning() << "saveJson does not work. File not found ";
    }
}


AnalysisResult AnalysisResult::fromJSONString(const QString listOfJsonObject)
{
    AnalysisResult results;
    QJsonDocument doc = QJsonDocument::fromJson(listOfJsonObject.toUtf8());
    QJsonObject obj = doc.object();
    foreach (const QString id, obj.keys())
    {
        foreach (const QString tag, obj[id].toObject().keys())
        {
            results.insert(id, tag, obj[id].toObject()[tag].toDouble());
        }
    }
    return results;
}

AnalysisResult AnalysisResult::project (const TimeSeriesID id) const
{
    AnalysisResult z;
    z.table_.insert(id, table_.value(id));
    return z;
}


AnalysisResult &AnalysisResult::insert(const TimeSeriesID &id, const AnalysisTag &tag, const double result)
{
    if(!table_.contains(id))
    {
        QHash<AnalysisTag, double> m;
        m.insert(tag, result);
        table_.insert(id,m);
    }
    else
    {
        table_[id].insert(tag,result);
    }
    return *this;
}

AnalysisResult &AnalysisResult::insertRow(const TimeSeriesID &id, const QHash<AnalysisTag, double> row)
{
    table_.insert(id,row);
    return *this;
}

AnalysisResult &AnalysisResult::unite(const AnalysisResult &result)
{
    table_.unite(result.table_);
    return *this;
}

QList<AnalysisTag> AnalysisResult::tags() const
{
    return table_.keys();
}

QList<AnalysisTag> AnalysisResult::tagsInside(const QString id) const
{
    return table_[id].keys();
}

QHash<TimeSeriesID, QHash<AnalysisTag, double> > AnalysisResult::getTable()
{
    return table_;
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
        foreach(const double  element, timeSeries.values())
        {
            summ +=  pow((element - aver),2);
        }
        const double divider = (timeSeries.size() - 1);
        if(divider == 0)
        {
            return 0;
        }
        return sqrt(summ / divider);

    }
    return 0;
}

double Analyzer::var(const TimeSeries &timeSeries)
{
    double avg_ = avg(timeSeries);
    double dev_ = dev(timeSeries);
    if (avg_ == 0 || dev_ == 0)
    {
        return 0;
    }
    return dev_/avg_;
}

Analyzer::Analyzer()
{
}

bool TimeSeries :: operator ==(const TimeSeries &series) const
{
    if(size() != series.size())
    {
        return false;
    }
    foreach (int key, series.keys())
    {
        if(!fuzzyCompare(this->value(key), series.value(key)))
        {
            return false;
        }
    }

    return id_ == series.id_;
}

bool TimeSeries::operator <(const TimeSeries &series) const
{
    return id_ < series.id_;
}
double AnalysisResult::value(const QString &id,const QString &tag) const
{
    return table_.value(id).value(tag, 0.0);
}

AnalysisResult::AnalysisResult()
{
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

QString AnalysisResult:: StrAll() const

{
    QString row;
    foreach(const QString id,  table_.keys())
    {
        row.append(id);
        row.append(" : ");
        foreach(const QString tag, table_[id].keys())
        {
            row.append(" #");
            row.append(tag);
            row.append(" = ");
            row.append(QString::number(table_[id].value(tag)));
        }
    }

    return row;
}

bool AnalysisResult::operator !=(const AnalysisResult &result) const
{
    return !(*this == result);
}


ComplexAnalyzer::ComplexAnalyzer(const QList<Analyzer *> analyzers) :
    Analyzer("#complex"),
    analyzers_(analyzers)
{
}

ComplexAnalyzer::~ComplexAnalyzer()
{
    foreach(Analyzer *a, analyzers_)
    {
        delete a;
    }
    analyzers_.clear();
}

AnalysisResultForOne ComplexAnalyzer::analyze(const TimeSeries &timeSeries)
{
    AnalysisResultForOne result;
    foreach(Analyzer *a, analyzers_)
    {
        result.unite(a->analyze(timeSeries));
    }
    return result;
}

AnalysisResultForOne ComplexAnalyzer::analyzeForID(const TimeSeriesID &id, const TimeSeries list)
{
    return analyze(TimeSeries(id) = list);
}

AnalysisResult ComplexAnalyzer::analyzeForIDs(TimeSeriesDBI *dbi, const QList<TimeSeriesID> &ids)
{
    AnalysisResult results;
    foreach (const TimeSeries &ts, dbi->read(ids))
    {
        results.insertRow(ts.id(), analyze(ts));
    }
    return results;
}

AnalysisResult ComplexAnalyzer::analyzeForIDsStream(TimeSeriesDBI *dbi, const QList<TimeSeriesID> &ids)
{
    AnalysisResult results;
    {
        TimeSeriesDBI *streamReader = dbi;
        TimeSeriesDBI::TimeSeriesStream *stream = streamReader->stream(ids);
        while(stream->next())
        {
            results.insertRow(stream->current().id(), analyze(stream->current()));
        }
        //delete stream;
    }
    return results;
}

AvgAnalyzer::AvgAnalyzer() :
    Analyzer("Average")
{
}

AvgAnalyzer::~AvgAnalyzer()
{
}

AnalysisResultForOne AvgAnalyzer::analyze(const TimeSeries &timeSeries)
{
    AnalysisResultForOne temp;
    temp.insert(tag(), avg(timeSeries));
    return temp;
}

VarCoefAnalyzer::VarCoefAnalyzer() :
    Analyzer("Variation")
{
}

VarCoefAnalyzer::~VarCoefAnalyzer()
{
}

AnalysisResultForOne VarCoefAnalyzer::analyze(const TimeSeries &timeSeries)
{
    AnalysisResultForOne temp;
    temp.insert(tag(), var(timeSeries));
    return temp;
}

DevAnalyzer::DevAnalyzer() :
    Analyzer("Deviation")
{
}

DevAnalyzer::~DevAnalyzer()
{
}

AnalysisResultForOne DevAnalyzer::analyze(const TimeSeries &timeSeries)
{
    AnalysisResultForOne temp;
    temp.insert(tag(), dev(timeSeries));
    return temp;
}
