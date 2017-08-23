#ifndef TTIMESERIESDBI_H

#define TTIMESERIESDBI_H
#include <QObject>

#include <TimeSeriesDBI.h>


class TTimeSeriesDBI : public QObject
{
public:
    TTimeSeriesDBI(int choose);

        Q_OBJECT
private slots:
    void TestWriteReadRewireRead_data();
    void TestWriteReadRewireRead();

private:
    QHash<QString, TimeSeriesDBI*> dbiTable_;

};

#endif // TTIMESERIESDBI_H
