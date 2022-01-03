#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QWidget>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QMenu>
#include <QActionGroup>
#include <QAction>

namespace Ui {
class playlist;
}

class playlist : public QWidget
{
    Q_OBJECT

public:
    explicit playlist(QWidget *parent = nullptr, QString user = "", QList<QString>* = nullptr, QList<int>* = nullptr);
    void update(int);
    ~playlist();

private slots:
    void on_songsTable_clicked(const QModelIndex &index);
    void on_addButton_clicked();
    void on_searchButton_clicked();
    void on_searchTable_doubleClicked(const QModelIndex &index);
    void on_songsTable_customContextMenuRequested(const QPoint &pos);
    void addToQue();
    void goToArtist();
    void goToAlbum();
    void getSongToToggleLike();
    void addSongToPlaylist(QAction*);
    void removeSongFromPlaylist();

signals:
    void play(int);
    void showArtist(int, QString);
    void showAlbum(int, QString);
    void addToQ(int, QString);
    void likeUnlike(int);

private:
    QString user;
    QSqlQueryModel* songs;
    int ID;
    Ui::playlist *ui;
    QList<QString>* playList;
    QList<int>* playList_IDs;
    QSqlQueryModel* searchedSongs;
    QSqlQueryModel* usr_plylsts;
    QAction* removeSong;
    QAction* addSongToQ;
    QAction* goToSongArtist;
    QAction* goToSongAlbum;
    QAction* goToAlbumArtist;
    QAction* like;
    QActionGroup* addToPlylst;
    QMenu* plSubmenu;
    int clickedRow;
    void setupAddtoPlaylist();
    void setupActions();
};

#endif // PLAYLIST_H
