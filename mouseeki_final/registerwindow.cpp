#include "registerwindow.h"
#include "ui_registerwindow.h"
#include <QFile>

RegisterWindow::RegisterWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterWindow)
{
    ui->setupUi(this);
    QFile file("D:/login/LoginMySQLdb/resources/dark.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    qApp->setStyleSheet(styleSheet);
}

RegisterWindow::~RegisterWindow()
{
    delete ui;
}

void RegisterWindow::on_pushButton_clicked()
{
    QString emailEdit = ui->lineEdit_3->text();
    QString userEdit = ui->lineEdit->text();
    QString passEdit = ui->lineEdit_2->text();
    QString md5pass = QString(QCryptographicHash::hash((passEdit.toLatin1()),QCryptographicHash::Sha512).toHex());
    QVariant v,a;
    QSqlQuery query;

    if(userEdit==""){
        QMessageBox::critical(this,"No Username", "You did not enter a username");
        return;
    }
    if(passEdit==""){
        QMessageBox::critical(this,"No Password", "You did not enter a password");
        return;
    }
    if(emailEdit==""){
        QMessageBox::critical(this,"No email", "You did not enter a email");
      return;
    }

    query.prepare(QString("SELECT `user_id` FROM `user` WHERE `user_id` LIKE ?"));
    query.bindValue(0, userEdit);

    if (!query.exec()) {
        qDebug() << "here" << query.lastError().text();
        return;
    }


    if (query.next()){
            QMessageBox::critical(this,"Error","The username already exists");
            return;
    }

    query.prepare(QString("SELECT `email`  FROM `user` WHERE `email` LIKE ?"));
    query.bindValue(0, emailEdit);
    if (!query.exec()) {
        qDebug()  << query.lastError().text();
        return;
    }

    if(query.next()){
        QMessageBox::critical(this,"Error","The email already exists");
        return;
    }

    std::regex x(R"(^(([^<>()[\]\\.,;:\s@"]+(\.[^<>()[\]\\.,;:\s@"]+)*)|(".+"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$)");

    if(!std::regex_match(emailEdit.toStdString(), x)){
        QMessageBox::critical(this,"Error","The email entered is not a valid email address");
        return;
    }

    query.prepare("INSERT INTO `user` (`user_id` ,`password` ,`email` ) VALUES (?, ?, ?)");
    query.bindValue(0, userEdit);
    query.bindValue(1, md5pass);
    query.bindValue(2, emailEdit);

    if (!query.exec()) {
        qDebug() << query.lastError().text();
        QMessageBox::critical(this,"Error","Due to an error your registration could not be completed");
        return;
    }else{
        QMessageBox::information(this,"Registration sucessfull","Your registration has been sucessfull");
        this->close();
    }


}

void RegisterWindow::on_pushButton_2_clicked()
{
    close();
}
