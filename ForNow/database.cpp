#include "Database.h"

DbManager::DbManager(const QString& path)
{
//    m_db = QSqlDatabase::addDatabase("QSQLITE");
//    m_db.setDatabaseName(path);
//    m_db.open();

//    query = QSqlQuery(m_db);

//    if (!m_db.open())
//    {
//        qWarning() << "Error: connection with database fail";
//    }
//    else
//    {

//        QSqlQuery query("CREATE TABLE point ("
//                        "point integer PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL,"
//                        "A double NOT NULL,"
//                        "B double NOT NULL,"
//                        "C double NOT NULL,"
//                        "D double NOT NULL"
//                        ")");
//        query.exec();
//        qWarning() << "Database: connection ok"<<query.lastError();
//    }
}


DbManager::DbManager()
{

}


void DbManager::setDb(const QString& path)
{
    m_db = QSqlDatabase();
    query = QSqlQuery();
    QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);
    m_db.open();

    query = QSqlQuery(m_db);

    if (!m_db.open())
    {
        qWarning() << "Error: connection with database fail";
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
        qWarning() << "Database: connection ok"<<query.lastError();
    }
}

//Insert point in DB
void DbManager::insert_to_db(int point,double A,double B,double C,double D)
{

    QSqlQuery query(m_db);
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

QList<double> DbManager::rownumbers(QString tag)
{
    QList<double> list;
    if (!m_db.open())
    {
        qWarning() << "Error: connection with database fail";
    }
    else
    {
        qWarning() << query.lastError().text()<<"I am 1 bug";
        if (tag == "A")
        {
            query.prepare("SELECT A FROM point");
        }
        else  if (tag == "B")
        {
            query.prepare("SELECT B FROM point");
        }
        else  if (tag == "C")
        {
            query.prepare("SELECT C FROM point");
        }
        else
        {
            query.prepare("SELECT D FROM point");
        }
        if(!query.exec())
        {
            qWarning() << query.lastError().text()<<"I am 2 bug";
        }

        while (query.next())
        {
            list << query.value(0).toDouble();
        }
    }
    return list;
}

//Print DB in debug
void DbManager::print_all(){
    QSqlQuery query("SELECT * FROM point");
    int idName = query.record().indexOf("point");
    while (query.next())
    {
        QString name = query.value(idName).toString();
        qWarning() << name;
    }
}
void DbManager::close_db()
{
    query = QSqlQuery();
    m_db = QSqlDatabase();
    QSqlDatabase::removeDatabase("1.db");
}
