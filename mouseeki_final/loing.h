#ifndef LOING_H
#define LOING_H

#include <QCryptographicHash>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QObject>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QMainWindow>
#include <regex>
#include "registerwindow.h"
#include "mainwindow.h"

namespace Ui {
class Loing;
}

class Loing : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit Loing(QWidget *parent = 0);
    ~Loing();
    
private slots:
    void on_pushButton_2_clicked();
    void on_checkBox_toggled(bool checked);
    void on_loginButton_clicked();
    void loggedOut();

private:
    Ui::Loing *ui;
    QSqlDatabase db;
    RegisterWindow *RegWin;
    MainWindow* mw;

};

#endif // LOING_H
