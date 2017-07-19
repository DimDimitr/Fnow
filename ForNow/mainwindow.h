#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QMainWindow>
#include <QJsonObject>
#include <QJsonParseError>
#include <QFile>
#include <QFileDialog>
#include <QStandardItem>
#include <QJsonArray>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QJsonDocument doc;
    QJsonArray docArr;
    QJsonParseError docError;
    QString globPath;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
