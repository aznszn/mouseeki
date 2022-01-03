#include "qkeccakhash/qkeccakhash.h"
#include "loing.h"
#include "ui_loing.h"
#include <QPixmap>



Loing::Loing(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Loing)
{
    qDebug() << "here\n";
    ui->setupUi(this);
    setWindowIcon(QIcon(":/logo/resources/logo/Mouseeki_v1.2.jpg"));
    setWindowTitle("Login");
    QPixmap pix(":/logo/mouseekiv1.2");
    ui->label_pic->setPixmap(pix.scaled(310,310,Qt::KeepAspectRatio));
    QFile file(":/stylesheet/dark");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    qApp->setStyleSheet(styleSheet);

    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setDatabaseName("mouseeki");
    db.setUserName("root");
    //pw here
    if (!db.open()) {
        QMessageBox::critical(0,"Connection error","Could not connect to the database, please check your internet connection.");
    }
}

void Loing::loggedOut(){
    ui->userEdit->clear();
    ui->passEdit->clear();
    mw->close();
    this->show();
}


Loing::~Loing()
{

}


void Loing::on_loginButton_clicked()
{
    QString passCltext = ui->passEdit->text();
    QString userName = ui->userEdit->text();
    QString md5pass = QString(QCryptographicHash::hash((passCltext.toLatin1()),QCryptographicHash::Sha512).toHex());
    if(userName==""){
        QMessageBox::critical(0, "No username provided","You left the username blank");
        return;     }
    if(passCltext==""){
    QMessageBox::critical(0, "No password provided","You left the password blank");
    return;
    }
    QSqlQuery query;
    query.prepare("SELECT `password` FROM `user` WHERE `user_id` = :user_name");
    query.bindValue(":user_name", userName);
    bool logsuf=true;
    if (!query.exec()) {
        qDebug() << query.lastError().text();
        logsuf = false;
        return;
    }
    if(logsuf==false){
        return;
    }

    query.next();
    qDebug() << md5pass << "\n";
    if(md5pass == query.value(0).toString()){
        mw = new MainWindow(this, userName);
        connect(mw, SIGNAL(loggedOut()), this, SLOT(loggedOut()));
        mw->show();
        this->hide();

    }else{
        QMessageBox::critical(this, "Invalid credentials","The credentials that you entered are incorrect");
        return;
    }
}

void Loing::on_checkBox_toggled(bool checked)
{
    int currentState = ui->checkBox->checkState();
    if (currentState==0){
        ui->passEdit->setEchoMode(QLineEdit::Password);
    }else{
        ui->passEdit->setEchoMode(QLineEdit::Normal);
    }

}

void Loing::on_pushButton_2_clicked()
{

RegWin = new RegisterWindow(this);
RegWin->setWindowTitle("Register");
RegWin->show();

}
