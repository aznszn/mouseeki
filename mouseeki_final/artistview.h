#ifndef ARTISTVIEW_H
#define ARTISTVIEW_H

#include <QWidget>
#include <QStackedWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QTableView>
#include <QAbstractTableModel>
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QString>
#include <QSqlRecord>
#include <QFont>
#include <QFontDatabase>


namespace Ui {
class ArtistView;
}

class ArtistView : public QWidget
{
    Q_OBJECT

public:
    explicit ArtistView(QWidget *parent = nullptr);
    void update(int ID, QString name);
    ~ArtistView();

private slots:
    void on_albums_clicked(const QModelIndex &index);

signals:
    void showAlbum(int ID, QString name);

private:
    Ui::ArtistView *ui;
    int ID;
    QString name;
    QSqlDatabase* db;
    QStackedWidget* stack;
    QSqlQueryModel* albums_table_model;
};

#endif // ARTISTVIEW_H
