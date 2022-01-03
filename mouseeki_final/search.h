#ifndef SEARCH_H
#define SEARCH_H

#include <QWidget>
#include <QListView>
#include <QStringListModel>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlQueryModel>
#include <QTableView>
#include <QStackedWidget>
#include <QSqlRecord>
#include "artistview.h"
#include <QObject>
#include <QMenu>
#include <QAction>
#include <QActionEvent>
#include <QActionGroup>
#include <QFrame>

namespace Ui {
class Search;
}

class Search : public QWidget
{
    Q_OBJECT

public:
    explicit Search(QWidget *parent = nullptr, QList<QString>* = nullptr, QList<int>* = nullptr, QString user = "");
    ~Search();

private slots:
    void on_pushButton_clicked();
    void on_searchedSongsTable_clicked(const QModelIndex &index);
    void on_searchedArtistsTable_clicked(const QModelIndex &index);
    void on_searchedAlbumsTable_clicked(const QModelIndex &index);
    void on_searchedSongsTable_customContextMenuRequested(const QPoint &pos);
    void on_searchedAlbumsTable_customContextMenuRequested(const QPoint &pos);
    void goToArtistFromAlbum();
    void addToQue();
    void goToArtist();
    void goToAlbum();
    void getSongToToggleLike();
    void addSongToPlaylist(QAction*);

signals:
    void playSong_(int);
    void showAlbum(int, QString);
    void showArtist(int, QString);
    void addToQ(int, QString);
    void likeUnlike(int);

private:
    Ui::Search *ui;
    QSqlQueryModel *searchedSongs;
    QSqlQueryModel *searchedAlbums;
    QSqlQueryModel *searchedArtists;
    QList<QString>* playList;
    QList<int>* playList_IDs;
    QAction* removeSong;
    QAction* goToAlbumArtist;
    QAction* addSongToQ;
    QAction* goToSongArtist;
    QAction* goToSongAlbum;
    QAction* like;
    QActionGroup* addToPlylst;
    QSqlQueryModel* usr_plylsts;
    QMenu* plSubmenu;
    QString user;
    int clickedRow;
    void setupAddtoPlaylist();
    void setupActions();
};

#endif // SEARCH_H
