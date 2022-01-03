#include "playlist.h"
#include "ui_playlist.h"

playlist::playlist(QWidget *parent, QString user, QList<QString>* playList, QList<int>* playList_IDs) :
    QWidget(parent),
    user(user),
    ui(new Ui::playlist),
    playList(playList),
    playList_IDs(playList_IDs)
{
    ui->setupUi(this);
    songs = new QSqlQueryModel;
    ui->songsTable->setModel(songs);
    ui->songsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->songsTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->songsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->songsTable->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->songsTable->setFocusPolicy(Qt::NoFocus);
    ui->songsTable->verticalHeader()->hide();
    ui->songsTable->setFrameStyle(QFrame::NoFrame);
    ui->songsTable->setContextMenuPolicy(Qt::CustomContextMenu);

    searchedSongs = new QSqlQueryModel;
    ui->searchTable->setModel(searchedSongs);
    ui->searchTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->searchTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->searchTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->searchTable->setFocusPolicy(Qt::NoFocus);
    ui->searchTable->setFocusPolicy(Qt::NoFocus);
    ui->searchTable->verticalHeader()->hide();
    ui->searchTable->setFrameStyle(QFrame::NoFrame);
    ui->searchTable->setContextMenuPolicy(Qt::CustomContextMenu);


    ui->addButton->setCheckable(true);

    ui->search->hide();
    ui->searchButton->hide();
    ui->searchTable->hide();
}

void playlist::update(int ID){
    this->ID = ID;
    QSqlQuery q;
    q.prepare("SELECT url, id, albid, artid, name, albname, albyear, artname FROM song_info WHERE id IN (SELECT song_id from playlist_song WHERE playlist_id = ?)");
    q.bindValue(0, ID);
    q.exec();
    songs->setQuery(std::move(q));
    QSqlQuery q1;
    q1.prepare("SELECT playlist_name, description FROM playlist WHERE playlist_id = ?");
    q1.bindValue(0, ID);
    q1.exec();
    while(q1.next()){
        ui->name->setText(q1.value(0).toString());
        ui->descript->setText(q1.value(1).toString());
    }

    songs->setHeaderData(4, Qt::Horizontal, tr("Song"));
    songs->setHeaderData(5, Qt::Horizontal, tr("Album"));
    songs->setHeaderData(6, Qt::Horizontal, tr("year"));
    songs->setHeaderData(7, Qt::Horizontal, tr("Artist"));


    ui->songsTable->setColumnHidden(0, true);
    ui->songsTable->setColumnHidden(1, true);
    ui->songsTable->setColumnHidden(2, true);
    ui->songsTable->setColumnHidden(3, true);

    usr_plylsts = new QSqlQueryModel;
    plSubmenu = new QMenu;
    plSubmenu->setTitle("Add to playlist");
    addToPlylst = new QActionGroup(plSubmenu);
    connect(addToPlylst, SIGNAL(triggered(QAction*)), this, SLOT(addSongToPlaylist(QAction*)));
    addToPlylst->setExclusive(false);
    setupActions();
}



playlist::~playlist()
{
    delete ui;
}

void playlist::on_songsTable_clicked(const QModelIndex &index)
{
    playList->clear();
    playList_IDs->clear();
    for(int i = 0; i < songs->rowCount(); ++i){
        playList->push_back(songs->record(i).value(0).toString());
        playList_IDs->push_back(songs->record(i).value(1).toInt());
    }

    emit play(index.row());
}


void playlist::on_addButton_clicked()
{
    if(ui->addButton->isChecked()){
        ui->addButton->setText("Hide");
        ui->search->show();
        ui->searchButton->show();
        ui->searchTable->show();
    }

    else{
        ui->addButton->setText("Add to playlist");
        ui->search->hide();
        ui->searchButton->hide();
        ui->searchTable->hide();
    }

}


void playlist::on_searchButton_clicked()
{
    searchedSongs->setQuery("SELECT id, name, albname, albyear, artname FROM song_info WHERE name LIKE \'%"+ ui->search->text() +"%\'");

    searchedSongs->setHeaderData(1, Qt::Horizontal, tr("Song"));
    searchedSongs->setHeaderData(2, Qt::Horizontal, tr("Album"));
    searchedSongs->setHeaderData(3, Qt::Horizontal, tr("Year"));
    searchedSongs->setHeaderData(4, Qt::Horizontal, tr("Artist"));
    ui->searchTable->setColumnHidden(0, true);
}


