#ifndef VIEW_H
#define VIEW_H

#include <QDialog>
#include<QPushButton>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QTableView>
#include<QMessageBox>
#include<Qsql>


#include <QDebug>

#include <QByteArray>
#include <QString>

#include<QLabel>
#include <QLineEdit>


#include <QJsonObject>
#include <QJsonParseError>
#include <QFile>
#include <QFileDialog>
#include <QStandardItem>
#include <QJsonArray>


class View : public QDialog
{
    Q_OBJECT
public:
    View(QWidget *parent=0);
private:

    QPushButton *open;
    QPushButton *save;
    QPushButton *analize;
    QTableView *table;
    QTableView *table_rez;
    QLabel *name_open;
    QLineEdit *open_str;
    QLabel *name_save;
    QLineEdit *save_str;


    QJsonDocument doc;
    QJsonArray docArr;
    QJsonParseError docError;
    QString globPath;
//Все для Json




private slots:
    void AnalizeClicked();
    void OpenClicked();


};



class Make_string : public QDialog
{
    Q_OBJECT
private:
    int i;
public slots:
    void FilePathw(){
    }
    void FilePathr()
    {
        QMessageBox msg;
        QString z="Файл успешно сохранен!";
        msg.setText(z);
        msg.exec();
    }




};
#endif // VIEW_H
