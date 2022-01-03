#include "albumview.h"
#include "ui_albumview.h"

AlbumView::AlbumView(QWidget *parent, QList<QString>* playList, QList<int>* playList_IDs, QString user) :
    QWidget(parent),
    ui(new Ui::AlbumView),
    playList(playList),
    playList_IDs(playList_IDs),
    user(user)
{
    ui->setupUi(this);
    songs_table_model = new QSqlQueryModel;
    ui->songs->setModel(songs_table_model);
    ui->songs->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->songs->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->songs->setSelectionBehavior(QTableView::SelectRows);
    ui->songs->setFocusPolicy(Qt::NoFocus);
    ui->songs->verticalHeader()->hide();
    ui->songs->setFrameStyle(QFrame::NoFrame);
    ui->songs->setContextMenuPolicy(Qt::CustomContextMenu);

    QFont font(QFontDatabase::applicationFontFamilies(QFontDatabase::addApplicationFont(":/fonts/resources/fonts/GothicA1-ExtraBold.ttf")).at(0));
    font.setPointSize(40);
    ui->name->setFont(font);
    font.setPointSize(28);
    ui->artName->setFont(font);

    usr_plylsts = new QSqlQueryModel;
    plSubmenu = new QMenu;
    plSubmenu->setTitle("Add to playlist");
    addToPlylst = new QActionGroup(plSubmenu);
    connect(addToPlylst, SIGNAL(triggered(QAction*)), this, SLOT(addSongToPlaylist(QAction*)));
    addToPlylst->setExclusive(false);
    setupActions();
}

void AlbumView::setupAddtoPlaylist(){
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

void AlbumView::addSongToPlaylist(QAction* act){
    int p_id = usr_plylsts->record(act->data().toInt()).value(0).toInt();
    QSqlQuery q;
    q.prepare("INSERT INTO playlist_song VALUES (?, ?)");
    q.bindValue(0, p_id);
    q.bindValue(1, songs_table_model->record(clickedRow).value(1).toInt());
    q.exec();
}

void AlbumView::update(int ID, QString name){
    this->ID = ID;
    this->name = name;
    ui->name->setText(name);

    QSqlQuery q0;
    q0.prepare("SELECT URL, song_id, song_name, song_length FROM song WHERE album_id = ? ORDER BY track_no");
    q0.bindValue(0, ID);
    q0.exec();
    songs_table_model->setQuery(std::move(q0));
    songs_table_model->setHeaderData(2,Qt::Horizontal,tr("Name"));
    songs_table_model->setHeaderData(3,Qt::Horizontal,tr("Length"));
    ui->songs->setColumnHidden(0, true);
    ui->songs->setColumnHidden(1, true);

    QSqlQuery q;
    q.prepare("SELECT artist_name FROM artist WHERE artist_id = (SELECT artist_id FROM album WHERE album_id = ?)");
    q.bindValue(0, ID);
    q.exec();
    q.next();
    ui->artName->setText(q.value(0).toString());

    q.prepare("SELECT album_year, no_of_songs, album_length, album_genre, album_pic_url FROM album WHERE album_id = ?");
    q.bindValue(0, ID);
    q.exec();
    q.next();
    QString info = q.value(0).toString() + " • " + q.value(1).toString() + " songs • " + q.value(2).toString() + " • " + q.value(3).toString();
    ui->info->setText(info);

    QPixmap p(q.value(4).toString());
    int w = ui->albumPic->width();
    int h = ui->albumPic->height();
    ui->albumPic->setPixmap(p.scaled(w,h,Qt::KeepAspectRatio));
}

AlbumView::~AlbumView()
{
    delete ui;
}

void AlbumView::on_songs_clicked(const QModelIndex &index)
{
    playList->clear();
    playList_IDs->clear();
    for(int i = 0; i < songs_table_model->rowCount(); ++i){
        playList->push_back(songs_table_model->record(i).value(0).toString());
        playList_IDs->push_back(songs_table_model->record(i).value(1).toInt());
    }
    emit playAlbum(index.row());
}


void AlbumView::on_songs_customContextMenuRequested(const QPoint &pos)
{
    QMenu menu (this);
    setupAddtoPlaylist();
    QSqlQuery q;
    q.prepare("SELECT * FROM likes WHERE user_id = ? AND song_id = ?");
    clickedRow = ui->songs->indexAt(pos).row();
    q.bindValue(0, user);
    q.bindValue(1, songs_table_model->record(clickedRow).value(1).toInt());
    q.exec();
    if(q.next())
        like->setText("&Unlike");
    else
        like->setText("&Like");
    menu.addAction(like);
    menu.addAction(addSongToQ);
    menu.addAction(goToSongArtist);
    menu.addMenu(plSubmenu);
    menu.exec(ui->songs->viewport()->mapToGlobal(pos));
}

void AlbumView::setupActions(){
    addSongToQ = new QAction(tr("&Add to queue"),this);
    goToSongArtist = new QAction(tr("&Go to artist"), this);
    goToAlbumArtist = new QAction(tr("&Go to artist"), this);
    like = new QAction(tr("&Like song"), this);

    connect(like, &QAction::triggered, this, &AlbumView::getSongToToggleLike);
    connect(addSongToQ, &QAction::triggered, this, &AlbumView::addToQue);
    connect(goToSongArtist, &QAction::triggered, this, &AlbumView::goToArtist);
}

void AlbumView::goToArtist(){
    QSqlQuery q;
    q.prepare("SELECT artist_id, artist_name FROM artist WHERE artist_id = (SELECT artist_id FROM album WHERE album_id = ?)");
    q.bindValue(0, ID);
    q.exec();
    q.next();
    int ID = q.value(0).toInt();
    QString name = q.value(1).toString();
    emit showArtist(ID, name);
}

void AlbumView::addToQue(){
    //if(!playList->empty()){
        QString url = songs_table_model->record(clickedRow).value(0).toString();
        int ID = songs_table_model->record(clickedRow).value(1).toInt();
        emit addToQ(ID, url);
    //}
}

void AlbumView::getSongToToggleLike(){
    emit likeUnlike(songs_table_model->record(clickedRow).value(1).toInt());
}
