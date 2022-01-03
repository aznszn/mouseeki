#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QtDebug>
#include <QStringList>
#include <QStringListModel>
#include <QListView>
#include <QTableView>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQueryModel>
#include <QAbstractItemView>
#include <QAbstractItemModel>
#include <QStackedLayout>
#include <QStackedLayout>
#include "albumview.h"
#include "artistview.h"
#include "home.h"
#include "search.h"
#include <QObject>
#include <QtMultimedia>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>
#include <QList>
#include <QPixmap>
#include <QTime>
#include "playlistsview.h"
#include "playlist.h"
#include <QFile>
#include <QFrame>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QSize>
#include <QAudioFormat>
#include <QFont>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, QString uname = "");
    QString user;
    ~MainWindow();

private slots:
    void moveToArtist(int ID,QString name);
    void moveToAlbum(int ID, QString name);
    void moveToPlaylist(int ID);
    void on_leftPanel_clicked(const QModelIndex &index);
    void playSong(QString url);
    void on_playButton_clicked();
    void playPlayList(int);
    void on_nextButton_clicked();
    void on_prevButton_clicked();
    void on_loopButton_clicked();
    void song_ended(QMediaPlayer::MediaStatus);
    void addToQ(int, QString);
    void on_likeButton_clicked();
    void toggleLike(int ID);
    void logout();
    void on_unameText_clicked();
    void on_prevPageButton_clicked();
    void on_nextPageButton_clicked();
    void updateHomeView();

signals:
    void loggedOut();

private:
    void init_mainWindow();
    void setupButtons();
    Ui::MainWindow *ui;
    void updateNowPlaying();
    QStringList* leftPanelItems;
    QStringListModel* qslm;
    //QStackedLayout* center;
    AlbumView* albumView;
    ArtistView* artistView;
    Home* homeView;
    Search* searchView;
    Playlistsview* plview;
    playlist* pl;
    QSqlDatabase db;
    QMediaPlayer* player;
    QAudioOutput* audioOutput;
    QList<QString>* playList;
    QList<int>* playList_IDs;
    QList<int> pages;
    int currPage;
    int current;
    bool isPlaying;
    bool shuffle = false;
    bool loop = false;
    bool loop1 = false;
    QIcon* loopIcon;
    QIcon* loop1Icon;
    QIcon* noLoopIcon;
    QAction* logoutAct;
};
#endif // MAINWINDOW_H
