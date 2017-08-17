#ifndef ANALYZER_H

#define ANALYZER_H

#include <QtCore>
#include <QtTest/QTest>
#include <TimeSeries.h>

class TimeSeriesDBI;
class DataInMemmoryMoc;

inline bool fuzzyCompare(const double d1, const double d2)
{
    return qAbs(d1 - d2) <= 0.00001;
}

typedef QString TimeSeriesID;
typedef QString AnalysisTag;
typedef QHash<QString,double> AnalysisResultForOne;


template <class Key, class T>
class Hash : public QHash<Key, T>
{
public:
    Hash &insertInc(const Key &key, const T &value)
    {
        this->insert(key, value);
        return *this;
    }
};

class AnalysisResult
{

public:
    AnalysisResult()
    {
    }


    double value(const QString &id,const QString &tag) const;

    bool operator == (const AnalysisResult &result) const;
    bool operator != (const AnalysisResult &result) const;
    QString operator << (const AnalysisResult &result) const;




    QString toJSONString()
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
        qWarning() << "************************I am toJSONString function************************";
        return doc.toJson() ;
    }

    AnalysisResult loadJson(QString fileName)
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


    void saveJson(QString fileName)
    {
        //qWarning() << "analiseDat.toJSONString():";
        //qWarning() << qPrintable(AnalysisResult::toJSONString());
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



    AnalysisResult fromJSONString( QString listOfJsonObject)
    {
        AnalysisResult results;
        QJsonDocument doc = QJsonDocument::fromJson(listOfJsonObject.toUtf8());
        QJsonObject obj = doc.object();
        foreach (QString id, obj.keys())
        {
            foreach (QString tag, obj[id].toObject().keys())
            {
                results.insert(id, tag, obj[id].toObject()[tag].toDouble());
            }
        }
        return results;
    }




    AnalysisResult project (TimeSeriesID id) const
    {
        AnalysisResult z;
        z.table_.insert(id, table_.value(id));
        return z;
    }

    AnalysisResult &insert(const TimeSeriesID &id, const AnalysisTag &tag, const double result)
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

    AnalysisResult &insertRow(const TimeSeriesID &id, const QHash<AnalysisTag, double> row)
    {
        table_.insert(id,row);
        return *this;
    }

    AnalysisResult &unite(const AnalysisResult &result)
    {
        table_.unite(result.table_);
        return *this;
    }

    QList<AnalysisTag> tags() const
    {
        return table_.keys();
    }

    QList<AnalysisTag> tagsInside(const QString id) const
    {
        return table_[id].keys();
    }

    //private:
    QHash<TimeSeriesID, QHash<AnalysisTag, double> >  table_;
};
Q_DECLARE_METATYPE(AnalysisResult)

class Analyzer
{
public:
    Analyzer(const AnalysisTag &tag) :
        tag_(tag)
    {

    }

    virtual ~Analyzer()
    {
    }

    virtual AnalysisResultForOne analyze(const TimeSeries &timeSeries) = 0;

    AnalysisTag tag() const
    {
        return tag_;
    }

    static double avg(const TimeSeries &timeSeries);
    static double dev(const TimeSeries &timeSeries);
    static double var(const TimeSeries &timeSeries);
    static QString nameOfTS();

private:
    Analyzer()
    {
    }

    AnalysisTag tag_;
};
Q_DECLARE_METATYPE(Analyzer*)



class AvgAnalyzer : public Analyzer
{
public:
    AvgAnalyzer() :
        Analyzer("Average")
    {
    }

    virtual ~AvgAnalyzer()
    {
    }

    virtual AnalysisResultForOne analyze(const TimeSeries &timeSeries)
    {
        AnalysisResultForOne temp;
        temp.insert(tag(),avg(timeSeries));
        return temp;
    }
};
Q_DECLARE_METATYPE(AvgAnalyzer*)

class VarCoefAnalyzer :public Analyzer
{
public:
    VarCoefAnalyzer() :
        Analyzer("Variation")
    {
    }

    virtual ~VarCoefAnalyzer()
    {
    }

    virtual AnalysisResultForOne analyze(const TimeSeries &timeSeries)
    {
        AnalysisResultForOne temp;
        temp.insert(tag(), var(timeSeries));
        return temp;
    }
};
Q_DECLARE_METATYPE(VarCoefAnalyzer*)

class DevAnalyzer : public Analyzer
{
public:
    DevAnalyzer() :
        Analyzer("Deviation")
    {
    }

    virtual ~DevAnalyzer()
    {
    }

    virtual AnalysisResultForOne analyze(const TimeSeries &timeSeries)
    {
        AnalysisResultForOne temp;
        temp.insert(tag(), dev(timeSeries));
        return temp;
    }
};
Q_DECLARE_METATYPE(DevAnalyzer*)

class ComplexAnalyzer : public Analyzer
{
public:
    ComplexAnalyzer(QList<Analyzer*> analyzers) :
        Analyzer("#complex"),
        analyzers_(analyzers)
    {
    }

    virtual ~ComplexAnalyzer()
    {
        foreach(Analyzer *a, analyzers_)
        {
            delete a;
        }
        analyzers_.clear();
    }

    virtual AnalysisResultForOne analyze(const TimeSeries &timeSeries)
    {
        AnalysisResultForOne result;
        foreach(Analyzer *a, analyzers_)
        {
            result.unite(a->analyze(timeSeries));
        }
        return result;
    }

    AnalysisResultForOne analyzeForID(const TimeSeriesID &id, TimeSeries list)
    {
        return analyze(TimeSeries(id)=list);
    }




    AnalysisResult analyzeForIDs(TimeSeriesDBI *database,
                                 const QList<QString> &ids);
    AnalysisResult analyzeForIDsTestMoc(DataInMemmoryMoc *database,
                                       const QList<QString> &ids);


private:
    QList<Analyzer*> analyzers_;
};
Q_DECLARE_METATYPE(ComplexAnalyzer*)

class TAnalyzer : public QObject
{
    Q_OBJECT

private slots:
    void TestAverage_data();
    void TestAverage();

    void TestDeviation_data();
    void TestDeviation();

    void TestVariation_data();
    void TestVariation();

    void TestAnalyze_data();
    void TestAnalyze();

    void TestAnalysisResultProject_data();
    void TestAnalysisResultProject();

    void TestAnalyzeForIDs_data();
    void TestAnalyzeForIDs();

    void TestJsonRecord_data();
    void TestJsonRecord();

    void TestJsonRecordInFile_data();
    void TestJsonRecordInFile();

    void TestTimeRecordWrite_data();
    void TestTimeRecordWrite();
};
#endif // ANALYZER_H
