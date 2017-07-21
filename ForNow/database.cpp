#include "database.h"
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
                       "D double NOT NULL,"
                       ");");
      qDebug() << "Database: connection ok";
   }
}



/*void DbManager::Vibor ()
{
    QSqlQuery query;
    while (query.next()) {
         QString name = query.value(0).toString();
         int salary = query.value(1).toInt();
         qDebug() << name << salary;
       }
}

void DbManager::setTable ()
{
       QSqlTableModel model;
       model.setTable("employee");
       QTableView *view = new QTableView;
       view->setModel(&model);
       view->show();
    }
*/

int DbManager::Insert_to_DB(int point,double A,double B,double C,double D)
{

    QSqlQuery query;
    query.prepare("INSERT INTO employee (point, A, B,C,D) "
             "VALUES (:point, :A, :B, :C, :D)");
    query.bindValue(":point", point);
    query.bindValue(":A", A);
    query.bindValue(":B", B);
    query.bindValue(":C", C);
    query.bindValue(":D", D);
    query.exec();
    int rer = query.lastInsertId().toInt();
    return rer;
}

void DbManager::PrintAll(){
QSqlQuery query("SELECT * FROM point");
int idName = query.record().indexOf("point");
while (query.next())
{
   QString name = query.value(idName).toString();
   qDebug() << name;
}
}


