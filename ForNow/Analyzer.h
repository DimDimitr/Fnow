#ifndef ANALYZER_H

#define ANALYZER_H

#include <QtCore>
#include <QtTest/QTest>

inline bool fuzzyCompare(const double d1, const double d2)
{
    return qAbs(d1 - d2) <= 0.00001;
}

class TimeSeries : public QList<double>
{
public:
    TimeSeries()
    {

    }

    TimeSeries(const QString &id) :
        id_(id)
    {

    }

    QString id() const
    {
        return id_;
    }

    TimeSeries& operator <<(const double elem)
    {
        append(elem);
        return *this;
    }

    bool operator ==(const TimeSeries &series) const
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

    QString toString() const
    {
        QStringList cells;
        foreach(const double elem, *this)
        {
            cells << QString::number(elem);
        }

        return "TimeSeries{id: " + id_ + ", data: " + cells.join(", ") + "}";
    }

private:
    QString id_;
};
Q_DECLARE_METATYPE(TimeSeries)

class AnalysisResult
{
public:
    AnalysisResult()
    {
    }

    double value(const QString &tag) const
    {
        return table_.value(tag, 0.0);
    }

    bool operator == (const AnalysisResult &result) const
    {
        foreach(const QString &tag, (table_.keys() + result.table_.keys()).toSet())
        {
            if(!table_.contains(tag) || !result.table_.contains(tag))
            {
                return false;
            }
            if(!fuzzyCompare(value(tag), result.value(tag)))
            {
                return false;
            }
        }
        return true;
    }

    AnalysisResult &insert(const QString &tag, const double result)
    {
        table_.insert(tag, result);
        return *this;
    }

    AnalysisResult &unite(const AnalysisResult &result)
    {
        table_.unite(result.table_);
        return *this;
    }

private:
    QHash<QString, double> table_;
};
Q_DECLARE_METATYPE(AnalysisResult)

class Analyzer
{
public:
    Analyzer(const QString &tag) :
        tag_(tag)
    {

    }

    virtual ~Analyzer()
    {
    }

    virtual AnalysisResult analyze(const TimeSeries &timeSeries) = 0;
    QString tag() const
    {
        return tag_;
    }

    static double avg(const TimeSeries &timeSeries);
    static double dev(const TimeSeries &timeSeries);

private:
    Analyzer()
    {
    }

    QString tag_;
};
Q_DECLARE_METATYPE(Analyzer*)

class StupidAnalyzer : public Analyzer
{
public:
    StupidAnalyzer() :
        Analyzer(QString())
    {
    }

    virtual ~StupidAnalyzer()
    {
    }

    virtual AnalysisResult analyze(const TimeSeries &timeSeries)
    {
        return AnalysisResult();
    }
};
Q_DECLARE_METATYPE(StupidAnalyzer*)

class AvgAnalyzer : public Analyzer
{
public:
    AvgAnalyzer() :
        Analyzer("#avg")
    {
    }

    virtual ~AvgAnalyzer()
    {
    }

    virtual AnalysisResult analyze(const TimeSeries &timeSeries)
    {
        return AnalysisResult().insert(tag(), avg(timeSeries));
    }
};
Q_DECLARE_METATYPE(AvgAnalyzer*)

class DevAnalyzer : public Analyzer
{
public:
    DevAnalyzer() :
        Analyzer("#dev")
    {
    }

    virtual ~DevAnalyzer()
    {
    }

    virtual AnalysisResult analyze(const TimeSeries &timeSeries)
    {
        return AnalysisResult().insert(tag(), dev(timeSeries));
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

    virtual AnalysisResult analyze(const TimeSeries &timeSeries)
    {
        AnalysisResult result;
        foreach(Analyzer *a, analyzers_)
        {
            result.unite(a->analyze(timeSeries));
        }

        return result;
    }
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

    void TestAnalyze_data();
    void TestAnalyze();
};
#endif // ANALYZER_H
