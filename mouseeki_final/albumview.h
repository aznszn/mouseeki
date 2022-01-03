#ifndef ALBUMVIEW_H
#define ALBUMVIEW_H

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
#include <QMediaPlayer>
#include <QList>
#include <QMenu>
#include <QAction>
#include <QActionEvent>
#include <QFont>
#include <QFontDatabase>
#include <QActionGroup>


namespace Ui {
class AlbumView;
}

class AlbumView : public QWidget
{
    Q_OBJECT

public:
    explicit AlbumView(QWidget *parent = nullptr, QList<QString>* = nullptr, QList<int>* = nullptr, QString user = "");
    void update(int ID, QString name);
    ~AlbumView();

private slots:
    void on_songs_clicked(const QModelIndex &index);
    void on_songs_customContextMenuRequested(const QPoint &pos);
    void addToQue();
    void goToArtist();
    void getSongToToggleLike();
    void addSongToPlaylist(QAction*);


signals:
    void playAlbum(int);
    void showArtist(int, QString);
    void addToQ(int, QString);
    void likeUnlike(int);

private:
    Ui::AlbumView *ui;
    int ID;
    QString name;
    QSqlQueryModel* songs_table_model;
    QList<QString>* playList;
    QList<int>* playList_IDs;
    QAction* goToAlbumArtist;
    QAction* addSongToQ;
    QAction* goToSongArtist;
    QAction* like;
    QActionGroup* addToPlylst;
    QSqlQueryModel* usr_plylsts;
    QMenu* plSubmenu;
    QString user;
    int clickedRow;
    void setupAddtoPlaylist();
    void setupActions();
};

#endif // ALBUMVIEW_H
