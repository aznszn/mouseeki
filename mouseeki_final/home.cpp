#include "home.h"
#include "ui_home.h"

Home::Home(QWidget *parent, QString userName, QList<QString>* playlist, QList<int>* playlist_IDs) :
    QWidget(parent),
    ui(new Ui::Home),
    user(userName),
    playList(playlist),
    playList_IDs(playlist_IDs)
{
    ui->setupUi(this);

    likedSongs = new QSqlQueryModel;
    ui->likedSongsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->likedSongsTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->likedSongsTable->setSelectionBehavior(QTableView::SelectRows);
    ui->likedSongsTable->setModel(likedSongs);
    ui->likedSongsTable->verticalHeader()->hide();
    ui->likedSongsTable->setFrameStyle(QFrame::NoFrame);
    ui->likedSongsTable->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->likedSongsTable->setFocusPolicy(Qt::NoFocus);

    playlists = new QSqlQueryModel;
    ui->playlistTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->playlistTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->playlistTable->setSelectionBehavior(QTableView::SelectRows);
    ui->playlistTable->setModel(playlists);
    ui->playlistTable->verticalHeader()->hide();
    ui->playlistTable->setFrameStyle(QFrame::NoFrame);
    ui->playlistTable->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->playlistTable->setFocusPolicy(Qt::NoFocus);
    update();
    plSubmenu = new QMenu;
    plSubmenu->setTitle("Add to playlist");
    addToPlylst = new QActionGroup(plSubmenu);
    connect(addToPlylst, SIGNAL(triggered(QAction*)), this, SLOT(addSongToPlaylist(QAction*)));
    addToPlylst->setExclusive(false);
    setupActions();

    QFont font(QFontDatabase::applicationFontFamilies(QFontDatabase::addApplicationFont(":/fonts/resources/fonts/GothicA1-ExtraBold.ttf")).at(0));
    font.setPointSize(30);
    ui->likedSongsLable->setFont(font);
    ui->playlistsLable->setFont(font);
}

void Home::update(){
    QSqlQuery q;
    q.prepare("SELECT url, id, albid, artid, name, albname, albyear, artname FROM song_info WHERE id IN (SELECT song_id FROM likes WHERE user_id = ?)");
    q.bindValue(0, user);
    q.exec();
    likedSongs->setQuery(std::move(q));

    likedSongs->setHeaderData(4, Qt::Horizontal, tr("Song"));
    likedSongs->setHeaderData(5, Qt::Horizontal, tr("Album"));
    likedSongs->setHeaderData(6, Qt::Horizontal, tr("Year"));
    likedSongs->setHeaderData(7, Qt::Horizontal, tr("Artist"));
    ui->likedSongsTable->setColumnHidden(0, true);
    ui->likedSongsTable->setColumnHidden(1, true);
    ui->likedSongsTable->setColumnHidden(2, true);
    ui->likedSongsTable->setColumnHidden(3, true);

    QSqlQuery q0;
    q0.prepare("SELECT playlist_id, playlist_name FROM playlist WHERE user_id = ?");
    q0.bindValue(0, user);
    q0.exec();
    playlists->setQuery(std::move(q0));
    playlists->setHeaderData(1, Qt::Horizontal, tr("Name"));
    ui->playlistTable->setColumnHidden(0, true);
}



Home::~Home()
{
    delete ui;
}

void Home::on_playlistTable_clicked(const QModelIndex &index)
{
    ui->playlistTable->clearSelection();
    emit showPlaylist(playlists->record(index.row()).value(0).toInt());
}

void Home::goToArtist(){
    ui->likedSongsTable->clearSelection();
    int ID = likedSongs->record(clickedRow).value(3).toInt();
    QString name = likedSongs->record(clickedRow).value(7).toString();
    emit showArtist(ID, name);
}

void Home::goToAlbum(){
    ui->likedSongsTable->clearSelection();
    int ID = likedSongs->record(clickedRow).value(2).toInt();
    QString name = likedSongs->record(clickedRow).value(5).toString();
    emit showAlbum(ID, name);
}

void Home::addToQue(){
    //if(!playList->empty()){
        QString url = likedSongs->record(clickedRow).value(0).toString();
        int ID = likedSongs->record(clickedRow).value(1).toInt();
        emit addToQ(ID, url);
    //}
}



void Home::on_likedSongsTable_customContextMenuRequested(const QPoint &pos)
{
    QMenu menu (this);
    setupAddtoPlaylist();
    clickedRow = ui->likedSongsTable->indexAt(pos).row();
    like->setText("&Unlike");
    menu.addAction(like);
    menu.addAction(addSongToQ);
    menu.addAction(goToSongAlbum);
    menu.addAction(goToSongArtist);
    menu.addMenu(plSubmenu);
    qDebug() << "AAAAAAAAAAAAA";
    menu.exec(ui->likedSongsTable->viewport()->mapToGlobal(pos));
}

void Home::addSongToPlaylist(QAction* act){
    int p_id = playlists->record(act->data().toInt()).value(0).toInt();
    QSqlQuery q;
    q.prepare("INSERT INTO playlist_song VALUES (?, ?)");
    q.bindValue(0, p_id);
    q.bindValue(1, likedSongs->record(clickedRow).value(1).toInt());
    q.exec();
}

void Home::setupAddtoPlaylist(){
    for(auto& x : addToPlylst->actions()){
        addToPlylst->removeAction(x);
    }
    plSubmenu->clear();

    for(int i = 0; i < playlists->rowCount(); ++i){
            addToPlylst->addAction(plSubmenu->addAction(tr(playlists->record(i).value(1).toString().toLocal8Bit().data())))->setData(i);
    }
}

void Home::setupActions(){
    addSongToQ = new QAction(tr("&Add to queue"),this);
    goToSongAlbum = new QAction(tr("&Go to album"), this);
    goToSongArtist = new QAction(tr("&Go to artist"), this);
    like = new QAction(tr("&Like song"), this);

    connect(like, &QAction::triggered, this, &Home::getSongToToggleLike);
    connect(addSongToQ, &QAction::triggered, this, &Home::addToQue);
    connect(goToSongAlbum, &QAction::triggered, this, &Home::goToAlbum);
    connect(goToSongArtist, &QAction::triggered, this, &Home::goToArtist);
}

void Home::getSongToToggleLike(){
    emit likeUnlike(likedSongs->record(clickedRow).value(1).toInt());
    update();
}

void Home::on_likedSongsTable_clicked(const QModelIndex &index)
{
    playList->clear();
    playList_IDs->clear();
    for(int i = 0; i < likedSongs->rowCount(); ++i){
        playList->push_back(likedSongs->record(i).value(0).toString());
        playList_IDs->push_back(likedSongs->record(i).value(1).toInt());
    }

    emit play(index.row());
}

