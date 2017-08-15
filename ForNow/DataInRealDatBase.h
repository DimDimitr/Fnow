#ifndef DATAINREALDATBASE_H
#define DATAINREALDATBASE_H

#include <QtSql>
#include <QMessageBox>
#include <QMainWindow>

class DbManager
{
public:
    //QSqlQuery query;
    DbManager(const QString& path);
    DbManager();
    void setDb(const QString& path);    void choose ();
    void setTable ();
    void create_table();
    void insert_to_db(int point,double A,double B,double C,double D);
    QList<double> rownumbers(QString tag);
    void print_all();
    void close_db();
private:
    QSqlDatabase m_db;
};


class TDbManager : public QObject
{
    Q_OBJECT
private slots:
    void TestReadFromSql_data();
    void TestReadFromSql();
};

#endif // DATAINREALDATBASE_H
