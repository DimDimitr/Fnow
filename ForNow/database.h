#ifndef DATABASE_H
#define DATABASE_H



#include <QtSql>
#include <QMessageBox>
#include <QMainWindow>

class DbManager
{
public:
    QSqlQuery query;
    DbManager(const QString& path);
    bool addPerson(const QString& name);
    void Vibor ();
    void setTable ();
    int Insert_to_DB(int point,double A,double B,double C,double D);
    void PrintAll();
    void CloseDb();
private:
    QSqlDatabase m_db;
};

#endif // DATABASE_H
