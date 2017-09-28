#ifndef ANALYZER_H

#define ANALYZER_H

#include <QtCore>
#include <QtTest/QTest>

#include <TimeSeriesDBI.h>

//function of compare
inline bool fuzzyCompare(const double d1, const double d2)
{
    return qAbs(d1 - d2) <= 0.00001;
}

//define the types
typedef QString AnalysisTag;
typedef QHash<QString,double> AnalysisResultForOne;

//reassign the insert function
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

//results class
class AnalysisResult
{
public:
    AnalysisResult();
    bool operator == (const AnalysisResult &result) const;
    bool operator != (const AnalysisResult &result) const;
    QString operator << (const AnalysisResult &result) const;

    //get value from table_
    double value(const QString &id,const QString &tag) const;

    //create JSON string from TimeSeriesID
    QString toJSONString();

    //return AnalysisResult from json file
    AnalysisResult loadJson(const QString fileName);

    //save AnalysisResult to json file
    void saveJson(const QString fileName);

    //create AnalysisResult from list of json strings
    AnalysisResult fromJSONString(const QString listOfJsonObject);

    //set new TimeSeriesID in table_
    AnalysisResult project (const TimeSeriesID id) const;

    //insert value with id an tag in to AnalysisResult
    AnalysisResult &insert(const TimeSeriesID &id, const AnalysisTag &tag, const double result);

    //isert row of value to AnalysisResult
    AnalysisResult &insertRow(const TimeSeriesID &id, const QHash<AnalysisTag, double> row);

    //unite 2 AnalysisResult
    AnalysisResult &unite(const AnalysisResult &result);

    //return ids of AnalysisResult
    QList<AnalysisTag> tags() const;

    //return tags of AnalysisResult
    QList<AnalysisTag> tagsInside(const QString id) const;

    QString StrAll () const;

    //return table of AnalysisResult
    QHash<TimeSeriesID, QHash<AnalysisTag, double> > getTable();

    private:
    //main object
    QHash<TimeSeriesID, QHash<AnalysisTag, double> >  table_;
};
Q_DECLARE_METATYPE(AnalysisResult)

//analiser class
class Analyzer
{
public:
    //create Analyzer with tag
    Analyzer(const AnalysisTag &tag);

    virtual ~Analyzer();


    virtual AnalysisResultForOne analyze(const TimeSeries &timeSeries) = 0;
    //return tags
    AnalysisTag tag() const;

    //return value of average for timeSeries
    static double avg(const TimeSeries &timeSeries);

    //return value of deviation for timeSeries
    static double dev(const TimeSeries &timeSeries);

    //return value of variation for timeSeries
    static double var(const TimeSeries &timeSeries);

private:
    Analyzer();

    //main object in class
    AnalysisTag tag_;
};
Q_DECLARE_METATYPE(Analyzer*)



//class average
class AvgAnalyzer : public Analyzer
{
public:
    AvgAnalyzer();
    virtual ~AvgAnalyzer();

    //return value of avg function with tag
    virtual AnalysisResultForOne analyze(const TimeSeries &timeSeries);
};
Q_DECLARE_METATYPE(AvgAnalyzer*)


//class variation
class VarCoefAnalyzer :public Analyzer
{
public:
    VarCoefAnalyzer();

    virtual ~VarCoefAnalyzer();
    //return value of var function with tag
    virtual AnalysisResultForOne analyze(const TimeSeries &timeSeries);
};
Q_DECLARE_METATYPE(VarCoefAnalyzer*)


//class deviation
class DevAnalyzer : public Analyzer
{
public:
    DevAnalyzer();

    virtual ~DevAnalyzer();

    //return value of var function with tag
    virtual AnalysisResultForOne analyze(const TimeSeries &timeSeries);
};
Q_DECLARE_METATYPE(DevAnalyzer*)


//class complex for all analises
class ComplexAnalyzer : public Analyzer
{
public:
    ComplexAnalyzer(const QList<Analyzer*> analyzers);

    //virtual destructor
    virtual ~ComplexAnalyzer();

    //return values of functions with tags
    virtual AnalysisResultForOne analyze(const TimeSeries &timeSeries);

    //analise for 1 id (name of TimeSeries)
    AnalysisResultForOne analyzeForID(const TimeSeriesID &id, const TimeSeries list);

    AnalysisResult analyzeForIDs(TimeSeriesDBI *dbi, const QList<TimeSeriesID> &ids);

    AnalysisResult analyzeForIDsStream(TimeSeriesDBI *dbi, const QList<TimeSeriesID> &ids);

private:
    QList<Analyzer*> analyzers_;
};
Q_DECLARE_METATYPE(ComplexAnalyzer*)
Q_DECLARE_METATYPE(QSharedPointer<ComplexAnalyzer>)

#endif // ANALYZER_H
