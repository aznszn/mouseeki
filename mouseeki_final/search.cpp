#include "search.h"
#include "ui_search.h"

Search::Search(QWidget *parent, QList<QString>* playList, QList<int>* playList_IDs, QString user) :
    QWidget(parent),
    ui(new Ui::Search),
    playList(playList),
    playList_IDs(playList_IDs),
    user(user)
{
    ui->setupUi(this);
    searchedSongs = new QSqlQueryModel;
    ui->searchedSongsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->searchedSongsTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->searchedSongsTable->setSelectionBehavior(QTableView::SelectRows);
    ui->searchedSongsTable->setModel(searchedSongs);
    ui->searchedSongsTable->verticalHeader()->hide();
    ui->searchedSongsTable->setFrameStyle(QFrame::NoFrame);
    ui->searchedSongsTable->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->searchedSongsTable->setFocusPolicy(Qt::NoFocus);
    ui->searchedSongsTable->hide();

    searchedAlbums = new QSqlQueryModel;
    ui->searchedAlbumsTable->verticalHeader()->hide();
    ui->searchedAlbumsTable->setSelectionBehavior(QTableView::SelectRows);
    ui->searchedAlbumsTable->setFrameStyle(QFrame::NoFrame);
    ui->searchedAlbumsTable->setModel(searchedAlbums);
    ui->searchedAlbumsTable->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->searchedAlbumsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->searchedAlbumsTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //ui->searchedAlbumsTable->setSelectionMode(QTableView::NoSelection)azn;
    ui->searchedAlbumsTable->setFocusPolicy(Qt::NoFocus);
    ui->searchedAlbumsTable->hide();

    searchedArtists = new QSqlQueryModel;
    ui->searchedArtistsTable->setFrameStyle(QFrame::NoFrame);
    ui->searchedArtistsTable->setSelectionBehavior(QTableView::SelectRows);
    ui->searchedArtistsTable->verticalHeader()->hide();
    ui->searchedArtistsTable->setModel(searchedArtists);
    ui->searchedArtistsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->searchedArtistsTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //ui->searchedArtistsTable->setSelectionMode(QTableView::NoSelection);
    //ui->searchedArtistsTable->setSelectionModel(QTableView::ro);
    ui->searchedArtistsTable->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->searchedArtistsTable->setFocusPolicy(Qt::NoFocus);
    ui->searchedArtistsTable->hide();

    usr_plylsts = new QSqlQueryModel;
    plSubmenu = new QMenu;
    plSubmenu->setTitle("Add to playlist");
    addToPlylst = new QActionGroup(plSubmenu);
    connect(addToPlylst, SIGNAL(triggered(QAction*)), this, SLOT(addSongToPlaylist(QAction*)));
    addToPlylst->setExclusive(false);
    setupActions();
}

void Search::setupAddtoPlaylist(){
    QSqlQuery q;
    q.prepare("SELECT playlist_id, playlist_name FROM playlist WHERE user_id = ?");
    q.bindValue(0, user);
    q.exec();
    usr_plylsts->setQuery(std::move(q));

    for(auto& x : addToPlylst->actions()){
        addToPlylst->removeAction(x);
    }
    plSubmenu->clear();

    for(int i = 0; i < usr_plylsts->rowCount(); ++i){
            addToPlylst->addAction(plSubmenu->addAction(tr(usr_plylsts->record(i).value(1).toString().toLocal8Bit().data())))->setData(i);
    }
}

void Search::addSongToPlaylist(QAction* act){
    int p_id = usr_plylsts->record(act->data().toInt()).value(0).toInt();
    QSqlQuery q;
    q.prepare("INSERT INTO playlist_song VALUES (?, ?)");
    q.bindValue(0, p_id);
    q.bindValue(1, searchedSongs->record(clickedRow).value(1).toInt());
    q.exec();
}

void Search::on_pushButton_clicked()
{
    QString toSearch = ui->search->text();
    if(toSearch.isEmpty())
        return;

    QSqlQuery q;
    q.prepare("SELECT url, id, albid, artid, name, albname, albyear, artname FROM song_info WHERE name LIKE ?");
    q.bindValue(0, "%" + toSearch + "%");
    q.exec();
    searchedSongs->setQuery(std::move(q));

    QSqlQuery q1;
    q1.prepare("SELECT alb.album_id, art.artist_id, alb.album_name, alb.album_year, art.artist_name FROM album alb JOIN artist art ON alb.artist_id = art.artist_id WHERE alb.album_name LIKE ?");
    q1.bindValue(0, "%" + toSearch + "%");
    q1.exec();
    searchedAlbums->setQuery(std::move(q1));

    QSqlQuery q2;
    q2.prepare("SELECT artist_id, artist_name FROM artist WHERE artist_name LIKE ?");
    q2.bindValue(0, "%" + toSearch + "%");
    q2.exec();
    searchedArtists->setQuery(std::move(q2));

    searchedSongs->setHeaderData(4, Qt::Horizontal, tr("Song"));
    searchedSongs->setHeaderData(5, Qt::Horizontal, tr("Album"));
    searchedSongs->setHeaderData(6, Qt::Horizontal, tr("Year"));
    searchedSongs->setHeaderData(7, Qt::Horizontal, tr("Artist"));
    ui->searchedSongsTable->setColumnHidden(0, true);
    ui->searchedSongsTable->setColumnHidden(1, true);
    ui->searchedSongsTable->setColumnHidden(2, true);
    ui->searchedSongsTable->setColumnHidden(3, true);

    searchedAlbums->setHeaderData(2,Qt::Horizontal, tr("Album"));
    searchedAlbums->setHeaderData(3,Qt::Horizontal, tr("Year"));
    searchedAlbums->setHeaderData(4,Qt::Horizontal, tr("Artist"));
    ui->searchedAlbumsTable->setColumnHidden(0, true);
    ui->searchedAlbumsTable->setColumnHidden(1, true);

    searchedArtists->setHeaderData(1, Qt::Horizontal, tr("Artist"));
    ui->searchedArtistsTable->setColumnHidden(0, true);

    if(searchedSongs->rowCount() > 0)
        ui->searchedSongsTable->show();
    else
        ui->searchedSongsTable->hide();

    if(searchedArtists->rowCount() > 0)
        ui->searchedArtistsTable->show();
    else
        ui->searchedArtistsTable->hide();

    if(searchedAlbums->rowCount() > 0)
        ui->searchedAlbumsTable->show();
    else
        ui->searchedAlbumsTable->hide();
}


