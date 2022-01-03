#ifndef PLAYLISTSVIEW_H
#define PLAYLISTSVIEW_H

#include <QWidget>
#include <QSql>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include "createplaylistdialogue.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QAction>
#include <QMenu>

namespace Ui {
class Playlistsview;
}

class Playlistsview : public QWidget
{
    Q_OBJECT

public:
    explicit Playlistsview(QWidget *parent = nullptr, QString uname = "");
    void update();
    ~Playlistsview();

private slots:
    void create(QString name, QString descript);
    void on_create_clicked();
    void on_playlistsTable_clicked(const QModelIndex &index);
    void on_playlistsTable_customContextMenuRequested(const QPoint &pos);
    void deletePlaylist();

signals:
    void showPlaylist(int);
    void playlistChange();

private:
    Ui::Playlistsview *ui;
    QSqlQueryModel* playlists;
    createPlaylistDialogue* createplistdialogue;
    QString uname;
    QAction* delPlaylistAct;
    int clickedRow;
};

#endif // PLAYLISTSVIEW_H