void playlist::on_searchTable_doubleClicked(const QModelIndex &index){
    ui->searchTable->clearSelection();
    QSqlQuery q;
    q.prepare("INSERT INTO playlist_song VALUES(?, ?);");
    q.bindValue(0, ID);
    q.bindValue(1, searchedSongs->record(index.row()).value(0).toInt());
    q.exec();
    update(ID);
}

void playlist::addSongToPlaylist(QAction* act){
    int p_id = usr_plylsts->record(act->data().toInt()).value(0).toInt();
    QSqlQuery q;
    q.prepare("INSERT INTO playlist_song VALUES (?, ?)");
    q.bindValue(0, p_id);
    q.bindValue(1, songs->record(clickedRow).value(1).toInt());
    q.exec();
}

void playlist::setupAddtoPlaylist(){
    usr_plylsts->setQuery("SELECT playlist_id, playlist_name FROM playlist WHERE user_id = \'" + user + "\'");

    for(auto& x : addToPlylst->actions()){
        addToPlylst->removeAction(x);
    }

    plSubmenu->clear();

    for(int i = 0; i < usr_plylsts->rowCount(); ++i){
            if(usr_plylsts->record(i).value(0).toInt() != ID)
                addToPlylst->addAction(plSubmenu->addAction(tr(usr_plylsts->record(i).value(1).toString().toLocal8Bit().data())))->setData(i);
    }
}

void playlist::setupActions(){
    removeSong = new QAction(tr("&Remove song from playlist"));
    addSongToQ = new QAction(tr("&Add to queue"),this);
    goToSongAlbum = new QAction(tr("&Go to album"), this);
    goToSongArtist = new QAction(tr("&Go to artist"), this);
    goToAlbumArtist = new QAction(tr("&Go to artist"), this);
    like = new QAction(tr("&Like song"), this);

    connect(removeSong, &QAction::triggered, this, &playlist::removeSongFromPlaylist);
    connect(like, &QAction::triggered, this, &playlist::getSongToToggleLike);
    connect(addSongToQ, &QAction::triggered, this, &playlist::addToQue);
    connect(goToSongAlbum, &QAction::triggered, this, &playlist::goToAlbum);
    connect(goToSongArtist, &QAction::triggered, this, &playlist::goToArtist);
}

void playlist::removeSongFromPlaylist(){
    QSqlQuery q;
    q.prepare("DELETE FROM playlist_song WHERE playlist_id = ? AND song_id = ?");
    q.bindValue(0, ID);
    q.bindValue(1, songs->record(clickedRow).value(1).toInt());
    q.exec();
    update(ID);
}

void playlist::getSongToToggleLike(){
    emit likeUnlike(songs->record(clickedRow).value(1).toInt());
}

void playlist::on_songsTable_customContextMenuRequested(const QPoint &pos)
{
    QMenu menu (this);
    setupAddtoPlaylist();
    QSqlQuery q;
    q.prepare("SELECT * FROM likes WHERE user_id = ? AND song_id = ?");
    clickedRow = ui->songsTable->indexAt(pos).row();
    q.bindValue(0, user);
    q.bindValue(1, songs->record(clickedRow).value(1).toInt());
    q.exec();
    if(q.next())
        like->setText("&Unlike");
    else
        like->setText("&Like");

    menu.addAction(like);
    menu.addAction(removeSong);
    menu.addAction(addSongToQ);
    menu.addAction(goToSongAlbum);
    menu.addAction(goToSongArtist);
    menu.addMenu(plSubmenu);
    menu.exec(ui->songsTable->viewport()->mapToGlobal(pos));
}


void playlist::goToArtist(){
    ui->songsTable->clearSelection();
    int ID = songs->record(clickedRow).value(3).toInt();
    QString name = songs->record(clickedRow).value(7).toString();
    emit showArtist(ID, name);
}

void playlist::goToAlbum(){
    ui->songsTable->clearSelection();
    int ID = songs->record(clickedRow).value(2).toInt();
    QString name = songs->record(clickedRow).value(5).toString();
    emit showAlbum(ID, name);
}

void playlist::addToQue(){
    if(!playList->empty()){
        QString url = songs->record(clickedRow).value(0).toString();
        int ID = songs->record(clickedRow).value(1).toInt();
        emit addToQ(ID, url);
    }
}