void Search::on_searchedSongsTable_clicked(const QModelIndex &index)
{
    playList->clear();
    playList_IDs->clear();
    playList->push_back(searchedSongs->record(index.row()).value(0).toString());
    playList_IDs->push_back(searchedSongs->record(index.row()).value(1).toInt());
    emit playSong_(0);
}


void Search::on_searchedArtistsTable_clicked(const QModelIndex &index){
    ui->search->clear();
    ui->searchedArtistsTable->clearSelection();
    int ID = searchedArtists->record(index.row()).value(0).toInt();
    QString name = searchedArtists->record(index.row()).value(1).toString();
    emit showArtist(ID, name);
}


void Search::on_searchedAlbumsTable_clicked(const QModelIndex &index){
    ui->search->clear();
    ui->searchedAlbumsTable->clearSelection();
    int ID = searchedAlbums->record(index.row()).value(0).toInt();
    QString name = searchedAlbums->record(index.row()).value(2).toString();
    emit showAlbum(ID, name);
}


void Search::on_searchedSongsTable_customContextMenuRequested(const QPoint &pos)
{
    QMenu menu (this);
    setupAddtoPlaylist();
    QSqlQuery q;
    q.prepare("SELECT * FROM likes WHERE user_id = ? AND song_id = ?");
    clickedRow = ui->searchedSongsTable->indexAt(pos).row();
    q.bindValue(0, user);
    q.bindValue(1, searchedSongs->record(clickedRow).value(1).toInt());
    q.exec();
    if(q.next())
        like->setText("&Unlike");
    else
        like->setText("&Like");
    menu.addAction(like);
    menu.addAction(addSongToQ);
    menu.addAction(goToSongAlbum);
    menu.addAction(goToSongArtist);
    menu.addMenu(plSubmenu);
    menu.exec(ui->searchedSongsTable->viewport()->mapToGlobal(pos));

}

void Search::on_searchedAlbumsTable_customContextMenuRequested(const QPoint &pos)
{
    QMenu menu(this);
    clickedRow = ui->searchedAlbumsTable->indexAt(pos).row();
    menu.addAction(goToAlbumArtist);
    menu.exec(ui->searchedAlbumsTable->viewport()->mapToGlobal(pos));
}

void Search::setupActions(){
    addSongToQ = new QAction(tr("&Add to queue"),this);
    goToSongAlbum = new QAction(tr("&Go to album"), this);
    goToSongArtist = new QAction(tr("&Go to artist"), this);
    goToAlbumArtist = new QAction(tr("&Go to artist"), this);
    like = new QAction(tr("&Like song"), this);

    connect(like, &QAction::triggered, this, &Search::getSongToToggleLike);
    connect(addSongToQ, &QAction::triggered, this, &Search::addToQue);
    connect(goToSongAlbum, &QAction::triggered, this, &Search::goToAlbum);
    connect(goToSongArtist, &QAction::triggered, this, &Search::goToArtist);
    connect(goToAlbumArtist, &QAction::triggered, this, &Search::goToArtistFromAlbum);
}

void Search::getSongToToggleLike(){
    emit likeUnlike(searchedSongs->record(clickedRow).value(1).toInt());
}

void Search::goToArtistFromAlbum(){
    int ID = searchedAlbums->record(clickedRow).value(1).toInt();
    QString name = searchedAlbums->record(clickedRow).value(4).toString();
    emit showArtist(ID, name);
}

void Search::goToArtist(){
    int ID = searchedSongs->record(clickedRow).value(3).toInt();
    QString name = searchedSongs->record(clickedRow).value(7).toString();
    emit showArtist(ID, name);
}

void Search::goToAlbum(){
    int ID = searchedSongs->record(clickedRow).value(2).toInt();
    QString name = searchedSongs->record(clickedRow).value(5).toString();
    emit showAlbum(ID, name);
}

void Search::addToQue(){
    if(!playList->empty()){
        QString url = searchedSongs->record(clickedRow).value(0).toString();
        int ID = searchedSongs->record(clickedRow).value(1).toInt();
        emit addToQ(ID, url);
    }
}

Search::~Search()
{
    delete ui;
}
