#include "Database.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

DbManager::DbManager(const QString& path)
{
   m_db = QSqlDatabase::addDatabase("QSQLITE");
   m_db.setDatabaseName(path);
   m_db.open();

   if (!m_db.open())
   {
      qDebug() << "Error: connection with database fail";
   }
   else
   {

       QSqlQuery query("CREATE TABLE point ("
                       "point integer PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL,"
                       "A double NOT NULL,"
                       "B double NOT NULL,"
                       "C double NOT NULL,"
                       "D double NOT NULL"
                       ")");
       query.exec();
      qDebug() << "Database: connection ok"<<query.lastError();
   }
}

//Insert point in DB
void DbManager::insert_to_db(int point,double A,double B,double C,double D)
{

    QSqlQuery query;
    query.prepare("INSERT INTO point (point, A, B,C,D) "
             "VALUES (:point, :A, :B, :C, :D)");
    query.bindValue(":point", point);
    query.bindValue(":A", A);
    query.bindValue(":B", B);
    query.bindValue(":C", C);
    query.bindValue(":D", D);
    query.exec();
    //query.next();
}

//Print DB in debug
void DbManager::print_all(){
QSqlQuery query("SELECT * FROM point");
int idName = query.record().indexOf("point");
while (query.next())
{
   QString name = query.value(idName).toString();
   qDebug() << name;
}
}


