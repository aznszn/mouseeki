#ifndef HOME_H
#define HOME_H

#include <QWidget>
#include <QSql>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QFont>
#include <QFontDatabase>

namespace Ui {
class Home;
}

class Home : public QWidget
{
    Q_OBJECT

public:
    explicit Home(QWidget *parent = nullptr, QString userName = "", QList<QString>* playlist = nullptr, QList<int>* playlist_IDs = nullptr);
    void update();
    ~Home();

signals:
    void play(int);
    void showArtist(int, QString);
    void showAlbum(int, QString);
    void addToQ(int, QString);
    void likeUnlike(int);
    void showPlaylist(int);

private slots:
    void on_playlistTable_clicked(const QModelIndex &index);
    void on_likedSongsTable_customContextMenuRequested(const QPoint &pos);
    void addToQue();
    void goToArtist();
    void goToAlbum();
    void getSongToToggleLike();
    void addSongToPlaylist(QAction*);
    void on_likedSongsTable_clicked(const QModelIndex &index);

private:
    Ui::Home *ui;
    QString user;
    QSqlQueryModel* likedSongs;
    QSqlQueryModel* playlists;
    QAction* removeSong;
    QAction* addSongToQ;
    QAction* goToSongArtist;
    QAction* goToSongAlbum;
    QAction* like;
    QActionGroup* addToPlylst;
    QMenu* plSubmenu;
    int clickedRow;
    void setupAddtoPlaylist();
    void setupActions();
    QList<QString>* playList;
    QList<int>* playList_IDs;
};

#endif // HOME_H
